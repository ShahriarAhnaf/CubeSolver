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

# --- Visual Engine ---
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

include(FetchContent)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)

find_package(OpenGL REQUIRED)

add_executable(RubiksCubeVisualizer
    "Visual Engine/src/main.cpp"
    "Visual Engine/src/CubeRenderer.cpp"
)
target_include_directories(RubiksCubeVisualizer PRIVATE
    ${OPENGL_INCLUDE_DIR}
    "Visual Engine/include"
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
)
find_package(Threads REQUIRED)
target_link_libraries(RubiksCubeVisualizer PRIVATE
    CubeCore
    Threads::Threads
    ${OPENGL_LIBRARIES}
    glfw
    glad
)

# Copy shaders to build directory
add_custom_command(TARGET RubiksCubeVisualizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Visual\ Engine/shaders
    ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

# --- Terminal app (ncurses) ---
find_package(Curses)
if(CURSES_FOUND)
    add_executable(CubeSolver Rubix/src/main.cpp Rubix/src/Draw.cpp)
    target_include_directories(CubeSolver PRIVATE Rubix/Inc ${CURSES_INCLUDE_DIRS})
    target_link_libraries(CubeSolver PRIVATE CubeCore ${CURSES_LIBRARIES})
endif()

# --- Tests (no ncurses needed) ---
add_executable(Test_Cube
    Rubix/Testing/Test_Cube.cpp
)
target_link_libraries(Test_Cube PRIVATE CubeCore)

# --- Custom targets ---
add_custom_target(run-visual
    COMMAND RubiksCubeVisualizer
    DEPENDS RubiksCubeVisualizer
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run-tests
    COMMAND Test_Cube
    DEPENDS Test_Cube
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

message(STATUS "CubeSolver unified build")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
