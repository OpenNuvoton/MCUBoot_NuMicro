add_library(mcuboot_lib STATIC)

set(MCUBOOT_BOOTUTIL_PATH ${NU_MCUBOOT_PATH}/boot/bootutil CACHE PATH "")

target_include_directories(mcuboot_lib
    PUBLIC
        ${MCUBOOT_BOOTUTIL_PATH}/include
    PRIVATE
        ${NU_MBEDTLS_PATH}/include
        ${MCUBOOT_BOOTUTIL_PATH}/src
)

target_sources(mcuboot_lib
    PRIVATE
        ${MCUBOOT_BOOTUTIL_PATH}/src/boot_record.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/bootutil_misc.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/bootutil_public.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/caps.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/encrypted.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/fault_injection_hardening.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/fault_injection_hardening_delay_rng_mbedtls.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/image_ecdsa.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/image_ed25519.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/image_rsa.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/image_validate.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/loader.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/swap_misc.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/swap_move.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/swap_scratch.c
        ${MCUBOOT_BOOTUTIL_PATH}/src/tlv.c
)
