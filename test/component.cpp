//
// Created by HP on 27.09.2024.
//
#include "component.hpp"
#include <catch2/catch_all.hpp>
#include <random>
#include "entity.hpp"

struct dummy {
    int a{};
    std::string b;
};

TEST_CASE("component", "[component]") {
    ecs::component<dummy, memory_layout::compressed> component_store;

    SECTION("add component") { REQUIRE(component_store.add(ecs::entity{}, dummy{}) == ecs::error::ok); }
    SECTION("add multiple") {
        REQUIRE(component_store.add(ecs::entity{}, dummy{}) == ecs::error::ok);
        REQUIRE(component_store.add(ecs::entity{1}, dummy{}) == ecs::error::ok);
        REQUIRE(component_store.add(ecs::entity{2}, dummy{}) == ecs::error::ok);
        REQUIRE(component_store.add(ecs::entity{3}, dummy{}) == ecs::error::ok);
        REQUIRE(component_store.size() == 4);
    }

    SECTION("remove simple") {
        auto const e = ecs::entity{};
        REQUIRE(component_store.add(e, dummy{1, "Hello There"}) == ecs::error::ok);
        REQUIRE(component_store.remove(e) == ecs::error::ok);
        REQUIRE(component_store.remove(e) == ecs::error::not_found);
        REQUIRE(component_store.size() == 0);
    }

    SECTION("get entity") {
        auto const e = ecs::entity{1};
        REQUIRE(component_store.add(e, dummy{1, "Hello There"}) == ecs::error::ok);
        auto const component = component_store.get(e);
        REQUIRE(component.a == 1);
        REQUIRE(component.b == "Hello There");
    }

    SECTION("update entity") {
        auto const e = ecs::entity{1};
        REQUIRE(component_store.add(e, dummy{1, "Hello There"}) == ecs::error::ok);
        {
            auto &component = component_store.get(e);
            component.a += 2;
            component.b += ", ObiWan";
        }
        auto const component = component_store.get(e);
        REQUIRE(component.a == 3);
        REQUIRE(component.b == "Hello There, ObiWan");
    }

    SECTION("fuzzy") {
        using engine = std::mt19937;
        ecs::entity_store store;
        std::vector<ecs::entity> living_entities;
        std::vector<ecs::entity> dead_entities;
        living_entities.reserve(200);

        for (int i = 0; i < 200; i++) {
            auto const e = store.create();
            REQUIRE(component_store.add(e, dummy{i, ""}) == ecs::error::ok);
            living_entities.emplace_back(e);
        }
        std::random_device os_seed;
        engine generator{os_seed()};

        // remove 100 random entities
        for (int repitition = 0; repitition < 100; repitition++) {
            std::uniform_int_distribution<std::mt19937::result_type> distribution(0, living_entities.size() - 1);

            auto const entity_index = distribution(generator);
            auto const removed_entity = living_entities[entity_index];
            living_entities[entity_index] = living_entities[living_entities.size() - 1];
            living_entities.pop_back();

            REQUIRE(store.destroy(removed_entity) == ecs::error::ok);
            REQUIRE(component_store.remove(removed_entity) == ecs::error::ok);
        }
        // check survived entities
        for (auto const entity: living_entities) {
            auto const component = component_store.get(entity);
            REQUIRE(component.a == static_cast<int>(entity));
        }

        REQUIRE(component_store.size() == 100);
    }
}
