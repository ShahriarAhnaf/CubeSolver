#!/bin/bash

# Check for --fresh argument
if [ "$1" == "--fresh" ]; then
    echo "Performing fresh build: Removing existing build directory..."
    rm -rf build
fi

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake (always reconfigure on fresh build)
echo "Configuring CMake..."
cmake -S .. -B .

# Build the project
echo "Building project..."
cmake --build .

# This make clean seems redundant if we just configured/built
# make clean 

# Run make again? This also seems redundant after cmake --build .
# make -j$(nproc)

echo "Build complete. Executable is in ./bin/RubiksCubeVisualizer"

# Optionally, run the executable after build
# ./bin/RubiksCubeVisualizer