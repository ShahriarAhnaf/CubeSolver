#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake if not already configured
if [ ! -f "CMakeCache.txt" ]; then
    cmake ..
fi

# Build the project (use all available CPU cores)
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# Return to original directory
cd .. 