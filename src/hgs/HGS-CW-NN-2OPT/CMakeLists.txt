cmake_minimum_required(VERSION 3.15)
project(HGS_CVRP VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -O3)
elseif (MSVC)
    add_compile_options(/W4 /O2)
endif()

# Source files
set(src_files
    Program/Genetic.cpp
    Program/Individual.cpp
    Program/LocalSearch.cpp
    Program/Params.cpp
    Program/Population.cpp
    Program/Split.cpp
    Program/InstanceCVRPLIB.cpp
    Program/AlgorithmParameters.cpp
    Program/C_Interface.cpp
)

# Include directories
include_directories(Program)

# Object library for shared compilation
add_library(objlib OBJECT ${src_files})
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE ON)

# Static library
add_library(hgs_cvrp_static STATIC $<TARGET_OBJECTS:objlib>)
set_target_properties(hgs_cvrp_static PROPERTIES OUTPUT_NAME hgs_cvrp)

# Shared library
add_library(hgs_cvrp_shared SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties(hgs_cvrp_shared PROPERTIES OUTPUT_NAME hgs_cvrp)
if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Executable (assuming a main.cpp exists)
# Adjust the path to your main file if different
add_executable(hgs_cvrp Program/main.cpp)
target_link_libraries(hgs_cvrp hgs_cvrp_static)

# Install targets (optional, for deployment)
install(TARGETS hgs_cvrp hgs_cvrp_static hgs_cvrp_shared
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Optional: Add custom target for cleaning
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMENT "Clean all build artifacts"
)
