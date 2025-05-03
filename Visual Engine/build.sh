#!/bin/bash

rm -rf build
mkdir build
cd build
cmake -S .. -B .
cmake --build . 