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
find_package(ortools CONFIG)

# If OR-Tools is not found via find_package, specify paths manually (uncomment and adjust if needed)
# set(ORTOOLS_ROOT /usr/local/or-tools)
# find_library(ORTOOLS_LIBRARY NAMES ortools PATHS ${ORTOOLS_ROOT}/lib)
# include_directories(${ORTOOLS_ROOT}/include)

# Find Protobuf (required by OR-Tools)
find_package(Protobuf REQUIRED)

# Include directories for Protobuf
include_directories(${Protobuf_INCLUDE_DIRS})

# Executable for nearest_n.cpp
add_executable(nearest_n nearest_n.cpp)

# Executable for vrp_solution.cpp
add_executable(vrp_solution vrp_solution.cpp)

# Link libraries for vrp_solution (OR-Tools and Protobuf)
if (ortools_FOUND)
    target_link_libraries(vrp_solution PRIVATE ortools::ortools ${Protobuf_LIBRARIES})
else()
    message(WARNING "OR-Tools not found via find_package. Attempting manual linking.")
    target_link_libraries(vrp_solution PRIVATE ${ORTOOLS_LIBRARY} ${Protobuf_LIBRARIES})
endif()

# Ensure the txt directory is copied to the build directory (optional, for runtime access)
file(COPY ${CMAKE_SOURCE_DIR}/txt DESTINATION ${CMAKE_BINARY_DIR})

# Install targets (optional, for installing executables)
install(TARGETS nearest_n vrp_solution DESTINATION bin)
