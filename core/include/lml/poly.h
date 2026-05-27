#pragma once

#include <lml/reduce.h>
#include <stdint.h>
#include <stdalign.h>
#include <lml/params.h>

/**
 * @struct mlwe_fast_poly
 * @brief Representation of a polynomial in the ring R_q = Z_q[X]/(X^n + 1).
 * * The coefficients are stored as 16-bit integers and are aligned to 32 bytes
 * to use AVX2/SIMD vectorization.
 */
typedef struct {
	alignas(32) int16_t coeffs[MLWE_FAST_N];
} mlwe_fast_poly;

/**
 * @struct mlwe_fast_polyvec
 * @brief A vector of polynomials of length K.
 * * Used to represent elements in the module of rank K.
 */
typedef struct {
	mlwe_fast_poly vec[MLWE_FAST_K];
} mlwe_fast_polyvec;

/**
 * @brief Serializes a polynomial into a byte array.
 * * @param[out] r Output byte array of size MLWE_FAST_POLYBYTES.
 * @param[in]  a Pointer to the polynomial to be serialized.
 */
void mlwe_fast_poly_tobytes(uint8_t r[MLWE_FAST_POLYBYTES], const mlwe_fast_poly *a);

/**
 * @brief Deserializes a byte array into a polynomial.
 * * @param[out] r Pointer to the output polynomial.
 * @param[in]  a Input byte array of size MLWE_FAST_POLYBYTES.
 */
void mlwe_fast_poly_frombytes(mlwe_fast_poly *r, const uint8_t a[MLWE_FAST_POLYBYTES]);

/**
 * @brief Adds two polynomials component-wise.
 * * @param[out] r Resulting polynomial (r = a + b).
 * @param[in]  a First operand.
 * @param[in]  b Second operand.
 */
void mlwe_fast_poly_add(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b);

/**
 * @brief Subtracts one polynomial from another component-wise.
 * * @param[out] r Resulting polynomial (r = a - b).
 * @param[in]  a First operand.
 * @param[in]  b Second operand (subtrahend).
 */
void mlwe_fast_poly_sub(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b);

/**
 * @brief Generates a single polynomial for the public matrix A.
 * * @param[out] a Pointer to the output polynomial.
 * @param[in]  seed 32-byte public seed.
 * @param[in]  i Row index of the matrix.
 * @param[in]  j Column index of the matrix.
 */
void mlwe_fast_gen_matrix_poly(mlwe_fast_poly *a, const uint8_t seed[32], uint8_t i, uint8_t j);

/**
 * @brief Generates a vector of polynomials for the secret or noise (eta1).
 * @param[out] r Pointer to the output polynomial vector.
 * @param[in]  seed 32-byte seed.
 * @param[in]  nonce Nonce to ensure domain separation.
 */
void mlwe_fast_polyvec_getnoise_eta1(mlwe_fast_polyvec *r, const uint8_t seed[32], uint8_t nonce);

/**
 * @brief Generates a vector of polynomials for the secret or noise (eta2).
 * @param[out] r Pointer to the output polynomial vector.
 * @param[in]  seed 32-byte seed.
 * @param[in]  nonce Nonce to ensure domain separation.
 */
void mlwe_fast_polyvec_getnoise_eta2(mlwe_fast_polyvec *r, const uint8_t seed[32], uint8_t nonce);

/**
 * @brief Performs pointwise multiplication of two polynomial vectors and accumulates the result.
 * @param[out] r Resulting polynomial.
 * @param[in]  a First operand vector.
 * @param[in]  b Second operand vector.
 */
void mlwe_fast_polyvec_basemul_acc_montgomery(mlwe_fast_poly *r, const mlwe_fast_polyvec *a, const mlwe_fast_polyvec *b);

/**
 * @brief Applies forward NTT to all polynomials in a vector.
 * @param[in,out] r Pointer to the polynomial vector.
 */
void mlwe_fast_polyvec_ntt(mlwe_fast_polyvec *r);

/**
 * @brief Applies inverse NTT to all polynomials in a vector.
 * @param[in,out] r Pointer to the polynomial vector.
 */
void mlwe_fast_polyvec_invntt(mlwe_fast_polyvec *r);

/**
 * @brief Serializes a polynomial vector into a byte array.
 * @param[out] r Output byte array of size MLWE_FAST_POLYVECBYTES.
 * @param[in]  a Pointer to the polynomial vector.
 */
void mlwe_fast_polyvec_tobytes(uint8_t r[MLWE_FAST_POLYVECBYTES], const mlwe_fast_polyvec *a);

/**
 * @brief Deserializes a byte array into a polynomial vector.
 * @param[out] r Pointer to the output polynomial vector.
 * @param[in]  a Input byte array of size MLWE_FAST_POLYVECBYTES.
 */
void mlwe_fast_polyvec_frombytes(mlwe_fast_polyvec *r, const uint8_t a[MLWE_FAST_POLYVECBYTES]);

/**
 * @brief Performs pointwise multiplication of two polynomials in the NTT domain.
 * @param[out] r Resulting polynomial.
 * @param[in]  a First operand.
 * @param[in]  b Second operand.
 */
void mlwe_fast_poly_basemul_montgomery(mlwe_fast_poly *r,
                                       const mlwe_fast_poly *a,
                                       const mlwe_fast_poly *b);

/**
 * @brief Encodes a 32-byte message into a polynomial.
 * Bits are mapped to {0, ceil(Q/2)}.
 * @param[out] r Output polynomial.
 * @param[in]  msg 32-byte input message.
 */
void mlwe_fast_poly_frommsg(mlwe_fast_poly *r, const uint8_t msg[32]);

/**
 * @brief Decodes a polynomial into a 32-byte message.
 * @param[out] msg 32-byte output message.
 * @param[in]  a Input polynomial.
 */
void mlwe_fast_poly_tomsg(uint8_t msg[32], const mlwe_fast_poly *a);

/**
 * @brief Compresses a polynomial (12 bits to 4 bits) for the ciphertext v.
 * @param[out] r Output compressed byte array (128 bytes).
 * @param[in]  a Input polynomial.
 */
void mlwe_fast_poly_compress(uint8_t r[128], const mlwe_fast_poly *a);

/**
 * @brief Decompresses a 4-bit polynomial back to 12 bits.
 * @param[out] r Output decompressed polynomial.
 * @param[in]  a Input compressed byte array (128 bytes).
 */
void mlwe_fast_poly_decompress(mlwe_fast_poly *r, const uint8_t a[128]);

/**
 * @brief Compresses a polynomial vector (12 bits to 10 bits per coefficient) for ciphertext u.
 * @param[out] r Output compressed byte array (MLWE_FAST_POLYVECCOMPRESSEDBYTES).
 * @param[in]  a Input polynomial vector.
 */
void mlwe_fast_polyvec_compress(uint8_t r[MLWE_FAST_POLYVECCOMPRESSEDBYTES], const mlwe_fast_polyvec *a);

/**
 * @brief Decompresses a 10-bit polynomial vector back to 12 bits.
 * @param[out] r Output decompressed polynomial vector.
 * @param[in]  a Input compressed byte array (MLWE_FAST_POLYVECCOMPRESSEDBYTES).
 */
void mlwe_fast_polyvec_decompress(mlwe_fast_polyvec *r, const uint8_t a[MLWE_FAST_POLYVECCOMPRESSEDBYTES]);

/**
 * @brief Generates a single polynomial for noise terms.
 * @param[out] r Pointer to the output polynomial.
 * @param[in]  seed 32-byte seed.
 * @param[in]  nonce Nonce to ensure domain separation.
 */
void mlwe_fast_poly_getnoise_eta1(mlwe_fast_poly *r, const uint8_t seed[32], uint8_t nonce);
void mlwe_fast_poly_getnoise_eta2(mlwe_fast_poly *r, const uint8_t seed[32], uint8_t nonce);

/**
 * @brief Converts a polynomial to the Montgomery domain (multiplies by R mod Q).
 * @param[in,out] r Pointer to the polynomial.
 */
void mlwe_fast_poly_tomont(mlwe_fast_poly *r);
