#include <catch2/catch_test_macros.hpp>
#include <cstdint>

extern "C" {
#include <lml/poly.h>
}

TEST_CASE("Polynomial Arithmetic and Serialization", "[poly]") {

    mlwe_fast_poly p1, p2, res;
    uint8_t buffer[MLWE_FAST_POLYBYTES];

    for (int i = 0; i < MLWE_FAST_N; i++) {
        p1.coeffs[i] = i * 10;
        p2.coeffs[i] = 5;
    }

    SECTION("Addition and Subtraction") {
        mlwe_fast_poly_add(&res, &p1, &p2);
        REQUIRE(res.coeffs[0] == 5);
        REQUIRE(res.coeffs[1] == 15);

        mlwe_fast_poly_sub(&res, &p1, &p2);
        REQUIRE(res.coeffs[0] == -5);
        REQUIRE(res.coeffs[1] == 5);
    }

    SECTION("Byte Serialization Roundtrip") {
        mlwe_fast_poly_tobytes(buffer, &p1);
        mlwe_fast_poly_frombytes(&p2, buffer);

        for (int i = 0; i < MLWE_FAST_N; i++) {
            REQUIRE(p1.coeffs[i] == (p2.coeffs[i] & 0xFFF));
        }
    }
}
