#ifndef __MCUBOOT_MBEDTLS_CFG__
#define __MCUBOOT_MBEDTLS_CFG__

#if defined(MCUBOOT_SIGN_RSA)
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V21
/* Save RAM by adjusting to our exact needs */
#if MCUBOOT_SIGN_RSA_LEN == 3072
#define MBEDTLS_MPI_MAX_SIZE 384
#else /* RSA2048 */
#define MBEDTLS_MPI_MAX_SIZE 256
#endif
#endif /* MCUBOOT_SIGN_RSA */

#if defined(MCUBOOT_SIGN_EC384)
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
/* When the image is signed with EC-P384 the image hash
 * is calculated using SHA-384
 */
#define MBEDTLS_SHA512_C
#define MBEDTLS_SHA384_C
#else
/* All the other supported signing algorithms use SHA-256 to compute the image hash */
#define MBEDTLS_SHA256_C
#endif /* MCUBOOT_SIGN_EC384 */

#ifdef MCUBOOT_SIGN_EC256
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#endif /* MCUBOOT_SIGN_EC256 */

/* System support */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES

#define MBEDTLS_PLATFORM_EXIT_ALT
#define MBEDTLS_PLATFORM_PRINTF_ALT


/* mbed TLS modules */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_MD_C
#define MBEDTLS_OID_C
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_MODE_CTR
#if defined(MCUBOOT_SIGN_EC256) || \
    defined(MCUBOOT_SIGN_EC384)
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECDSA_C
#endif

#endif