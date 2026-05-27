#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <cstring>

extern "C" {
#include <lml/mlwe-fast.h>
#include <lml/kem.h>
#include <lml/params.h>
}

TEST_CASE("KEM Shared Secret Consistency", "[kem]") {
    uint8_t pk[MLWE_FAST_PUBLICKEYBYTES];
    uint8_t sk[MLWE_FAST_SECRETKEYBYTES];
    uint8_t ct[MLWE_FAST_CIPHERTEXTBYTES];
    uint8_t ss1[MLWE_FAST_SSBYTES];
    uint8_t ss2[MLWE_FAST_SSBYTES];

    mlwe_fast_kem_keypair(pk, sk);

    SECTION("Roundtrip equality") {
        mlwe_fast_kem_encaps(ct, ss1, pk);
        mlwe_fast_kem_decaps(ss2, ct, sk);
        REQUIRE(std::memcmp(ss1, ss2, 32) == 0);
    }
}

TEST_CASE("Key Generation Consistency", "[kem]") {
    uint8_t pk[MLWE_FAST_PUBLICKEYBYTES];
    uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES];

    std::memset(pk, 0xAA, MLWE_FAST_PUBLICKEYBYTES);
    std::memset(sk, 0xBB, MLWE_FAST_INDCPA_SECRETKEYBYTES);

    mlwe_fast_indcpa_keypair(pk, sk);

    SECTION("Output is non-zero") {
        bool pk_all_zero = true;
        for(int i=0; i < MLWE_FAST_PUBLICKEYBYTES; i++) if(pk[i] != 0) pk_all_zero = false;

        bool sk_all_zero = true;
        for(int i=0; i < MLWE_FAST_INDCPA_SECRETKEYBYTES; i++) if(sk[i] != 0) sk_all_zero = false;

        REQUIRE_FALSE(pk_all_zero);
        REQUIRE_FALSE(sk_all_zero);
    }
}
