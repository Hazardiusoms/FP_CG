#!/bin/zsh
set -eu
cd -- "${0:A:h}"
export PATH="/opt/homebrew/bin:/usr/local/bin:$PATH"
if ! command -v cmake >/dev/null 2>&1; then
  print 'CMake is missing. Install the dependencies described in README.md.'
  read '?Press Enter to close.'
  exit 1
fi
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
exec ./build/synthwave_drive
