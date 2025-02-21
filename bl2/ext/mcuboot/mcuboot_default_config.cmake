set(DEFAULT_MCUBOOT_FLASH_MAP       ON CACHE BOOL "Whether to use the default flash map")

set(MCUBOOT_LOG_LEVEL               "INFO" CACHE STRING "Level of logging to use for MCUboot [OFF, ERROR, WARNING, INFO, DEBUG]")

set(MCUBOOT_SIG_TYPE                "RSA" CACHE STRING "Algorithm to use for signature validation")
set(MCUBOOT_SIG_LEN                 "3072" CACHE STRING "Algorithm to use for signature validation")
set(MCUBOOT_ALIGN_VAL               1 CACHE STRING "align option for mcuboot and build image with imgtool [1, 2, 4, 8, 16, 32]")

set(MCUBOOT_MBEDTLS_CONFIG_FILEPATH "${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/config/mcuboot-mbedtls-cfg.h" CACHE FILEPATH "Mbed TLS config file to use with MCUboot")
