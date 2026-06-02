cmake_minimum_required(VERSION 4.0)
project(
  mlwe-fast
  DESCRIPTION "A library for fast MLWE-based implementation of the CRYSTALS-Kyber KEM"
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AVX2 "Enable AVX2 vectorization" OFF)
option(AVX512 "Enable AVX-512 vectorization" OFF)
option(BUILD_BENCHMARK "Build benchmarking target" OFF)
option(BUILD_TESTS "Build testing targets" OFF)

if(AVX512)
    add_compile_definitions(LML_AVX512)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-mavx512f -mavx512vl -mavx512bw -mavx512dq)
    elseif(MSVC)
        add_compile_options(/arch:AVX512)
    endif()
    message(STATUS "AVX-512 vectorization enabled")
elseif(AVX2)
    add_compile_definitions(LML_AVX2)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-mavx2)
    elseif(MSVC)
        add_compile_options(/arch:AVX2)
    endif()
    message(STATUS "AVX2 vectorization enabled")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native -mtune=native")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-literal-operator")
endif()

include(FetchContent)
if(BUILD_TESTS OR BUILD_BENCHMARK)
    include(cmake/Catch2.cmake)
    include(cmake/Kyber-Ref.cmake)
endif()

add_subdirectory(core)
add_subdirectory(crypto)

if(BUILD_BENCHMARK)
    add_subdirectory(bench)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
