//
// Created by HP on 27.09.2024.
//
#include "compressor.hpp"
#include "const.hpp"

namespace memory_layout {
    tl::expected<size_t, ecs::error> compressed::add(ecs::entity e) {
        if (auto const new_index = m_entity_count; new_index < ecs::ENTITY_COUNT) {
            m_entity_to_index[e] = new_index;
            m_index_to_entity[new_index] = e;
            ++m_entity_count;
            return new_index;
        }
        return tl::unexpected(ecs::error::max_entities);
    }

    tl::expected<size_t, ecs::error> compressed::get(ecs::entity e) const {
        if (contains(e)) {
            return m_entity_to_index.at(e);
        }
        return tl::unexpected(ecs::error::not_found);
    }
    tl::expected<size_t, ecs::error> compressed::remove(ecs::entity e) {
        if (!contains(e)) {
            return tl::unexpected(ecs::error::not_found);
        }

        if (auto const removed_entity = m_entity_to_index.find(e); removed_entity != m_entity_to_index.end()) {
            auto const index_removed_entity = removed_entity->second;
            auto const index_last_entity = m_entity_count - 1;

            auto const last_entity = m_index_to_entity[index_last_entity];
            m_entity_to_index[last_entity] = index_removed_entity;
            m_index_to_entity[index_removed_entity] = last_entity;

            m_entity_to_index.erase(e);
            m_index_to_entity.erase(index_last_entity);
            --m_entity_count;
            return index_removed_entity;
        }
        return tl::unexpected(ecs::error::not_found);
    }

    size_t compressed::size() const { return m_entity_count; }

    bool compressed::contains(ecs::entity e) const { return m_entity_to_index.contains(e); }

} // namespace memory_layout
