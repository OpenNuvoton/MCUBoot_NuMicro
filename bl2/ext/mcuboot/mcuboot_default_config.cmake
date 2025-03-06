set(DEFAULT_MCUBOOT_FLASH_MAP       ON CACHE BOOL "Whether to use the default flash map")

set(MCUBOOT_LOG_LEVEL               "INFO" CACHE STRING "Level of logging to use for MCUboot [OFF, ERROR, WARNING, INFO, DEBUG]")
set(MCUBOOT_ENCRYPT_RSA             OFF CACHE BOOL "Use RSA for encrypted image upgrade support")

set(MCUBOOT_IMAGE_NUMBER            1 CACHE STRING "Number of images to be managed by MCUboot")
set(MCUBOOT_HW_KEY                  OFF CACHE BOOL "Whether to embed the entire public key in the image metadata instead of the hash only")
set(MCUBOOT_UPGRADE_STRATEGY        "OVERWRITE_ONLY" CACHE STRING "Upgrade strategy for images")
set(MCUBOOT_SIG_TYPE                "RSA" CACHE STRING "Algorithm to use for signature validation")
set(MCUBOOT_SIG_LEN                 "3072" CACHE STRING "Algorithm to use for signature validation")
set(MCUBOOT_ALIGN_VAL               1 CACHE STRING "align option for mcuboot and build image with imgtool [1, 2, 4, 8, 16, 32]")

set(MCUBOOT_MBEDTLS_CONFIG_FILEPATH "${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/config/mcuboot-mbedtls-cfg.h" CACHE FILEPATH "Mbed TLS config file to use with MCUboot")
