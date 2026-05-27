#pragma once

#include <stdint.h>
#include <lml/poly.h>
#include <lml/params.h>


/**
 * @brief Computes Centered Binomial Distribution with eta=2.
 * @param[out] r Output polynomial.
 * @param[in] buf Input random byte buffer.
 */
void mlwe_fast_cbd2(mlwe_fast_poly *r, const uint8_t buf[2 * MLWE_FAST_N / 4]);

/**
 * @brief Computes Centered Binomial Distribution with eta=3.
 * @param[out] r Output polynomial.
 * @param[in] buf Input random byte buffer.
 */
void mlwe_fast_cbd3(mlwe_fast_poly *r, const uint8_t buf[3 * MLWE_FAST_N / 4]);
