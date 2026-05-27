#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>

extern "C" {
#include <lml/fips202.h>
}

TEST_CASE("FIPS-202 Implementation Checks", "[fips202]") {
    const uint8_t input[3] = {'L', 'M', 'L'};

    SECTION("SHA3-256") {
        uint8_t out[32];
        sha3_256(out, input, 3);

        bool all_zero = true;
        for(int i=0; i<32; i++) if(out[i] != 0) all_zero = false;
        REQUIRE_FALSE(all_zero);
    }

    SECTION("SHAKE128 Consistency") {
        shake128ctx state;

        uint8_t out1[SHAKE128_RATE];
        uint8_t out2[SHAKE128_RATE];

        shake128_absorb(&state, input, 3);
        shake128_squeezeblocks(out1, 1, &state);
        shake128_ctx_release(&state);


        shake128(out2, SHAKE128_RATE, input, 3);

        REQUIRE(std::memcmp(out1, out2, SHAKE128_RATE) == 0);
    }
}
