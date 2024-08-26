cmake_minimum_required(VERSION 3.4.1)

# Define the project name
project(MyRaylibApp)

# Add raylib as a subdirectory
add_subdirectory(raylib)

# Add executable target
add_executable(MyRaylibApp main.cpp)

# Link raylib library
target_link_libraries(MyRaylibApp raylib)

# Include directories
include_directories(raylib/src)
