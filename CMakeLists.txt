cmake_minimum_required(VERSION 3.21)

######################### CONFIGURATION ########################################

include(${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/mcuboot_default_config.cmake)
include(${CMAKE_SOURCE_DIR}/config/config_base.cmake)

######################### Compiler configuration ###############################

project(MCUBoot VERSION 1.0
                LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(lib/ext)
add_subdirectory(bl2)
