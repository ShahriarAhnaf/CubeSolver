#!/bin/bash

# Check for --fresh argument
if [ "$1" == "--fresh" ]; then
    echo "Performing fresh build: Removing existing build directory..."
    rm -rf build
fi

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake (always reconfigure on fresh build or if not configured)
if [ ! -f "CMakeCache.txt" ] || [ "$1" == "--fresh" ]; then
    echo "Configuring CMake..."
    cmake ..
fi

# Build the project (use all available CPU cores)
echo "Building project..."
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# Return to original directory
cd .. 