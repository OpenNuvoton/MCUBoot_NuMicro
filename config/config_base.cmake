set(PLATFORM_NAME "m2354" CACHE STRING "Platform to build MCUBOOT for")
set(NU_MCUBOOT_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/ext/mcubootlib CACHE PATH "Path to parent directory of MCUBoot")
set(NU_MCUBOOT_PATH ${NU_MCUBOOT_LIB_PATH}/mcuboot CACHE PATH "Path to MCUBoot")
set(NU_MBEDTLS_LIB_PATH ${CMAKE_SOURCE_DIR}/lib/ext/mbedtlslib CACHE PATH "Path to parent directory of MbedTLS")
set(NU_MBEDTLS_PATH ${NU_MBEDTLS_LIB_PATH}/mbedtls CACHE PATH "Path to MbedTLS")