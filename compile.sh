#!/bin/bash

# Create the build directory if it doesn't exist
mkdir -p build

# Move to the build directory
cd build

# Run CMake to configure the project
cmake ..

# Run Make to compile the project
make
