# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(VRPProject LANGUAGES CXX)

# Set C++ standard to C++17 (required for OR-Tools and your code)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable optimization flags for release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Find OR-Tools package
find_package(ortools CONFIG REQUIRED)

# Find Protobuf (required by OR-Tools)
find_package(Protobuf REQUIRED)

# Include directories for Protobuf and OR-Tools
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${ORTOOLS_INCLUDE_DIRS})

# Add executable for nearest_n.cpp
add_executable(nearest_n nearest_n.cpp)
target_link_libraries(nearest_n PRIVATE ortools::ortools ${Protobuf_LIBRARIES})

# Add executable for vrp_solution.cpp
add_executable(vrp_solution vrp_solution.cpp)
target_link_libraries(vrp_solution PRIVATE ortools::ortools ${Protobuf_LIBRARIES})

# Add executable for ver_second_solution.cpp
add_executable(ver_second_solution ver_second_solution.cpp)
target_link_libraries(ver_second_solution PRIVATE ortools::ortools ${Protobuf_LIBRARIES})

# Optional: Specify output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)