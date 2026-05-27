#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Generates random bytes.
 * @param[out] out Output buffer.
 * @param[in]  outlen Number of bytes to generate.
 * @return 0 on success.
 */
int randombytes(uint8_t *out, size_t outlen);
