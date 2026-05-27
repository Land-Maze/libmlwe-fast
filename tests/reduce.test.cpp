#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <cstdint>

extern "C" {
#include <lml/reduce.h>
#include <lml/params.h>
}

TEST_CASE("Barrett Reduction Correctness", "[reduce]") {
    SECTION("Negative values congruence") {
        int16_t a = -1000;
        int16_t result = mlwe_fast_barrett_reduce(a);

        int32_t diff = (int32_t)result - a;
        REQUIRE(diff % MLWE_FAST_Q == 0);
    }

    SECTION("Range check") {
        int16_t a = 30000;
        int16_t result = mlwe_fast_barrett_reduce(a);
        REQUIRE(result > -MLWE_FAST_Q);
        REQUIRE(result < 2 * MLWE_FAST_Q);
    }
}

TEST_CASE("Montgomery Reduction Consistency", "[reduce]") {
    SECTION("Consistency with R") {
        int32_t a = 50000;
        int16_t r = mlwe_fast_montgomery_reduce(a);
        int32_t R = 65536;


        int32_t left = (static_cast<int32_t>(r) * R) % MLWE_FAST_Q;
        int32_t right = a % MLWE_FAST_Q;

        if (left < 0) left += MLWE_FAST_Q;
        if (right < 0) right += MLWE_FAST_Q;

        REQUIRE(left == right);
    }
}
