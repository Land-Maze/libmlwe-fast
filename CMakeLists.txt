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
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(BUILD_BENCHMARK "Build benchmarking target" OFF)
option(BUILD_TESTS "Build testing targets" OFF)


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
