cmake_minimum_required(VERSION 3.17)

include(${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/samc21_cmsis/samc21_cmsis.cmake)

project(samc21_blink)
set(CMAKE_VERBOSE_MAKEFILE true)
add_executable(${PROJECT_NAME})

add_definitions(-D__SAMC21J18A__)

target_sources(${PROJECT_NAME} PUBLIC
        src/main.cpp
        $<TARGET_OBJECTS:samc21_cmsis>
        )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-lm,-gc-sections,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/samc21_bootloader/gcc/app.ld -L ${CMAKE_CURRENT_SOURCE_DIR}/samc21_bootloader/gcc")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.Map")

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")