#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <vector>

extern "C" {
#include <lml/mlwe-fast.h>
#include <api.h>
}

TEST_CASE("KEM Reference Code Performance", "[ref]") {
    uint8_t pk[MLWE_FAST_PUBLICKEYBYTES];
    uint8_t sk[MLWE_FAST_SECRETKEYBYTES];
    uint8_t ct[MLWE_FAST_CIPHERTEXTBYTES];
    uint8_t ss[MLWE_FAST_SSBYTES];

    SECTION("Keypair generation") {
        BENCHMARK("Reference Keypair") {
            return pqcrystals_kyber512_ref_keypair(pk, sk);
        };
    }

    SECTION("Encapsulation") {
        pqcrystals_kyber512_ref_keypair(pk, sk);
        BENCHMARK("Reference Encapsulation") {
            return pqcrystals_kyber512_ref_enc(ct, ss, pk);
        };
    }

    SECTION("Decapsulation") {
        pqcrystals_kyber512_ref_keypair(pk, sk);
        pqcrystals_kyber512_ref_enc(ct, ss, pk);
        BENCHMARK("Reference Decapsulation") {
            return pqcrystals_kyber512_ref_dec(ss, ct, sk);
        };
    }
}

TEST_CASE("MLWE-Fast Optimizations Performance", "[mlwe-fast]") {
    uint8_t pk[MLWE_FAST_PUBLICKEYBYTES];
    uint8_t sk[MLWE_FAST_SECRETKEYBYTES];
    uint8_t ct[MLWE_FAST_CIPHERTEXTBYTES];
    uint8_t ss[MLWE_FAST_SSBYTES];

    SECTION("Keypair generation") {
        BENCHMARK("MLWE-Fast Keypair") {
            return mlwe_fast_kem_keypair(pk, sk);
        };
    }

    SECTION("Encapsulation") {
        mlwe_fast_kem_keypair(pk, sk);
        BENCHMARK("MLWE-Fast Encapsulation") {
            return mlwe_fast_kem_encaps(ct, ss, pk);
        };
    }

    SECTION("Decapsulation") {
        mlwe_fast_kem_keypair(pk, sk);
        mlwe_fast_kem_encaps(ct, ss, pk);
        BENCHMARK("MLWE-Fast Decapsulation") {
            return mlwe_fast_kem_decaps(ss, ct, sk);
        };
    }
}
