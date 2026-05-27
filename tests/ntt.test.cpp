#include "catch2/catch_message.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

extern "C" {
#include <lml/ntt.h>
#include <lml/poly.h>
#include <lml/reduce.h>
#include <lml/params.h>
}

TEST_CASE("NTT Core Functionality", "[ntt]") {
    mlwe_fast_poly p;
    mlwe_fast_poly original;

    for (int i = 0; i < MLWE_FAST_N; i++) {
        p.coeffs[i] = mlwe_fast_barrett_reduce(i * 7);
        original.coeffs[i] = p.coeffs[i];
    }

    SECTION("Roundtrip Identity") {
        mlwe_fast_ntt(p.coeffs);
        mlwe_fast_invntt(p.coeffs);

        for (int i = 0; i < MLWE_FAST_N; i++) {
            int32_t res =  mlwe_fast_montgomery_reduce((int32_t)p.coeffs[i]);
            while (res < 0) res += MLWE_FAST_Q;
            res %= MLWE_FAST_Q;

            int32_t exp = original.coeffs[i];
            while (exp < 0) exp += MLWE_FAST_Q;
            exp %= MLWE_FAST_Q;

            if (res != exp) {
                UNSCOPED_INFO("Index: " << i << " Result: " << res << " Expected: " << exp);
            }
            REQUIRE(res == exp);
        }
    }

    SECTION("Linearity Property") {
        mlwe_fast_poly a, b, a_plus_b, ntt_a, ntt_b, ntt_sum;

        for (int i = 0; i < MLWE_FAST_N; i++) {
            a.coeffs[i] = mlwe_fast_barrett_reduce(i * 3);
            b.coeffs[i] = mlwe_fast_barrett_reduce(i * 5);
            a_plus_b.coeffs[i] = mlwe_fast_barrett_reduce(a.coeffs[i] + b.coeffs[i]);
        }

        mlwe_fast_ntt(a_plus_b.coeffs);

        mlwe_fast_ntt(a.coeffs);
        mlwe_fast_ntt(b.coeffs);

        for (int i = 0; i < MLWE_FAST_N; i++) {
            int16_t sum_of_ntt = mlwe_fast_barrett_reduce(a.coeffs[i] + b.coeffs[i]);
            int16_t ntt_of_sum = mlwe_fast_barrett_reduce(a_plus_b.coeffs[i]);

            if (sum_of_ntt < 0) sum_of_ntt += MLWE_FAST_Q;
            if (ntt_of_sum < 0) ntt_of_sum += MLWE_FAST_Q;

            REQUIRE(sum_of_ntt == ntt_of_sum);
        }
    }
}
