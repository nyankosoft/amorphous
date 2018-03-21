#! /usr/bin/env bash

# The build script for the amorphous framework
# - This script does not clear external libs but clears only the object files of the amorphous source code and rebuilds the framework.

rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake Makefile libamorphous-core.a amorphous-core-test
cmake .
make

# Run unit tests
./amorphous-core-test

