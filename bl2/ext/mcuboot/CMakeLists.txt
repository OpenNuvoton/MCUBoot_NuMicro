add_library(mcuboot_config INTERFACE)

target_include_directories(mcuboot_config
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for mcuboot_config.h only
)

######################### MCUBOOT ##############################################

target_include_directories(mcuboot_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mcuboot_lib
    PUBLIC
        mcuboot_config
        platform_bl2_lib
        mbedtls_lib
)

target_compile_definitions(mcuboot_lib
    PRIVATE
        $<$<BOOL:${DEFAULT_MCUBOOT_FLASH_MAP}>:DEFAULT_MCUBOOT_FLASH_MAP>
)

######################### BL2 ##################################################

target_include_directories(bl2
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(bl2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bl2_main.c
)

target_link_libraries(bl2
    PUBLIC
        mcuboot_lib
)

# Returns the index of the element specified in the list
set(MCUBOOT_ALLOWED_LOG_LEVELS OFF ERROR WARNING INFO DEBUG)
list(FIND MCUBOOT_ALLOWED_LOG_LEVELS ${MCUBOOT_LOG_LEVEL} LOG_LEVEL_ID)

if (MCUBOOT_ALIGN_VAL GREATER 8)
set (MCUBOOT_BOOT_MAX_ALIGN ${MCUBOOT_ALIGN_VAL})
else()
set (MCUBOOT_BOOT_MAX_ALIGN 8)
endif()

configure_file(include/mcuboot_config/mcuboot_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/mcuboot_config/mcuboot_config.h)
