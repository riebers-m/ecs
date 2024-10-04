//
// Created by HP on 27.09.2024.
//
#include "ecs.hpp"
#include <catch2/catch_all.hpp>

struct position {
    int dx{};
    int dy{};
};

struct velocity {
    int dx{};
    int dy{};
};

struct not_default_constructable {
    int a;

    not_default_constructable() = delete;
    explicit not_default_constructable(const int *other) : a{*other} {}
};

TEST_CASE("emplace component", "[ecs]") {
    ecs::ecs ecs;
    ecs::entity e{};
    REQUIRE(ecs.emplace<position>(e) == ecs::error::ok);
}

TEST_CASE("emplace", "[ecs]") {
    ecs::ecs ecs;
    ecs::entity e{};

    SECTION("default constructable") { REQUIRE(ecs.emplace<position, velocity>(e) == ecs::error::ok); }

    SECTION("default constructable simple") { REQUIRE(ecs.emplace<position>(e) == ecs::error::ok); }
}

TEST_CASE("insert", "[ecs]") {
    ecs::ecs ecs;
    ecs::entity e{};

    SECTION("insert simple") {
        position pos;

        REQUIRE(ecs.insert(e, pos) == ecs::error::ok);
    }
    SECTION("insert inplace") { REQUIRE(ecs.insert(e, position{0, 0}) == ecs::error::ok); }
}
