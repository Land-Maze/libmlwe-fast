#include <lml/poly.h>
#include <lml/params.h>
#include <lml/fips202.h>
#include <lml/cbd.h>

#define REJECTION_SAMPLE_SQUEEZE_BLOCKS 2

static unsigned int rejection_sample(int16_t *r, unsigned int len, const uint8_t *buf, unsigned int buflen) {
    unsigned int ctr = 0, pos = 0;
    uint16_t val0, val1;

    while (ctr < len && pos + 3 <= buflen) {
        val0 = ((uint16_t)buf[pos + 0] | ((uint16_t)buf[pos + 1] << 8)) & 0xFFF;
        val1 = (((uint16_t)buf[pos + 1] >> 4) | ((uint16_t)buf[pos + 2] << 4)) & 0xFFF;
        pos += 3;

        if (val0 < MLWE_FAST_Q) {
            r[ctr++] = (int16_t)val0;
        }
        if (ctr < len && val1 < MLWE_FAST_Q) {
            r[ctr++] = (int16_t)val1;
        }
    }
    return ctr;
}

void mlwe_fast_gen_matrix_poly(mlwe_fast_poly *a, const uint8_t seed[32], uint8_t i, uint8_t j) {
    shake128ctx state;
    uint8_t buf[REJECTION_SAMPLE_SQUEEZE_BLOCKS * SHAKE128_RATE];
    uint8_t extseed[34];
    unsigned int ctr;

    for (int k = 0; k < 32; k++) extseed[k] = seed[k];
    extseed[32] = i;
    extseed[33] = j;

    shake128_absorb(&state, extseed, 34);
    shake128_squeezeblocks(buf, REJECTION_SAMPLE_SQUEEZE_BLOCKS, &state);

    ctr = rejection_sample(a->coeffs, MLWE_FAST_N, buf, REJECTION_SAMPLE_SQUEEZE_BLOCKS * SHAKE128_RATE);

    while (ctr < MLWE_FAST_N) {
        shake128_squeezeblocks(buf, 1, &state);
        ctr += rejection_sample(a->coeffs + ctr, MLWE_FAST_N - ctr, buf, SHAKE128_RATE);
    }

    shake128_ctx_release(&state);
}

void mlwe_fast_poly_getnoise_eta1(mlwe_fast_poly *r, const uint8_t seed[32], uint8_t nonce) {
    uint8_t buf[3 * MLWE_FAST_N / 4];
    uint8_t extseed[33];

    for (int i = 0; i < 32; i++) {
        extseed[i] = seed[i];
    }
    extseed[32] = nonce;

    shake256(buf, sizeof(buf), extseed, 33);
    mlwe_fast_cbd3(r, buf);
}

void mlwe_fast_poly_getnoise_eta2(mlwe_fast_poly *r, const uint8_t seed[32], uint8_t nonce) {
    uint8_t buf[2 * MLWE_FAST_N / 4];
    uint8_t extseed[33];

    for (int i = 0; i < 32; i++) {
        extseed[i] = seed[i];
    }
    extseed[32] = nonce;

    shake256(buf, sizeof(buf), extseed, 33);
    mlwe_fast_cbd2(r, buf);
}

void mlwe_fast_polyvec_getnoise_eta1(mlwe_fast_polyvec *r, const uint8_t seed[32], uint8_t nonce) {
    for (int i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_poly_getnoise_eta1(&r->vec[i], seed, nonce++);
    }
}

void mlwe_fast_polyvec_getnoise_eta2(mlwe_fast_polyvec *r, const uint8_t seed[32], uint8_t nonce) {
    for (int i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_poly_getnoise_eta2(&r->vec[i], seed, nonce++);
    }
}
