//
// Created by HP on 27.09.2024.
//
#include "entity.hpp"

namespace ecs {
    entity entity_store::create() {
        entity new_entity{};
        if (!m_available_entities.empty()) {
            new_entity = m_available_entities.front();
            m_available_entities.pop();
        } else {
            new_entity = m_total_entity_count++;
        }
        m_living_entities.insert(new_entity);
        return new_entity;
    }

    error entity_store::destroy(entity e) {
        if (!m_living_entities.contains(e)) {
            return error::not_found;
        }

        if (auto const ok = m_living_entities.erase(e); ok == 0) {
            return error::failed;
        }
        m_available_entities.push(e);
        return error::ok;
    }
    error entity_store::clear() {
        for (auto e: m_living_entities) {
            m_available_entities.push(e);
        }
        m_living_entities.clear();
        return error::ok;
    }

} // namespace ecs
