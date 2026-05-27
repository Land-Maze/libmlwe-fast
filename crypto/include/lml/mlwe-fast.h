/*
	MIT License

	Copyright (c) 2026 Timur Dzhesur

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#pragma once

#include <stdint.h>
#include <stddef.h>

#define MLWE_FAST_VERSION_MAJOR 0
#define MLWE_FAST_VERSION_MINOR 1
#define MLWE_FAST_VERSION_PATCH 0

#define MLWE_FAST_ERROR_NONE 0
#define MLWE_FAST_ERROR_GENERIC (-1)

#define MLWE_FAST_PUBLICKEYBYTES  800
#define MLWE_FAST_SECRETKEYBYTES  1632
#define MLWE_FAST_CIPHERTEXTBYTES 768
#define MLWE_FAST_SSBYTES         32

#if defined(_WIN32)
#define MLWE_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define MLWE_EXPORT __attribute__((visibility("default")))
#else
#define MLWE_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generates a public and secret key pair for the MLWE-KEM.
 *
 * @param[out] pk Pointer to the output public key buffer (MLWE_FAST_PUBLICKEYBYTES).
 * @param[out] sk Pointer to the output secret key buffer (MLWE_FAST_SECRETKEYBYTES).
 * @return 0 on success, or -1 error code if fails.
 */
MLWE_EXPORT
int mlwe_fast_kem_keypair(uint8_t* pk, uint8_t* sk);

/**
 * @brief Generates a shared secret and an encapsulated ciphertext.
 *
 * @param[out] ct Pointer to the output ciphertext buffer (MLWE_FAST_CIPHERTEXTBYTES).
 * @param[out] ss Pointer to the output shared secret buffer (MLWE_FAST_SSBYTES).
 * @param[in]  pk Pointer to the input public key.
 * @return 0 on success, or -1 error code if fails.
 */
MLWE_EXPORT
int mlwe_fast_kem_encaps(uint8_t* ct, uint8_t* ss, const uint8_t* pk);

/**
 * @brief Decapsulates a ciphertext to recover the shared secret.
 *
 * @param[out] ss Pointer to the output shared secret buffer (MLWE_FAST_SSBYTES).
 * @param[in]  ct Pointer to the input ciphertext.
 * @param[in]  sk Pointer to the input secret key.
 * @return 0 on success, or -1 error code if fails.
 */
MLWE_EXPORT
int mlwe_fast_kem_decaps(uint8_t* ss, const uint8_t* ct, const uint8_t* sk);

#ifdef __cplusplus
}
#endif
