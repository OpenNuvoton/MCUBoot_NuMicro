add_executable(bl2
    src/flash_map.c
    $<$<BOOL:${DEFAULT_MCUBOOT_FLASH_MAP}>:src/default_flash_map.c>
)

add_subdirectory(ext/mcuboot)
add_subdirectory(platform)

target_include_directories(bl2
    PRIVATE
        $<BUILD_INTERFACE:${NU_MCUBOOT_PATH}/boot/bootutil/src>
)

target_link_libraries(bl2
    PRIVATE
        platform_bl2_lib
)

target_compile_definitions(bl2
    PRIVATE
        $<$<BOOL:${DEFAULT_MCUBOOT_FLASH_MAP}>:DEFAULT_MCUBOOT_FLASH_MAP>
)

if (CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
target_link_options(bl2
    PRIVATE
        --library_type=microlib
        --info=sizes,totals
        --symbols
        --map
        --scatter=${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM_NAME}/scatter/${PLATFORM_NAME}.sct
)

add_custom_command(TARGET bl2
        POST_BUILD
        COMMAND ${ARM_ELF2BIN} --bin ${EXECUTABLE_OUTPUT_PATH}/bl2.elf --output ${EXECUTABLE_OUTPUT_PATH}/bl2.bin)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_link_options(bl2
    PRIVATE
        -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/bl2.map
        -T${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM_NAME}/scatter/${PLATFORM_NAME}.ld
        -lc
        -Wl,--print-memory-usage
)

add_custom_command(TARGET bl2
                   POST_BUILD
                   COMMAND ${GCC_ELF2BIN} -O binary ${EXECUTABLE_OUTPUT_PATH}/bl2 ${EXECUTABLE_OUTPUT_PATH}/bl2.bin)
endif()

######################### MBEDTLS ##############################################

add_library(mbedtls_config INTERFACE)

target_compile_definitions(mbedtls_config
    INTERFACE
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},RSA>:MCUBOOT_SIGN_RSA>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},RSA>:MCUBOOT_SIGN_RSA_LEN=${MCUBOOT_SIG_LEN}>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},EC>:MCUBOOT_SIGN_EC${MCUBOOT_SIG_LEN}>
        MBEDTLS_CONFIG_FILE="${MCUBOOT_MBEDTLS_CONFIG_FILEPATH}"
)
