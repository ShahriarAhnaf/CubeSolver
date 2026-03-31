cmake_minimum_required(VERSION 3.15)
project(CubeSolver VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# --- CubeCore: shared static library (no UI dependencies) ---
add_library(CubeCore STATIC
    Core/src/Cube.cpp
    Core/src/Solver.cpp
)
target_include_directories(CubeCore PUBLIC Core/Inc)

# --- Tests (no ncurses needed) ---
add_executable(Test_Cube
    Rubix/Testing/Test_Cube.cpp
)
target_link_libraries(Test_Cube PRIVATE CubeCore)

# --- Custom targets ---
add_custom_target(run-tests
    COMMAND Test_Cube
    DEPENDS Test_Cube
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

message(STATUS "CubeSolver unified build")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
