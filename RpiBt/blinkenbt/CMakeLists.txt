cmake_minimum_required(VERSION 3.10)
project(blinkenbt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-o2 -march=native")

# main program #
add_executable(blinkenbt main.cpp lib/gpio.cpp lib/btcon.cpp lib/xmodem.cpp lib/handle.cpp)
TARGET_LINK_LIBRARIES(blinkenbt bluetooth pthread)
