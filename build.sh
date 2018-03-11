#!/usr/bin/env bash

# Download and build externals
cd externals
chmod u+x download_and_build_externals.sh
./download_and_build_externals.sh
cd ..

# Build the framework
cd framework/cmake
cmake .
make

