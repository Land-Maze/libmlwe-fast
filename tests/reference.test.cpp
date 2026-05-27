#include <catch2/catch_test_macros.hpp>
#include <cstring>

extern "C" {
    #include <lml/mlwe-fast.h>
    #include <api.h>
    #include <params.h>
}

TEST_CASE("Cross-Compatibility with Official Reference", "[reference]") {
    uint8_t pk_ref[KYBER_PUBLICKEYBYTES];
    uint8_t sk_ref[KYBER_SECRETKEYBYTES];
    uint8_t ct_my[MLWE_FAST_CIPHERTEXTBYTES];
    uint8_t ss_ref[KYBER_SSBYTES];
    uint8_t ss_my[MLWE_FAST_SSBYTES];

    SECTION("Reference Keygen -> My Encaps -> Reference Decaps") {
        pqcrystals_kyber512_ref_keypair(pk_ref, sk_ref);

        mlwe_fast_kem_encaps(ct_my, ss_my, pk_ref);

        pqcrystals_kyber512_ref_dec(ss_ref, ct_my, sk_ref);

        REQUIRE(std::memcmp(ss_ref, ss_my, 32) == 0);
    }
}
