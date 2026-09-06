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
  add_executable(match-counter-tests tests/match-counter-test.c tests/obs-util-stubs.c src/match-counter.c)
  get_target_property(_libobs_include_dirs OBS::libobs INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(match-counter-tests PRIVATE src)
  target_include_directories(match-counter-tests SYSTEM PRIVATE ${_libobs_include_dirs})
  unset(_libobs_include_dirs)

  add_test(NAME match-counter-tests COMMAND match-counter-tests)
endif()

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
