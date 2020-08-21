cmake_minimum_required(VERSION 3.17)

include(${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/samc21_cmsis/samc21_cmsis.cmake)

project(samc21_blink)

add_executable(samc21_blink src/main.cpp)