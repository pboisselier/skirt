########################################
# Small Kernel in Real-Time (SKIRT)    #
# Copyright (c) 2022 Pierre Boisselier #
########################################

cmake_minimum_required(VERSION 3.15)

## Set those if there are errors with CMake not being able to test the compiler (on macOS for instance)
################################################################
#set(CMAKE_CROSSCOMPILING 1)
#set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS 1)
#set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
################################################################

project(skirt VERSION 0.1 DESCRIPTION "Small Kernel in Real-Time (or not)")

if (NOT SKIRT_ARCH)
    message(FATAL_ERROR "Please set SKIRT_ARCH to specify a target architecture!\n"
            "Supported:\n"
            "avr\n"
            "rp2040\n")
endif ()

# Use provided GCC compilers
# NOTE: For the moment it only works with GCC!
if (DEFINED SKIRT_CC_PREFIX)
    set(CMAKE_C_COMPILER ${SKIRT_CC_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${SKIRT_CC_PREFIX}g++)
    set(CMAKE_AR ${SKIRT_CC_PREFIX}ar)
    set(CMAKE_RANLIB ${SKIRT_CC_PREFIX}ranlib)
endif ()

add_library(skirt STATIC
        src/sk/skirt.c
        src/sk/task.c
        src/sk/ipc.c

        # Add architecture-specific source files
        $<$<STREQUAL:${SKIRT_ARCH},avr>:
        src/sk/arch/avr/avr.c>)

target_compile_definitions(skirt PUBLIC SKIRT_KERNEL)
# Only static allocation available for now.
target_compile_definitions(skirt PUBLIC SKIRT_ALLOC_STATIC)
target_include_directories(skirt PUBLIC include)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(skirt PUBLIC SKIRT_DEBUG)
    target_compile_options(skirt PUBLIC -std=gnu11 -g -Os -pedantic -Wextra -Wall)
endif ()

if (SKIRT_ARCH STREQUAL "avr")
    if (NOT SKIRT_AVR_MCU)
        message(FATAL_ERROR "Please set SKIRT_AVR_MCU to specify which MCU is targeted (only atmega328p for the moment)!\n")
    else ()
        message(VERBOSE "Using AVR MCU: ${SKIRT_AVR_MCU}")
    endif ()

    if (NOT SKIRT_CC_PREFIX)
        # Override compilers
        set(CMAKE_C_COMPILER avr-gcc)
        set(CMAKE_CXX_COMPILER avr-g++)
        set(CMAKE_AR avr-ar)
        set(CMAKE_RANLIB avr-ranlib)
    endif ()
    # Add AVR-specific includes
    target_include_directories(skirt PUBLIC include/arch/avr)
    target_compile_options(skirt PUBLIC -mmcu=${SKIRT_AVR_MCU})
    target_link_options(skirt PUBLIC -mmcu=${SKIRT_AVR_MCU})
endif ()


# TODO: Remove this test and maybe write true tests
add_executable(test.elf src/test/test.c)
target_include_directories(test.elf PUBLIC include)
target_compile_options(test.elf PUBLIC -mmcu=${SKIRT_AVR_MCU})
target_compile_options(test.elf PUBLIC -fno-fat-lto-objects -ffunction-sections -fdata-sections -flto --pedantic)
target_link_options(test.elf PUBLIC -mmcu=${SKIRT_AVR_MCU})
target_link_libraries(test.elf skirt)