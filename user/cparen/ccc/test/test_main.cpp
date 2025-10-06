#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ccc_coro.h"

TEST_CASE("passing_test") {
    CHECK(true);
}

TEST_CASE("failing_test" * doctest::skip()) {
    CHECK(false);
}

TEST_CASE("adding test") {
    CHECK(ccc::add(2,3) == 5);
}
