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
if (PLATFORM_NAME STREQUAL "m2l31")
set (NVMC_TYPE_RMC 1)
set (NVMC_DRV_SRC "common/rmc_drv.c")
else ()
set (NVMC_TYPE_RMC 0)
set (NVMC_DRV_SRC "common/fmc_drv.c")
endif ()

if (SECONDARY_SLOT_IN_SPI_FLASH EQUAL 1)
if (PLATFORM_NAME STREQUAL "m460")
set (SPI_FLASH_M460 1)
endif ()
target_sources(platform_bl2_lib PRIVATE common/spi_drv.c)
endif()

target_sources(platform_bl2_lib
    PRIVATE
        common/Driver_Flash.c
        common/uart_stdout.c
        common/syscall.c
        common/boot.c
        ${NVMC_DRV_SRC}
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
        $<$<BOOL:${NVMC_TYPE_RMC}>:NVMC_TYPE_RMC=${NVMC_TYPE_RMC}>
        $<$<BOOL:${SPI_FLASH_M460}>:SPI_FLASH_M460=${SPI_FLASH_M460}>
)

######################### Platform Region Defs #################################

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
        $<$<BOOL:${MCUBOOT_BUILTIN_KEY}>:MCUBOOT_BUILTIN_KEY>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},RSA>:MCUBOOT_SIGN_RSA>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},RSA>:MCUBOOT_SIGN_RSA_LEN=${MCUBOOT_SIG_LEN}>
        $<$<STREQUAL:${MCUBOOT_SIG_TYPE},EC>:MCUBOOT_SIGN_EC${MCUBOOT_SIG_LEN}>
        $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},RAM_LOAD>:IMAGE_EXECUTABLE_RAM_START=${MCUBOOT_IMAGE_EXEC_RAM_START}>
        $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},RAM_LOAD>:IMAGE_EXECUTABLE_RAM_SIZE=${MCUBOOT_IMAGE_EXEC_RAM_SIZE}>
        $<$<BOOL:${SECONDARY_SLOT_IN_SPI_FLASH}>:SECONDARY_SLOT_IN_SPI_FLASH=${SECONDARY_SLOT_IN_SPI_FLASH}>
)
