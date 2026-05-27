#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <vector>

extern "C" {
#include <lml/kem.h>
#include <lml/params.h>
#include <lml/randombytes.h>
}

TEST_CASE("IND-CPA Encryption/Decryption Roundtrip", "[indcpa]") {
    uint8_t pk[MLWE_FAST_PUBLICKEYBYTES];
    uint8_t sk[MLWE_FAST_INDCPA_SECRETKEYBYTES];
    uint8_t ct[MLWE_FAST_CIPHERTEXTBYTES];
    uint8_t m_orig[32];
    uint8_t m_dec[32];
    uint8_t coins[32];


    mlwe_fast_indcpa_keypair(pk, sk);

    SECTION("Recovery of random messages") {
        for (int i = 0; i < 10; i++) {

            randombytes(m_orig, 32);
            randombytes(coins, 32);

            mlwe_fast_indcpa_enc(ct, m_orig, pk, coins);

            mlwe_fast_indcpa_dec(m_dec, ct, sk);

            REQUIRE(std::memcmp(m_orig, m_dec, 32) == 0);
        }
    }

    SECTION("Decryption failure with wrong key") {
        uint8_t pk2[MLWE_FAST_PUBLICKEYBYTES];
        uint8_t sk2[MLWE_FAST_INDCPA_SECRETKEYBYTES];

        mlwe_fast_indcpa_keypair(pk2, sk2);
        randombytes(m_orig, 32);
        randombytes(coins, 32);

        mlwe_fast_indcpa_enc(ct, m_orig, pk, coins);
        mlwe_fast_indcpa_dec(m_dec, ct, sk2);


        REQUIRE(std::memcmp(m_orig, m_dec, 32) != 0);
    }
}
