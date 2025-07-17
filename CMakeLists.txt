cmake_minimum_required(VERSION 3.10)

project(PLUTO VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the main executable
add_executable(PLUTO main.cpp)
