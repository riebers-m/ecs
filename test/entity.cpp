//
// Created by HP on 26.09.2024.
//
#include "ecs.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("Test hello", "[hello]") {
    REQUIRE(hello({"John"}) == "Hello John");
}