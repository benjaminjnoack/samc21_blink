cmake_minimum_required(VERSION 3.17)

include(${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/CMakeLists.txt)

project(samc21_blink)
set(CMAKE_VERBOSE_MAKEFILE true)
add_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} samc21_bootloader)

add_definitions(-D__SAMC21J18A__)

target_sources(${PROJECT_NAME} PUBLIC
        src/main.cpp
        $<TARGET_OBJECTS:samc21_cmsis>
        )

target_link_options(${PROJECT_NAME} PRIVATE LINKER:-lm,-gc-sections,--print-memory-usage)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-L ${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/gcc)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-T ${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/gcc/app.ld)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "output assembly"
        COMMAND arm-none-eabi-objdump -d ${PROJECT_NAME}.elf > ${PROJECT_NAME}.s
        COMMAND arm-none-eabi-objdump -D --dwarf=follow-links ${PROJECT_NAME}.elf > ${PROJECT_NAME}.S
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        BYPRODUCTS ${PROJECT_NAME}.s ${PROJECT_NAME}.S
        )

# When objcopy generates a raw binary file, it will essentially produce
# a memory dump of the contents of the input object file. All symbols and relocation information will be discarded.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "output ${PROJECT_NAME} bin"
        COMMAND ${OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        BYPRODUCTS ${PROJECT_NAME}.bin
        )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "output ${PROJECT_NAME} S-records"
        COMMAND ${OBJCOPY} -O srec ${PROJECT_NAME}.elf ${PROJECT_NAME}.srec
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        BYPRODUCTS ${PROJECT_NAME}.srec ${PROJECT_NAME}.padded.srec
        )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "output ${PROJECT_NAME} hex"
        COMMAND ${OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "output ${PROJECT_NAME} lss"
        COMMAND ${NM} -S --demangle --numeric-sort ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lss
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "concatenating ${PROJECT_NAME} binary images"
        COMMAND cat ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin  > ${PROJECT_NAME}_bootloaded.bin
        DEPENDS ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_bootloaded.bin
        )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "concatenating ${PROJECT_NAME} S-records"
        COMMAND cat ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.srec ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.srec  > ${PROJECT_NAME}_bootloaded.srec
        DEPENDS ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.srec ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.srec
        BYPRODUCTS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_bootloaded.srec
        )
