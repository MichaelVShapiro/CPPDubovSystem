cmake_minimum_required(VERSION 3.16)

# Project name and version
project(CPPDubovSystem VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Define source files based on build.sh
set(SOURCES
    DubovSystem/main.cpp
    "DubovSystem/graph util/Graph.cpp"
    "DubovSystem/graph util/BinaryHeap.cpp"
    "DubovSystem/graph util/Matching.cpp"
    "DubovSystem/csv util/csv.cpp"
    DubovSystem/fpc.cpp
    "DubovSystem/trf util/trf.cpp"
    "DubovSystem/trf util/rtg.cpp"
    DubovSystem/Player.cpp
    DubovSystem/Tournament.cpp
    DubovSystem/baku.cpp
    DubovSystem/LinkedList.cpp
)

# Create the executable
add_executable(CPPDubovSystem ${SOURCES})

# Set include directories
target_include_directories(CPPDubovSystem PRIVATE
    DubovSystem
    "DubovSystem/graph util"
    "DubovSystem/csv util"
    "DubovSystem/trf util"
)

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Using Clang++ compiler")
    # Add any Clang-specific flags if needed
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using g++ compiler")
    # Add any GCC-specific flags if needed
endif()

