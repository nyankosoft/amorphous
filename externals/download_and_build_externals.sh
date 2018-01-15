#!/usr/bin/env bash

# GLEW
wget https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.tgz
tar -xvf glew-2.1.0.tgz
cd glew-2.1.0
make
cd ..

# googletest
wget -O googletest-release-1.8.0.tar.gz https://github.com/google/googletest/archive/release-1.8.0.tar.gz
tar -xvf googletest-release-1.8.0.tar.gz
cd googletest-release-1.8.0
cmake .
make
cd ..

# FreeType
wget https://download.savannah.gnu.org/releases/freetype/freetype-2.8.1.tar.gz
tar -xvf freetype-2.8.1.tar.gz
cd freetype-2.8.1

#>>> Commented out

## CMake does not allow what they call 'in-source builds', and causes an error saying
## "In-source builds are not permitted! Make a separate folder for building, ..."
## if we simply run 'cmake .' in the extracted freetype-x.x.x directory
##cmake .

## Therefore we create a new directory and run cmake from there.
#mkdir cmakebuild
#cd cmakebuild
#cmake ..
#make
#cd ..
#cd ..

#<<< Commented out

# For now, we go for the traditional ./configure + make method;
# libfreetype.a built via cmake & make caused undefined referrence
# errors with libpng functions, such as 'png_create_read_struct' and
# 'png_create_read_struct'. Their libfreetype.a was much smaller (1.2MB)
# than (6.3MB)

./configure
make
cd ..

