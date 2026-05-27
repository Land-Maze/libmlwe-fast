#include <catch2/catch_test_macros.hpp>
#include <cstdint>

extern "C" {
#include <lml/poly.h>
#include <lml/ntt.h>
#include <lml/reduce.h>
#include <lml/params.h>
}

TEST_CASE("Polynomial Vector Arithmetic", "[polyvec]") {
    mlwe_fast_polyvec pv1, pv2;
    mlwe_fast_poly res;

    for (int k = 0; k < MLWE_FAST_K; k++) {
        for (int i = 0; i < MLWE_FAST_N; i++) {
            pv1.vec[k].coeffs[i] = 1;
            pv2.vec[k].coeffs[i] = 2;
        }
    }

    SECTION("Polyvec NTT Roundtrip") {
        mlwe_fast_polyvec_ntt(&pv1);
        mlwe_fast_polyvec_invntt(&pv1);

        for (int k = 0; k < MLWE_FAST_K; k++) {
            for (int i = 0; i < MLWE_FAST_N; i++) {
                int32_t val = mlwe_fast_montgomery_reduce((int32_t)pv1.vec[k].coeffs[i]);
                while (val < 0) val += MLWE_FAST_Q;
                val %= MLWE_FAST_Q;
                REQUIRE(val == 1);
            }
        }
    }

    SECTION("Pointwise Accumulation (Inner Product)") {
        mlwe_fast_polyvec_basemul_acc_montgomery(&res, &pv1, &pv2);

        bool non_zero = false;
        for(int i=0; i<MLWE_FAST_N; i++) if(res.coeffs[i] != 0) non_zero = true;
        REQUIRE(non_zero);
    }

    SECTION("Vector Serialization") {
        uint8_t buffer[MLWE_FAST_POLYVECBYTES];
        mlwe_fast_polyvec_tobytes(buffer, &pv1);
        mlwe_fast_polyvec_frombytes(&pv2, buffer);

        for (int k = 0; k < MLWE_FAST_K; k++) {
            for (int i = 0; i < MLWE_FAST_N; i++) {
                REQUIRE((pv1.vec[k].coeffs[i] & 0xFFF) == pv2.vec[k].coeffs[i]);
            }
        }
    }

    SECTION("Vector Compression/Decompression") {
        uint8_t buffer[MLWE_FAST_POLYVECCOMPRESSEDBYTES];
        for (int k = 0; k < MLWE_FAST_K; k++) {
            for (int i = 0; i < MLWE_FAST_N; i++) {
                pv1.vec[k].coeffs[i] = (i * 13) % MLWE_FAST_Q;
            }
        }
        mlwe_fast_polyvec_compress(buffer, &pv1);
        mlwe_fast_polyvec_decompress(&pv2, buffer);

        for (int k = 0; k < MLWE_FAST_K; k++) {
            for (int i = 0; i < MLWE_FAST_N; i++) {
                int16_t diff = (pv1.vec[k].coeffs[i] - pv2.vec[k].coeffs[i]) % MLWE_FAST_Q;
                if (diff < 0) diff += MLWE_FAST_Q;
                if (diff > MLWE_FAST_Q / 2) diff = MLWE_FAST_Q - diff;
                REQUIRE(diff <= 2);
            }
        }
    }
}
