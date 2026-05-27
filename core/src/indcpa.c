#include <lml/kem.h>
#include <lml/poly.h>
#include <lml/ntt.h>
#include <lml/reduce.h>
#include <string.h>

void mlwe_fast_indcpa_enc(uint8_t c[MLWE_FAST_CIPHERTEXTBYTES],
                          const uint8_t m[32],
                          const uint8_t pk[MLWE_FAST_PUBLICKEYBYTES],
                          const uint8_t coins[32])
{
    mlwe_fast_polyvec at[MLWE_FAST_K], pkpv, r, e1, u;
    mlwe_fast_poly v, e2, k;
    const uint8_t *publicseed = pk + MLWE_FAST_POLYVECBYTES;
    unsigned int i, j;

    mlwe_fast_polyvec_frombytes(&pkpv, pk);
    for (i = 0; i < MLWE_FAST_K; i++) {
        for (j = 0; j < MLWE_FAST_K; j++) {
            mlwe_fast_gen_matrix_poly(&at[i].vec[j], publicseed, i, j);
        }
    }

    mlwe_fast_polyvec_getnoise_eta1(&r, coins, 0);
    mlwe_fast_polyvec_getnoise_eta2(&e1, coins, MLWE_FAST_K);
    mlwe_fast_poly_getnoise_eta2(&e2, coins, 2 * MLWE_FAST_K);

    mlwe_fast_polyvec_ntt(&r);

    for (i = 0; i < MLWE_FAST_K; i++) {
        mlwe_fast_polyvec_basemul_acc_montgomery(&u.vec[i], &at[i], &r);
        mlwe_fast_invntt(u.vec[i].coeffs);
        mlwe_fast_poly_add(&u.vec[i], &u.vec[i], &e1.vec[i]);
    }

    mlwe_fast_polyvec_basemul_acc_montgomery(&v, &pkpv, &r);
    mlwe_fast_invntt(v.coeffs);
    mlwe_fast_poly_add(&v, &v, &e2);

    mlwe_fast_poly_frommsg(&k, m);
    mlwe_fast_poly_add(&v, &v, &k);

    mlwe_fast_polyvec_compress(c, &u);
    mlwe_fast_poly_compress(c + MLWE_FAST_POLYVECCOMPRESSEDBYTES, &v);
}

void mlwe_fast_indcpa_dec(uint8_t m[32],
                          const uint8_t c[MLWE_FAST_CIPHERTEXTBYTES],
                          const uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES])
{
    mlwe_fast_polyvec u, skpv;
    mlwe_fast_poly v, w;

    mlwe_fast_polyvec_decompress(&u, c);
    mlwe_fast_poly_decompress(&v, c + MLWE_FAST_POLYVECCOMPRESSEDBYTES);
    mlwe_fast_polyvec_frombytes(&skpv, sk);

    mlwe_fast_polyvec_ntt(&u);
    mlwe_fast_polyvec_basemul_acc_montgomery(&w, &skpv, &u);
    mlwe_fast_invntt(w.coeffs);

    mlwe_fast_poly_sub(&w, &v, &w);
    mlwe_fast_poly_tomsg(m, &w);
}
