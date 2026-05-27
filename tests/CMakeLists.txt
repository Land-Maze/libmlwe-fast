file(GLOB TEST_SOURCES "*.test.cpp")

add_executable(mlwe-tests ${TEST_SOURCES})

target_link_libraries(mlwe-tests
  PRIVATE
  mlwe-fast
  Catch2::Catch2WithMain
  kyber512_ref
  core
)

include(CTest)
include(Catch)
catch_discover_tests(mlwe-tests)
