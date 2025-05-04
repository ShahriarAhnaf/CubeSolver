#!/bin/bash
cd build
cmake -S .. -B .
cmake --build .
make clean
make -j$(nproc)