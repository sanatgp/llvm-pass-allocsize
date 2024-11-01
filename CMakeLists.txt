cmake_minimum_required(VERSION 3.6)
project(alloc_size_pass)

# LLVM uses C++17.
set(CMAKE_CXX_STANDARD 17)

# Find the LLVM package.
find_package(LLVM REQUIRED CONFIG)

# Include the necessary LLVM libraries.
include(AddLLVM)

# Link LLVM definitions.
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

add_llvm_pass_plugin(alloc_size_pass allocpass/Allocpass.cpp)
