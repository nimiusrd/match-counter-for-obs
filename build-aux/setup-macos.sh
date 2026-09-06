#!/bin/bash
set -euo pipefail

if [[ "$(uname -s)" != Darwin ]]; then
  echo 'This setup script requires macOS and Xcode 16.0 or newer.' >&2
  exit 1
fi

for tool in cmake xcodebuild xcrun; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "Missing required tool: $tool. See README.md for prerequisites." >&2
    exit 1
  fi
done

if ! xcodebuild -checkFirstLaunchStatus; then
  echo 'Complete Xcode setup with xcodebuild -runFirstLaunch, then retry.' >&2
  exit 1
fi

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$project_dir"
export SDKROOT="$(xcrun --sdk macosx --show-sdk-path)"
cmake --preset macos -DCMAKE_OSX_SYSROOT="$SDKROOT"
