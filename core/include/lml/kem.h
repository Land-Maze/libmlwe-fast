#pragma once

#include <stdint.h>
#include <lml/params.h>

/**
 * @brief Generates an IND-CPA public and secret key pair.
 * @param[out] pk Output public key (size MLWE_FAST_PUBLICKEYBYTES).
 * @param[out] sk Output secret key (size MLWE_FAST_INDCPA_SECRETKEYBYTES).
 */
void mlwe_fast_indcpa_keypair(uint8_t pk[MLWE_FAST_PUBLICKEYBYTES],
                              uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES]);

/**
 * @brief IND-CPA Encryption.
 * @param[out] c Output ciphertext (MLWE_FAST_CIPHERTEXTBYTES).
 * @param[in]  m 32-byte message to encrypt.
 * @param[in]  pk Public key.
 * @param[in]  coins 32-byte random coins for deterministic encryption.
 */
void mlwe_fast_indcpa_enc(uint8_t c[MLWE_FAST_CIPHERTEXTBYTES],
                          const uint8_t m[32],
                          const uint8_t pk[MLWE_FAST_PUBLICKEYBYTES],
                          const uint8_t coins[32]);

/**
 * @brief IND-CPA Decryption.
 * @param[out] m Output 32-byte decrypted message.
 * @param[in]  c Input ciphertext (MLWE_FAST_CIPHERTEXTBYTES).
 * @param[in]  sk Secret key (MLWE_FAST_INDCPA_SECRETKEYBYTES).
 */
void mlwe_fast_indcpa_dec(uint8_t m[32],
                          const uint8_t c[MLWE_FAST_CIPHERTEXTBYTES],
                          const uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES]);
