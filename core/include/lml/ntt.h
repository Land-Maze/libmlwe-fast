#pragma once

#include <stdint.h>
#include <lml/params.h>

/**
 * @brief Precomputed twiddle factors for the NTT.
 */
extern const int16_t mlwe_fast_zetas[128];

/**
 * @brief Computes the forward Negacyclic NTT in-place.
 * * Input is in standard domain, output is in NTT domain.
 * @param[in,out] p Pointer to the polynomial coefficients to transform.
 */
void mlwe_fast_ntt(int16_t p[MLWE_FAST_N]);

/**
 * @brief Computes the inverse Negacyclic NTT in-place.
 * * Input is in NTT domain, output is in Montgomery domain.
 * @param[in,out] p Pointer to the polynomial coefficients to transform.
 */
void mlwe_fast_invntt(int16_t p[MLWE_FAST_N]);
