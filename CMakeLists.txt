cmake_minimum_required(VERSION 3.12)
include(ExternalProject)

# For simplicity of setup, just set defaults here. Can be overridden in IDE project settings or CLI
set(TARGET "PICO" CACHE STRING "Target hardware. Change here or via CMake options to Simulator to build the sim.")
set(MICROPY OFF CACHE BOOL "Micropython build.")

if (${TARGET} STREQUAL "PICO")
    set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk" CACHE PATH "Path to the Raspberry Pi Pico SDK" FORCE)
    set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
    if (NOT EXISTS ${PICO_SDK_INIT_CMAKE_FILE})
        message(FATAL_ERROR "Directory '${PICO_SDK_PATH}' does not appear to contain the Raspberry Pi Pico SDK")
    endif ()

    include(${PICO_SDK_INIT_CMAKE_FILE})
endif()

project(badge2022_c C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (MICROPY)
    if (${TARGET} STREQUAL "PICO")
        # Micropython build targets are not very fond of being added as a subdirectory. it *almost* works.
        # So instead we register it as an external command and run it.
        ExternalProject_Add(
                micropython
                SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/micropython/ports/rp2
                CMAKE_ARGS -DUSER_C_MODULES=${CMAKE_SOURCE_DIR}/source/micropython.cmake
                INSTALL_COMMAND true # skip this
                BUILD_BYPRODUCTS
        )
        ExternalProject_Get_Property(micropython BINARY_DIR)
        add_custom_command(
                TARGET micropython POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${BINARY_DIR}/firmware.uf2
                ${CMAKE_CURRENT_BINARY_DIR}/micropython.uf2)

    elseif(${TARGET} STREQUAL "SIMULATOR")

    endif()
else()
    # Initialize the SDK
    if (${TARGET} STREQUAL "PICO")
        add_compile_definitions(TARGET_PICO)
        pico_sdk_init()
    elseif(${TARGET} STREQUAL "SIMULATOR")
        add_compile_definitions(TARGET_SIMULATOR)
    endif()


    add_compile_options(-Wall)

    if (${TARGET} STREQUAL "PICO")
        add_compile_options(
                -Wno-format          # pico-examples: int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
                -Wno-unused-function # pico-examples: we have some for the docs that aren't called
                -Wno-maybe-uninitialized # sam: see if this causes extra warnings outside of pico-examples
        )
    endif()

    add_subdirectory(source)
endif()
