cmake_minimum_required(VERSION 3.28...3.30)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

include(CTest)

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${CMAKE_PROJECT_NAME} MODULE)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/plugin-main.c src/match-counter.c)

if(BUILD_TESTING)
  add_executable(match-counter-tests tests/match-counter-test.c src/match-counter.c)
  target_include_directories(match-counter-tests PRIVATE src)
  target_link_libraries(match-counter-tests PRIVATE OBS::libobs)

  if(APPLE)
    set_target_properties(match-counter-tests PROPERTIES XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME NO)
  elseif(WIN32)
    add_custom_command(
      TARGET match-counter-tests
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:match-counter-tests>
        $<TARGET_FILE_DIR:match-counter-tests>
      COMMAND_EXPAND_LISTS
    )
  endif()

  add_test(NAME match-counter-tests COMMAND match-counter-tests)
  if(APPLE)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/buildspec.json" _buildspec)
    string(JSON _prebuilt_version GET ${_buildspec} dependencies prebuilt version)
    set_tests_properties(
      match-counter-tests
      PROPERTIES
        ENVIRONMENT
          "DYLD_FRAMEWORK_PATH=${CMAKE_CURRENT_SOURCE_DIR}/.deps/Frameworks;DYLD_LIBRARY_PATH=${CMAKE_CURRENT_SOURCE_DIR}/.deps/obs-deps-${_prebuilt_version}-universal/lib"
    )
    unset(_buildspec)
    unset(_prebuilt_version)
  endif()
endif()

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
