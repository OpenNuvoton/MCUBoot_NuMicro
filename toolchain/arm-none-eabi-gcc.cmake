# Copyright (c) 2020-2023 Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Set CMAKE_MODULE_PATH so CMake can find our platform files
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Modules" ${CMAKE_MODULE_PATH})

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

set(GCC_ELF2BIN "arm-none-eabi-objcopy")
set_property(GLOBAL PROPERTY ELF2BIN ${GCC_ELF2BIN})
