#include <catch2/catch_test_macros.hpp>
#include <cstdint>

extern "C" {
#include <lml/poly.h>
#include <lml/params.h>
}

TEST_CASE("Matrix and Noise Generation", "[polygen]") {
    uint8_t seed[32] = {0xFA};
    mlwe_fast_poly p;

    SECTION("Matrix Poly Rejection Sampling") {
        mlwe_fast_gen_matrix_poly(&p, seed, 0, 1);

        for (int i = 0; i < MLWE_FAST_N; i++) {
            REQUIRE(p.coeffs[i] >= 0);
            REQUIRE(p.coeffs[i] < MLWE_FAST_Q);
        }
    }

    SECTION("Domain Separation (i, j indices)") {
        mlwe_fast_poly p2;
        mlwe_fast_gen_matrix_poly(&p, seed, 0, 0);
        mlwe_fast_gen_matrix_poly(&p2, seed, 0, 1);

        int matches = 0;
        for (int i = 0; i < MLWE_FAST_N; i++) {
            if (p.coeffs[i] == p2.coeffs[i]) matches++;
        }
        REQUIRE(matches < MLWE_FAST_N);
    }
}
