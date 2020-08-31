cmake_minimum_required(VERSION 3.16.3)

include(${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/CMakeLists.txt)

project(samc21_blink)
set(CMAKE_VERBOSE_MAKEFILE true)
add_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} samc21_bootloader)

add_definitions(-D__SAMC21J18A__)

target_sources(${PROJECT_NAME} PUBLIC
        image.h
        image.c
        src/main.cpp
        $<TARGET_OBJECTS:samc21_cmsis>
        )

target_link_options(${PROJECT_NAME} PRIVATE LINKER:-lm,-gc-sections,--print-memory-usage,--build-id=md5)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-L ${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/gcc)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-T ${CMAKE_CURRENT_LIST_DIR}/samc21_bootloader/gcc/app.ld)
target_link_options(${PROJECT_NAME} PRIVATE LINKER:-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "output ${PROJECT_NAME} lss"
        COMMAND ${NM} -S --demangle --numeric-sort ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lss
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        BYPRODUCTS ${PROJECT_NAME}.lss
        )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "Build ID"
        DEPENDS ${PROJECT_NAME}.elf
        COMMAND ${READELF} -n ${PROJECT_NAME}.elf | grep \"Build ID:\" | cut -d \\: -f 2 | sed 's/ //')

add_custom_command(OUTPUT ${PROJECT_NAME}.bin
        COMMENT "output ${PROJECT_NAME} bin"
        COMMAND ${OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
        DEPENDS ${PROJECT_NAME}
        BYPRODUCTS ${PROJECT_NAME}.bin
        )

add_custom_command(OUTPUT ${PROJECT_NAME}.patch.bin
        COMMENT "patching image"
        COMMAND ${CMAKE_CURRENT_LIST_DIR}/patch_image.sh ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
        DEPENDS ${PROJECT_NAME}.bin
        BYPRODUCTS ${PROJECT_NAME}.patch.bin
        )

add_custom_command(OUTPUT ${PROJECT_NAME}.bootloaded.bin
        COMMENT "concatenating ${PROJECT_NAME} binary images"
        COMMAND cat ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.patch.bin > ${PROJECT_NAME}.bootloaded.bin
        DEPENDS samc21_bootloader.padded.bin ${PROJECT_NAME}.patch.bin
        BYPRODUCTS ${PROJECT_NAME}.bootloaded.bin
        )
# the "bootloaded" target is called with ALL
# it depends on a single file (the output file)
# which is created by the chain of custom commands above
add_custom_target(bootloaded ALL
        DEPENDS ${PROJECT_NAME}.bootloaded.bin)


# TODO for reference
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMENT "output ${PROJECT_NAME} S-record"
#        COMMAND ${OBJCOPY} -O srec ${PROJECT_NAME}.elf ${PROJECT_NAME}.srec
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#        BYPRODUCTS ${PROJECT_NAME}.srec ${PROJECT_NAME}.padded.srec
#        )

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMENT "output ${PROJECT_NAME} hex"
#        COMMAND ${OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#        BYPRODUCTS ${PROJECT_NAME}.hex
#        )

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMENT "concatenating ${PROJECT_NAME} S-records"
#        COMMAND cat ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.srec ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.srec > ${PROJECT_NAME}.bootloaded.srec
#        DEPENDS ${CMAKE_BINARY_DIR}/samc21_bootloader.padded.srec ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.srec
#        BYPRODUCTS ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bootloaded.srec
#        )

