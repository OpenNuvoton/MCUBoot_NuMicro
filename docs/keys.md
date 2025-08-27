# ROTPK Configuration

As mentioned in the README file, you can configure the signature algorithm by modifying the CMake variables:

- For RSA: Set `MCUBOOT_SIG_TYPE=RSA` and `MCUBOOT_SIG_LEN=2048`
- For ECDSA: Set `MCUBOOT_SIG_TYPE=EC` and `MCUBOOT_SIG_LEN=256`

The public key used to verify signatures is managed in [keys.c](../bl2/ext/mcuboot/keys.c).
To modify the key value and the key length, locate the key variables based on your signature algorithm:

- For RSA: `rsa_pub_key` and `rsa_pub_key_len`
- For ECDSA: `ecdsa_pub_key` and `ecdsa_pub_key_len`
