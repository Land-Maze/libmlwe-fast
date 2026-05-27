#include <lml/kem.h>
#include <lml/poly.h>
#include <lml/params.h>
#include <lml/fips202.h>
#include <lml/ntt.h>
#include <lml/reduce.h>
#include <string.h>
#include <lml/randombytes.h>

static void mlwe_fast_pack_pk(uint8_t pk[MLWE_FAST_PUBLICKEYBYTES],
                    mlwe_fast_polyvec *pkpv,
                    const uint8_t seed[32])
{
    mlwe_fast_polyvec_tobytes(pk, pkpv);
    memcpy(pk + MLWE_FAST_POLYVECBYTES, seed, 32);
}

static void mlwe_fast_pack_sk(uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES],
                    mlwe_fast_polyvec *skpv)
{
    mlwe_fast_polyvec_tobytes(sk, skpv);
}

void mlwe_fast_indcpa_keypair(uint8_t pk[MLWE_FAST_PUBLICKEYBYTES],
                              uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES])
{
    uint8_t buf[64];
    uint8_t publicseed[32];
    uint8_t noiseseed[32];
    mlwe_fast_polyvec a[MLWE_FAST_K];
    mlwe_fast_polyvec skpv, e, pkpv;
    unsigned int i, j;

    if (randombytes(buf, 32) != 0) {
        return;
    }

    sha3_512(buf, buf, 32);

    memcpy(publicseed, buf, 32);
    memcpy(noiseseed, buf + 32, 32);

    for (i = 0; i < MLWE_FAST_K; i++) {
        for (j = 0; j < MLWE_FAST_K; j++) {
            mlwe_fast_gen_matrix_poly(&a[i].vec[j], publicseed, j, i);
        }
    }

    mlwe_fast_polyvec_getnoise_eta1(&skpv, noiseseed, 0);
    mlwe_fast_polyvec_getnoise_eta1(&e, noiseseed, MLWE_FAST_K);

    mlwe_fast_polyvec_ntt(&skpv);
    mlwe_fast_polyvec_ntt(&e);

    for (i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_polyvec_basemul_acc_montgomery(&pkpv.vec[i], &a[i], &skpv);
        mlwe_fast_poly_tomont(&pkpv.vec[i]);

        for (j = 0; j < MLWE_FAST_N; j++) {
            pkpv.vec[i].coeffs[j] = mlwe_fast_barrett_reduce(
                pkpv.vec[i].coeffs[j] + e.vec[i].coeffs[j]
            );
        }
    }

    mlwe_fast_pack_pk(pk, &pkpv, publicseed);
    mlwe_fast_pack_sk(sk, &skpv);
}
