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

struct render_target {
    int w{};
    int h{};
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
    SECTION("emplace same multiple") {
        REQUIRE(ecs.emplace<position>(e) == ecs::error::ok);
        REQUIRE(ecs.emplace<position>(e) == ecs::error::failed);
    }
    SECTION("emplace multiple same multiple") {
        REQUIRE(ecs.emplace<position, velocity>(e) == ecs::error::ok);
        REQUIRE(ecs.emplace<position, velocity>(e) == ecs::error::failed);
        REQUIRE(ecs.emplace<position>(e) == ecs::error::failed);
        REQUIRE(ecs.emplace<velocity>(e) == ecs::error::failed);
    }
}

TEST_CASE("insert", "[ecs]") {
    ecs::ecs ecs;
    ecs::entity e{};

    SECTION("insert simple") {
        position pos;

        REQUIRE(ecs.insert(e, pos) == ecs::error::ok);
    }
    SECTION("insert inplace") { REQUIRE(ecs.insert(e, position{0, 0}) == ecs::error::ok); }
    SECTION("insert same multiple") {
        REQUIRE(ecs.insert(e, position{0, 0}) == ecs::error::ok);
        REQUIRE(ecs.insert(e, position{0, 0}) == ecs::error::exists);
    }
}

TEST_CASE("contains", "[ecs]") {
    ecs::ecs ecs;

    SECTION("all_of simple") {
        auto const e = ecs.create();
        REQUIRE(ecs.emplace<position>(e) == ecs::error::ok);
        REQUIRE(ecs.all_of<position, velocity>(e) == false);
        REQUIRE(ecs.all_of<position>(e) == true);
    }

    SECTION("all_of") {
        auto const e = ecs.create();
        REQUIRE(ecs.emplace<position, velocity>(e) == ecs::error::ok);
        REQUIRE(ecs.all_of<position, velocity>(e) == true);
        REQUIRE(ecs.all_of<position>(e) == true);
        REQUIRE(ecs.destroy(e) == ecs::error::ok);
        REQUIRE(ecs.all_of<position, velocity>(e) == false);
        REQUIRE(ecs.all_of<position>(e) == false);
    }

    SECTION("any_of simple") {
        auto const e = ecs.create();
        REQUIRE(ecs.emplace<position>(e) == ecs::error::ok);
        REQUIRE(ecs.any_of<position, velocity>(e) == true);
        REQUIRE(ecs.any_of<velocity>(e) == false);
    }

    SECTION("any_of") {
        auto const e = ecs.create();
        REQUIRE(ecs.emplace<position>(e) == ecs::error::ok);
        REQUIRE(ecs.any_of<position, velocity>(e) == true);
        REQUIRE(ecs.any_of<velocity>(e) == false);
        REQUIRE(ecs.destroy(e) == ecs::error::ok);
        REQUIRE(ecs.any_of<position, velocity>(e) == false);
    }
}

TEST_CASE("erase", "[ecs]") {
    ecs::ecs ecs;

    SECTION("erase simple") {
        auto const e = ecs.create();
        REQUIRE(ecs.emplace<position, velocity>(e) == ecs::error::ok);
        REQUIRE(ecs.all_of<position, velocity>(e) == true);
        REQUIRE(ecs.erase<position>(e) == ecs::error::ok);
        REQUIRE(ecs.all_of<position, velocity>(e) == false);
        REQUIRE(ecs.all_of<velocity>(e) == true);
    }
}

TEST_CASE("get", "[ecs]") {
    ecs::ecs ecs;

    SECTION("get simple") {
        auto const e = ecs.create();
        REQUIRE(ecs.insert(e, position{0, 1}) == ecs::error::ok);
        try {
            auto const pos = ecs.get<position>(e);
            REQUIRE(pos.dx == 0);
            REQUIRE(pos.dy == 1);

        } catch (std::out_of_range const &) {
            FAIL("component not found");
        }
    }

    SECTION("update value") {
        auto const e = ecs.create();
        REQUIRE(ecs.insert(e, position{0, 1}) == ecs::error::ok);
        try {
            auto &pos = ecs.get<position>(e);
            pos.dx += 1;
            pos.dy += 1;
            auto const pos2 = ecs.get<position>(e);
            REQUIRE(pos2.dx == 1);
            REQUIRE(pos2.dy == 2);
        } catch (std::out_of_range const &) {
            FAIL("component not found");
        }
    }

    SECTION("structured bindings") {
        auto const e = ecs.create();
        REQUIRE(ecs.insert(e, position{0, 1}) == ecs::error::ok);
        REQUIRE(ecs.insert(e, velocity{0, 1}) == ecs::error::ok);
        try {
            auto [pos, vel] = ecs.get_multiple<position, velocity>(e);
            REQUIRE(pos.dx == 0);
            REQUIRE(pos.dy == 1);
            REQUIRE(vel.dx == 0);
            REQUIRE(vel.dy == 1);
        } catch (std::out_of_range const &) {
            FAIL("component not found");
        }
    }

    SECTION("structured bindings update") {
        auto const e = ecs.create();
        REQUIRE(ecs.insert(e, position{0, 1}) == ecs::error::ok);
        REQUIRE(ecs.insert(e, velocity{0, 1}) == ecs::error::ok);
        try {
            auto [pos, vel] = ecs.get_multiple<position, velocity>(e);
            pos.dx += 1;
            pos.dy += 1;
            vel.dx += 2;
            vel.dy += 2;

            auto [pos2, vel2] = ecs.get_multiple<position, velocity>(e);
            REQUIRE(pos2.dx == 1);
            REQUIRE(pos2.dy == 2);
            REQUIRE(vel2.dx == 2);
            REQUIRE(vel2.dy == 3);
        } catch (std::out_of_range const &) {
            FAIL("component not found");
        }
    }

    SECTION("structured bindings const") {
        auto const e = ecs.create();
        REQUIRE(ecs.insert(e, position{0, 1}) == ecs::error::ok);
        REQUIRE(ecs.insert(e, velocity{0, 1}) == ecs::error::ok);
        try {
            auto const [pos, vel] = ecs.get_multiple<position, velocity>(e);
            REQUIRE(pos.dx == 0);
            REQUIRE(pos.dy == 1);
            REQUIRE(vel.dx == 0);
            REQUIRE(vel.dy == 1);
        } catch (std::out_of_range const &) {
            FAIL("component not found");
        }
    }

    SECTION("invalid get") { REQUIRE_THROWS_AS(ecs.get<position>(ecs::entity{}), std::out_of_range); }

    SECTION("invalid get_multiple") {
        // catch2 has problems with multiple argument assertions

        try {
            ecs.get_multiple<position, velocity>(ecs::entity{});
        } catch (std::out_of_range const &) {
            SUCCEED("std::out_of_range catched");
        }
    }
}

TEST_CASE("view", "[ecs]") {
    ecs::ecs ecs;
    SECTION("simple component") {
        auto const e1 = ecs.create();
        auto const e2 = ecs.create();
        auto const e3 = ecs.create();
        REQUIRE(ecs.emplace<position>(e1) == ecs::error::ok);
        REQUIRE(ecs.insert(e2, velocity{2, 2}) == ecs::error::ok);
        REQUIRE(ecs.insert(e3, velocity{3, 3}) == ecs::error::ok);

        auto view = ecs.view<velocity>();
        auto const vel2 = view.get<velocity>(e2);
        auto const vel3 = view.get<velocity>(e3);
        REQUIRE(vel2.dx == 2);
        REQUIRE(vel2.dy == 2);
        REQUIRE(vel3.dx == 3);
        REQUIRE(vel3.dy == 3);
        REQUIRE_THROWS_AS(view.get<velocity>(e1), std::out_of_range);
    }

    SECTION("component") {
        auto const e1 = ecs.create();
        auto const e2 = ecs.create();
        auto const e3 = ecs.create();
        REQUIRE(ecs.insert(e1, position{1, 1}) == ecs::error::ok);
        REQUIRE(ecs.insert(e2, position{2, 2}) == ecs::error::ok);
        REQUIRE(ecs.insert(e3, position{3, 3}) == ecs::error::ok);
        REQUIRE(ecs.insert(e2, velocity{2, 2}) == ecs::error::ok);
        REQUIRE(ecs.insert(e3, velocity{3, 3}) == ecs::error::ok);
        REQUIRE(ecs.insert(e3, render_target{3, 3}) == ecs::error::ok);

        {
            auto view = ecs.view<position>();
            int want = 1;
            for (auto const entity: view) {
                auto [dx, dy] = view.get<position>(entity);
                REQUIRE(dx == want);
                REQUIRE(dy == want);
                want += 1;
            }
        }
        {
            auto view = ecs.view<position, velocity>();
            int want = 2;
            for (auto const entity: view) {
                auto [pos, vel] = view.get_multiple<position, velocity>(entity);
                REQUIRE(pos.dx == want);
                REQUIRE(pos.dy == want);
                REQUIRE(vel.dx == want);
                REQUIRE(vel.dy == want);
                want += 1;
            }
        }
        {
            auto view = ecs.view<position, velocity, render_target>();
            int want = 3;
            for (auto const entity: view) {
                auto [pos, vel, render] = view.get_multiple<position, velocity, render_target>(entity);
                REQUIRE(pos.dx == want);
                REQUIRE(pos.dy == want);
                REQUIRE(vel.dx == want);
                REQUIRE(vel.dy == want);
                REQUIRE(render.h == want);
                REQUIRE(render.w == want);
                want += 1;
            }
        }
    }

    SECTION("view get invalid") {
        auto const e1 = ecs.create();
        auto const e2 = ecs.create();
        auto const e3 = ecs.create();
        REQUIRE(ecs.emplace<position>(e1) == ecs::error::ok);
        REQUIRE(ecs.insert(e2, velocity{2, 2}) == ecs::error::ok);
        REQUIRE(ecs.insert(e3, velocity{3, 3}) == ecs::error::ok);

        auto view = ecs.view<velocity>();
        REQUIRE_NOTHROW(view.get<velocity>(e2));
        REQUIRE_NOTHROW(view.get<velocity>(e3));
        REQUIRE_THROWS_AS(view.get<velocity>(e1), std::out_of_range);
    }
}
