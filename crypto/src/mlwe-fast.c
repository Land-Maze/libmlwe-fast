#include <lml/mlwe-fast.h>
#include <lml/kem.h>
#include <lml/fips202.h>
#include <lml/randombytes.h>
#include <lml/params.h>
#include <string.h>

static int verify(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t acc = 0;

    for (size_t i = 0; i < len; i++) {
        acc |= a[i] ^ b[i];
    }

    return (-(uint64_t)acc) >> 63;
}

static void cmov(uint8_t *destination, const uint8_t *source, size_t len, uint8_t condition)
{
#if defined(__GNUC__) || defined(__clang__)
    __asm__("" : "+r"(condition) : );
#endif

    uint8_t mask = -condition;

    for (size_t i = 0; i < len; i++) {
        destination[i] ^= mask & (destination[i] ^ source[i]);
    }
}

static void rkprf(uint8_t out[32], const uint8_t key[32], const uint8_t input[MLWE_FAST_CIPHERTEXTBYTES])
{
    shake256incctx state;

    shake256_inc_init(&state);
    shake256_inc_absorb(&state, key, 32);
    shake256_inc_absorb(&state, input, MLWE_FAST_CIPHERTEXTBYTES);
    shake256_inc_finalize(&state);
    shake256_inc_squeeze(out, 32, &state);
    shake256_inc_ctx_release(&state);
}

int mlwe_fast_kem_keypair(uint8_t* pk, uint8_t* sk) {
    if (pk == NULL || sk == NULL) {
        return MLWE_FAST_ERROR_GENERIC;
    }

    mlwe_fast_indcpa_keypair(pk, sk);

    memcpy(sk + MLWE_FAST_INDCPA_SECRETKEYBYTES, pk, MLWE_FAST_PUBLICKEYBYTES);

    sha3_256(sk + MLWE_FAST_INDCPA_SECRETKEYBYTES + MLWE_FAST_PUBLICKEYBYTES, pk, MLWE_FAST_PUBLICKEYBYTES);

    if (randombytes(sk + MLWE_FAST_SECRETKEYBYTES - 32, 32) != 0) {
        return MLWE_FAST_ERROR_GENERIC;
    }

    return MLWE_FAST_ERROR_NONE;
}

int mlwe_fast_kem_encaps(uint8_t* ct, uint8_t* ss, const uint8_t* pk) {
    if (ct == NULL || ss == NULL || pk == NULL) {
        return MLWE_FAST_ERROR_GENERIC;
    }

    uint8_t buf[64], kr[64], h_pk[32];

    if (randombytes(buf, 32) != 0) {
        return MLWE_FAST_ERROR_GENERIC;
    }

    sha3_256(h_pk, pk, MLWE_FAST_PUBLICKEYBYTES);

    memcpy(buf + 32, h_pk, 32);
    sha3_512(kr, buf, 64);

    mlwe_fast_indcpa_enc(ct, buf, pk, kr + 32);

    memcpy(ss, kr, 32);
    return MLWE_FAST_ERROR_NONE;
}

int mlwe_fast_kem_decaps(uint8_t* ss, const uint8_t* ct, const uint8_t* sk) {
    if (ss == NULL || ct == NULL || sk == NULL) {
        return MLWE_FAST_ERROR_GENERIC;
    }

    uint8_t buf[64], kr[64], ct_cmp[MLWE_FAST_CIPHERTEXTBYTES];
    const uint8_t *pk = sk + MLWE_FAST_INDCPA_SECRETKEYBYTES;
    const uint8_t *h_pk = pk + MLWE_FAST_PUBLICKEYBYTES;
    const uint8_t *z = sk + MLWE_FAST_SECRETKEYBYTES - 32;
    int fail = 0;

    mlwe_fast_indcpa_dec(buf, ct, sk);

    memcpy(buf + 32, h_pk, 32);
    sha3_512(kr, buf, 64);

    mlwe_fast_indcpa_enc(ct_cmp, buf, pk, kr + 32);

    fail = verify(ct, ct_cmp, MLWE_FAST_CIPHERTEXTBYTES);

    rkprf(ss, z, ct);

    cmov(ss, kr, 32, !fail);

    return MLWE_FAST_ERROR_NONE;
}
