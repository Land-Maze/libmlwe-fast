FetchContent_Declare(
        kyber_ref
        GIT_REPOSITORY https://github.com/pq-crystals/kyber.git
        GIT_TAG        main
        GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(kyber_ref)


set(KYBER_REF_DIR "${kyber_ref_SOURCE_DIR}/ref")

#kem.c indcpa.c polyvec.c poly.c ntt.c cbd.c reduce.c verify.c

set(KYBER_SOURCES
    "${KYBER_REF_DIR}/kem.c"
    "${KYBER_REF_DIR}/indcpa.c"
    "${KYBER_REF_DIR}/poly.c"
    "${KYBER_REF_DIR}/polyvec.c"
    "${KYBER_REF_DIR}/ntt.c"
    "${KYBER_REF_DIR}/cbd.c"
    "${KYBER_REF_DIR}/reduce.c"
    "${KYBER_REF_DIR}/verify.c"
    "${KYBER_REF_DIR}/symmetric-shake.c"
    "${KYBER_REF_DIR}/fips202.c"
)

add_library(kyber512_ref STATIC ${KYBER_SOURCES})

target_include_directories(kyber512_ref PUBLIC "${KYBER_REF_DIR}")

target_compile_definitions(kyber512_ref PUBLIC KYBER_K=2)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(kyber512_ref PRIVATE -O0)
    else()
        target_compile_options(kyber512_ref PRIVATE -O3 -DNDEBUG)
    endif()
endif()
