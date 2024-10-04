//
// Created by HP on 27.09.2024.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <queue>
#include <unordered_set>

#include "error.hpp"
#include "types.hpp"

namespace ecs {
    class entity_store {
    private:
        std::queue<entity> m_available_entities{};
        std::unordered_set<entity> m_living_entities{};
        std::size_t m_total_entity_count{};

    public:
        entity_store() = default;

        [[nodiscard]] entity create();
        error destroy(entity);
        error clear();
    };
} // namespace ecs
#endif // ENTITY_HPP
