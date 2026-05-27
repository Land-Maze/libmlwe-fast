add_executable(kem-bench kem_bench.cpp)
target_link_libraries(kem-bench
    PRIVATE
    mlwe-fast
    Catch2::Catch2WithMain
    kyber512_ref
)
