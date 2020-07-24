cmake_minimum_required(VERSION 3.10)
project(Pi)

set(CMAKE_CXX_STANDARD 14)

add_executable(Pi main.cpp xmodem.cpp)

TARGET_LINK_LIBRARIES(Pi bluetooth)