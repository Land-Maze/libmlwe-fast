#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <vector>

extern "C" {
#include <lml/cbd.h>
#include <lml/poly.h>
}

TEST_CASE("CBD2 Sampling", "[cbd]") {
    mlwe_fast_poly p;
    uint8_t buf[2 * MLWE_FAST_N / 4];

    for (int i = 0; i < (2 * MLWE_FAST_N / 4); i++) buf[i] = 0xAA;

    mlwe_fast_cbd2(&p, buf);

    SECTION("Range check") {
        for (int i = 0; i < MLWE_FAST_N; i++) {
            REQUIRE(p.coeffs[i] >= -2);
            REQUIRE(p.coeffs[i] <= 2);
        }
    }

    SECTION("Deterministic output") {
        mlwe_fast_poly p2;
        mlwe_fast_cbd2(&p2, buf);
        for (int i = 0; i < MLWE_FAST_N; i++) {
            REQUIRE(p.coeffs[i] == p2.coeffs[i]);
        }
    }
}

TEST_CASE("CBD3 Sampling", "[cbd]") {
    mlwe_fast_poly p;
    uint8_t buf[3 * MLWE_FAST_N / 4];

    for (int i = 0; i < (3 * MLWE_FAST_N / 4); i++) buf[i] = 0xAA;

    mlwe_fast_cbd3(&p, buf);

    SECTION("Range check") {
        for (int i = 0; i < MLWE_FAST_N; i++) {
            REQUIRE(p.coeffs[i] >= -3);
            REQUIRE(p.coeffs[i] <= 3);
        }
    }

    SECTION("Deterministic output") {
        mlwe_fast_poly p2;
        mlwe_fast_cbd3(&p2, buf);
        for (int i = 0; i < MLWE_FAST_N; i++) {
            REQUIRE(p.coeffs[i] == p2.coeffs[i]);
        }
    }
}
