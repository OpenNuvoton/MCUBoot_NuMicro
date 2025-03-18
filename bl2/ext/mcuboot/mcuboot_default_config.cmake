set(DEFAULT_MCUBOOT_FLASH_MAP       ON CACHE BOOL "Whether to use the default flash map")

set(MCUBOOT_LOG_LEVEL               "INFO" CACHE STRING "Level of logging to use for MCUboot [OFF, ERROR, WARNING, INFO, DEBUG]")
set(MCUBOOT_ENCRYPT_RSA             OFF CACHE BOOL "Use RSA for encrypted image upgrade support")

set(MCUBOOT_IMAGE_NUMBER            1 CACHE STRING "Number of images to be managed by MCUboot")
set(MCUBOOT_HW_KEY                  OFF CACHE BOOL "Whether to embed the entire public key in the image metadata instead of the hash only")
set(MCUBOOT_UPGRADE_STRATEGY        "OVERWRITE_ONLY" CACHE STRING "Upgrade strategy for images")
set(MCUBOOT_SIG_TYPE                "RSA" CACHE STRING "Algorithm to use for signature validation")
set(MCUBOOT_SIG_LEN                 "3072" CACHE STRING "Algorithm to use for signature validation")
set(MCUBOOT_ALIGN_VAL               16 CACHE STRING "align option for mcuboot and build image with imgtool [1, 2, 4, 8, 16, 32]")
set(MCUBOOT_IMAGE_EXEC_RAM_START    0x2000A000 CACHE STRING "Start address of predefined RAM-load area that are allowed to be used by executable images")
set(MCUBOOT_IMAGE_EXEC_RAM_SIZE     0x8000 CACHE STRING "Size of predefined RAM-load area that are allowed to be used by executable images")

# Specifying a scope of the accepted values of MCUBOOT_UPGRADE_STRATEGY for
# platforms to choose a specific upgrade strategy for images. These certain
# configurations will be used to facilitate the later validation.
set_property(CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS "OVERWRITE_ONLY;SWAP_USING_SCRATCH;SWAP_USING_MOVE;DIRECT_XIP;RAM_LOAD")

# Specifying a scope of the accepted values of MCUBOOT_ALIGN_VAL for
# platforms requiring specific flash alignmnent
set_property(CACHE MCUBOOT_ALIGN_VAL PROPERTY STRINGS "1;2;4;8;16;32")

set(MCUBOOT_MBEDTLS_CONFIG_FILEPATH "${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/config/mcuboot-mbedtls-cfg.h" CACHE FILEPATH "Mbed TLS config file to use with MCUboot")
