add_library(platform_bl2_lib STATIC)
add_library(platform_region_defs INTERFACE)
add_library(platform_common_interface INTERFACE)

add_subdirectory(${PLATFORM_NAME})

######################### Platform Common Interface ############################

target_include_directories(platform_common_interface
    INTERFACE
        common
        driver
        include
)

######################### Platform BL2 #########################################

# target_include_directories(platform_bl2_lib
#     PUBLIC
# )

target_sources(platform_bl2_lib
    PRIVATE
        common/Driver_Flash.c
        common/uart_stdout.c
        common/exit.c
        common/boot.c
)

target_link_libraries(platform_bl2_lib
    PUBLIC
        platform_common_interface
        platform_region_defs
)

target_compile_definitions(platform_bl2_lib
    PUBLIC
        MCUBOOT_${MCUBOOT_UPGRADE_STRATEGY}
        $<$<BOOL:${MCUBOOT_HW_KEY}>:MCUBOOT_HW_KEY>
)

######################### Platform Region Defs #################################

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
        $<$<BOOL:${MCUBOOT_BUILTIN_KEY}>:MCUBOOT_BUILTIN_KEY>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},RSA>:MCUBOOT_SIGN_RSA>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},RSA>:MCUBOOT_SIGN_RSA_LEN=${MCUBOOT_SIG_LEN}>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},EC>:MCUBOOT_SIGN_EC${MCUBOOT_SIG_LEN}>
)
