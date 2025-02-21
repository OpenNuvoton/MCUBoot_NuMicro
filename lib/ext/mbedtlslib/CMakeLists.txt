# Starting Mbed TLS 3.x release, mbedtls/library/CMakeLists.txt has dependency on
# link_to_source() function that is in mbedtls/CMakeLists.txt, so define
# link_to_source here. This shall be later expanded to add a link to the mbedtls
# source file from the build directory if required by libspdm.
function(link_to_source base_name)
endfunction(link_to_source)

add_subdirectory(${NU_MBEDTLS_PATH}/library)

# mbedtls

target_link_libraries(mbedtls
    PUBLIC
        mbedtls_config
)

target_include_directories(mbedtls PRIVATE
    ${NU_MBEDTLS_PATH}/include
    ${NU_MBEDTLS_PATH}/library
)

# mbedx509

target_link_libraries(mbedx509
    PUBLIC
        mbedtls_config
)

target_include_directories(mbedx509 PRIVATE
    ${NU_MBEDTLS_PATH}/include
    ${NU_MBEDTLS_PATH}/library
)

# mbedcrypto

target_link_libraries(mbedcrypto
    PUBLIC
        mbedtls_config
)

target_include_directories(mbedcrypto PRIVATE
    ${NU_MBEDTLS_PATH}/include
    ${NU_MBEDTLS_PATH}/library
)

add_library(mbedtls_lib INTERFACE)
target_link_libraries(mbedtls_lib INTERFACE
    mbedx509
    mbedcrypto
)
