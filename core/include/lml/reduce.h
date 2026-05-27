#pragma once

#include <stdint.h>
#include <lml/params.h>

/**
 * @brief Performs Montgomery reduction on a 32-bit integer.
 *
 * It computes (a * R^-1) mod Q.
 *
 * @param[in] a The 32-bit intermediate product to be reduced.
 * @return int16_t The reduced 16-bit coefficient in the range (-Q, Q).
 */
static inline int16_t mlwe_fast_montgomery_reduce(const int32_t a) {
    int32_t t;
    int16_t m;

    m = (int16_t)(a * MLWE_FAST_QINV);
    t = (a - (int32_t)m * MLWE_FAST_Q) >> 16;

    return (int16_t)t;
}

/**
 * @brief Performs Barrett reduction on a 16-bit integer.
 * @param[in] a The 16-bit coefficient to be reduced.
 * @return int16_t The reduced coefficient in the range (-Q, Q).
 */
static inline int16_t mlwe_fast_barrett_reduce(const int16_t a) {
    int32_t t;
    int16_t v;

    t = ((int32_t)a * MLWE_FAST_BARRETT_M) >> MLWE_FAST_BARRETT_K;
    v = a - (int16_t)t * MLWE_FAST_Q;

    return v;
}
