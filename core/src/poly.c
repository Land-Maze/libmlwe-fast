#include <lml/poly.h>
#include <lml/reduce.h>
#include <lml/ntt.h>

#if defined(LML_AVX512)
#include <immintrin.h>
void mlwe_fast_poly_add(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N; i += 32) {
        __m512i va = _mm512_loadu_si512((const __m512i *)&a->coeffs[i]);
        __m512i vb = _mm512_loadu_si512((const __m512i *)&b->coeffs[i]);
        __m512i vr = _mm512_add_epi16(va, vb);
        _mm512_storeu_si512((__m512i *)&r->coeffs[i], vr);
    }
}

void mlwe_fast_poly_sub(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N; i += 32) {
        __m512i va = _mm512_loadu_si512((const __m512i *)&a->coeffs[i]);
        __m512i vb = _mm512_loadu_si512((const __m512i *)&b->coeffs[i]);
        __m512i vr = _mm512_sub_epi16(va, vb);
        _mm512_storeu_si512((__m512i *)&r->coeffs[i], vr);
    }
}
#elif defined(LML_AVX2)
#include <immintrin.h>
void mlwe_fast_poly_add(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N; i += 16) {
        __m256i va = _mm256_loadu_si256((const __m256i *)&a->coeffs[i]);
        __m256i vb = _mm256_loadu_si256((const __m256i *)&b->coeffs[i]);
        __m256i vr = _mm256_add_epi16(va, vb);
        _mm256_storeu_si256((__m256i *)&r->coeffs[i], vr);
    }
}

void mlwe_fast_poly_sub(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N; i += 16) {
        __m256i va = _mm256_loadu_si256((const __m256i *)&a->coeffs[i]);
        __m256i vb = _mm256_loadu_si256((const __m256i *)&b->coeffs[i]);
        __m256i vr = _mm256_sub_epi16(va, vb);
        _mm256_storeu_si256((__m256i *)&r->coeffs[i], vr);
    }
}
#else
void mlwe_fast_poly_add(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N; i++) {
        r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
    }
}

void mlwe_fast_poly_sub(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N; i++) {
        r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
    }
}
#endif

void mlwe_fast_polyvec_ntt(mlwe_fast_polyvec *r) {
    for (int i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_ntt(r->vec[i].coeffs);
    }
}

void mlwe_fast_polyvec_invntt(mlwe_fast_polyvec *r) {
    for (int i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_invntt(r->vec[i].coeffs);
    }
}

void mlwe_fast_polyvec_basemul_acc_montgomery(mlwe_fast_poly *r, const mlwe_fast_polyvec *a, const mlwe_fast_polyvec *b) {
    mlwe_fast_poly t;
    
    mlwe_fast_poly_basemul_montgomery(r, &a->vec[0], &b->vec[0]);

    for (int i = 1; i < MLWE_FAST_K; i++) {
        mlwe_fast_poly_basemul_montgomery(&t, &a->vec[i], &b->vec[i]);
        mlwe_fast_poly_add(r, r, &t);
    }

    
    for (int i = 0; i < MLWE_FAST_N; i++) {
        r->coeffs[i] = mlwe_fast_barrett_reduce(r->coeffs[i]);
    }
}

void mlwe_fast_poly_tobytes(uint8_t r[MLWE_FAST_POLYBYTES], const mlwe_fast_poly *a) {
    uint16_t t0, t1;
    for (int i = 0; i < MLWE_FAST_N / 2; i++) {
        
        t0 = mlwe_fast_barrett_reduce(a->coeffs[2 * i]);
        t0 += ((int16_t)t0 >> 15) & MLWE_FAST_Q;

        t1 = mlwe_fast_barrett_reduce(a->coeffs[2 * i + 1]);
        t1 += ((int16_t)t1 >> 15) & MLWE_FAST_Q;

        r[3 * i + 0] = (uint8_t)(t0 & 0xFF);
        r[3 * i + 1] = (uint8_t)((t0 >> 8) | ((t1 & 0x0F) << 4));
        r[3 * i + 2] = (uint8_t)(t1 >> 4);
    }
}

void mlwe_fast_poly_frombytes(mlwe_fast_poly *r, const uint8_t a[MLWE_FAST_POLYBYTES]) {
    for (int i = 0; i < MLWE_FAST_N / 2; i++) {
        r->coeffs[2 * i + 0] = (int16_t)(a[3 * i + 0] | ((uint16_t)(a[3 * i + 1] & 0x0F) << 8));
        r->coeffs[2 * i + 1] = (int16_t)((a[3 * i + 1] >> 4) | ((uint16_t)a[3 * i + 2] << 4));
    }
}

static int16_t fqmul(int16_t a, int16_t b) {
    return mlwe_fast_montgomery_reduce((int32_t)a * b);
}

void mlwe_fast_polyvec_tobytes(uint8_t r[MLWE_FAST_POLYVECBYTES], const mlwe_fast_polyvec *a) {
    for (int i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_poly_tobytes(r + (i * MLWE_FAST_POLYBYTES), &a->vec[i]);
    }
}

void mlwe_fast_polyvec_frombytes(mlwe_fast_polyvec *r, const uint8_t a[MLWE_FAST_POLYVECBYTES]) {
    for (int i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_poly_frombytes(&r->vec[i], a + (i * MLWE_FAST_POLYBYTES));
    }
}

void mlwe_fast_poly_basemul_montgomery(mlwe_fast_poly *r, const mlwe_fast_poly *a, const mlwe_fast_poly *b) {
    for (int i = 0; i < MLWE_FAST_N / 4; i++) {
        int16_t zeta = mlwe_fast_zetas[64 + i];

        int16_t a0 = a->coeffs[4 * i + 0];
        int16_t a1 = a->coeffs[4 * i + 1];
        int16_t b0 = b->coeffs[4 * i + 0];
        int16_t b1 = b->coeffs[4 * i + 1];

        
        r->coeffs[4 * i + 0] = mlwe_fast_barrett_reduce(fqmul(a0, b0) + fqmul(fqmul(a1, b1), zeta));
        r->coeffs[4 * i + 1] = mlwe_fast_barrett_reduce(fqmul(a0, b1) + fqmul(a1, b0));

        a0 = a->coeffs[4 * i + 2];
        a1 = a->coeffs[4 * i + 3];
        b0 = b->coeffs[4 * i + 2];
        b1 = b->coeffs[4 * i + 3];

        
        r->coeffs[4 * i + 2] = mlwe_fast_barrett_reduce(fqmul(a0, b0) - fqmul(fqmul(a1, b1), zeta));
        r->coeffs[4 * i + 3] = mlwe_fast_barrett_reduce(fqmul(a0, b1) + fqmul(a1, b0));
    }
}

void mlwe_fast_poly_frommsg(mlwe_fast_poly *r, const uint8_t msg[32]) {
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 8; j++) {
            uint16_t mask = -((msg[i] >> j) & 1);
            r->coeffs[8 * i + j] = mask & ((MLWE_FAST_Q + 1) / 2);
        }
    }
}
void mlwe_fast_poly_tomsg(uint8_t msg[32], const mlwe_fast_poly *a) {
    uint32_t t;
    for (int i = 0; i < 32; i++) {
        msg[i] = 0;
        for (int j = 0; j < 8; j++) {
            
            t = mlwe_fast_barrett_reduce(a->coeffs[8 * i + j]);
            t += ((int16_t)t >> 15) & MLWE_FAST_Q;

            
            
            t = (((t << 1) + (MLWE_FAST_Q / 2)) / MLWE_FAST_Q) & 1;
            msg[i] |= (uint8_t)(t << j);
        }
    }
}

void mlwe_fast_poly_compress(uint8_t r[128], const mlwe_fast_poly *a) {
    uint8_t t[8];
    for (int i = 0; i < MLWE_FAST_N / 8; i++) {
        for (int j = 0; j < 8; j++) {
            uint32_t val = mlwe_fast_barrett_reduce(a->coeffs[8 * i + j]);
            val += ((int16_t)val >> 15) & MLWE_FAST_Q;
            
            t[j] = (uint8_t)((((val << 4) + (MLWE_FAST_Q / 2)) / MLWE_FAST_Q) & 15);
        }
        r[4 * i + 0] = t[0] | (t[1] << 4);
        r[4 * i + 1] = t[2] | (t[3] << 4);
        r[4 * i + 2] = t[4] | (t[5] << 4);
        r[4 * i + 3] = t[6] | (t[7] << 4);
    }
}

void mlwe_fast_poly_decompress(mlwe_fast_poly *r, const uint8_t a[128]) {
    for (int i = 0; i < MLWE_FAST_N / 2; i++) {
        r->coeffs[2 * i + 0] = (int16_t)(((uint32_t)(a[i] & 15) * MLWE_FAST_Q + 8) >> 4);
        r->coeffs[2 * i + 1] = (int16_t)(((uint32_t)(a[i] >> 4) * MLWE_FAST_Q + 8) >> 4);
    }
}

void mlwe_fast_polyvec_compress(uint8_t r[MLWE_FAST_POLYVECCOMPRESSEDBYTES], const mlwe_fast_polyvec *a) {
    unsigned int i, j, k;
    uint64_t d0;
    uint16_t t[4];

    for (i = 0; i < MLWE_FAST_K; i++) {
        for (j = 0; j < MLWE_FAST_N / 4; j++) {
            for (k = 0; k < 4; k++) {
                t[k] = a->vec[i].coeffs[4 * j + k];
                t[k] += ((int16_t)t[k] >> 15) & MLWE_FAST_Q;
                d0 = t[k];
                d0 <<= 10;
                d0 += 1665;
                d0 *= 1290167;
                d0 >>= 32;
                t[k] = d0 & 0x3ff;
            }

            r[0] = (uint8_t)(t[0] >> 0);
            r[1] = (uint8_t)((t[0] >> 8) | (t[1] << 2));
            r[2] = (uint8_t)((t[1] >> 6) | (t[2] << 4));
            r[3] = (uint8_t)((t[2] >> 4) | (t[3] << 6));
            r[4] = (uint8_t)(t[3] >> 2);
            r += 5;
        }
    }
}

void mlwe_fast_polyvec_decompress(mlwe_fast_polyvec *r, const uint8_t a[MLWE_FAST_POLYVECCOMPRESSEDBYTES]) {
    unsigned int i, j, k;
    uint16_t t[4];

    for (i = 0; i < MLWE_FAST_K; i++) {
        for (j = 0; j < MLWE_FAST_N / 4; j++) {
            t[0] = (uint16_t)((a[0] >> 0) | ((uint16_t)a[1] << 8));
            t[1] = (uint16_t)((a[1] >> 2) | ((uint16_t)a[2] << 6));
            t[2] = (uint16_t)((a[2] >> 4) | ((uint16_t)a[3] << 4));
            t[3] = (uint16_t)((a[3] >> 6) | ((uint16_t)a[4] << 2));
            a += 5;

            for (k = 0; k < 4; k++) {
                r->vec[i].coeffs[4 * j + k] = (int16_t)(((uint32_t)(t[k] & 0x3FF) * MLWE_FAST_Q + 512) >> 10);
            }
        }
    }
}

void mlwe_fast_poly_tomont(mlwe_fast_poly *r) {
    const int32_t f = 1353; 
    for (int i = 0; i < MLWE_FAST_N; i++) {
        r->coeffs[i] = mlwe_fast_montgomery_reduce((int32_t)r->coeffs[i] * f);
    }
}
