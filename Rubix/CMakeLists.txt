cmake_minimum_required(VERSION 3.15)
project(RubixCubeSolver VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# Add source files
set(SOURCES
    src/Cube.cpp
    src/Solver.cpp
    main.cpp
)

# Add header files
set(HEADERS
    Inc/Cube.h
    Inc/Solver.h
)

# Create main executable
add_executable(CubeSolver ${SOURCES} ${HEADERS})

# Add testing executable if testing is enabled
option(BUILD_TESTING "Build test executable" ON)
if(BUILD_TESTING)
    add_executable(Test_Cube
        Testing/Test_Cube.cpp
        src/Cube.cpp
        src/Solver.cpp
    )
    target_include_directories(Test_Cube PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
endif()

# Installation configuration
install(TARGETS CubeSolver
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(BUILD_TESTING)
    install(TARGETS Test_Cube
        RUNTIME DESTINATION bin
    )
endif()

# Add custom targets for common operations
add_custom_target(run
    COMMAND CubeSolver
    DEPENDS CubeSolver
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run-tests
    COMMAND Test_Cube
    DEPENDS Test_Cube
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
)

# Add build type selection
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Output configuration
message(STATUS "Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Testing enabled: ${BUILD_TESTING}")

# Add build instructions to README
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md.in
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    @ONLY
) 