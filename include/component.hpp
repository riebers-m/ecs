//
// Created by HP on 27.09.2024.
//

#ifndef COMPONENT_HPP
#define COMPONENT_HPP
#include <array>
#include <format>
#include <stdexcept>
#include <tl/expected.hpp>
#include "compressor.hpp"
#include "const.hpp"

namespace ecs {

    class base_component {
    public:
        virtual ~base_component() = default;
        virtual error destroy(entity) = 0;
    };

    template<typename T, typename MemoryLayout>
    class component : public base_component {
        static_assert(std::is_base_of_v<memory_layout::base_layout, MemoryLayout>,
                      "MemoryLayout must inherit layout interface");

    private:
        using component_array = std::array<T, ENTITY_COUNT>;
        component_array m_components;
        MemoryLayout m_layout;


    public:
        error add(entity e, T const &c) {
            auto const new_index = m_layout.add(e);
            if (new_index.has_value()) {
                m_components[new_index.value()] = c;
                return error::ok;
            }
            return new_index.error();
        }

        error remove(entity e) {
            auto const removed_entity = m_layout.remove(e);
            if (removed_entity.has_value()) {
                auto const last_index = m_layout.size();
                auto const removed_entity_index = removed_entity.value();

                m_components[removed_entity_index] = m_components[last_index];
                return error::ok;
            }
            return removed_entity.error();
        }

        T &get(entity e) {
            auto const entity_index = m_layout.get(e);
            if (entity_index.has_value()) {
                return m_components[entity_index.value()];
            }
            throw std::out_of_range(std::format("entity {} not found", e));
        }

        T get(entity e) const {
            auto const entity_index = m_layout.get(e);
            if (entity_index.has_value()) {
                return m_components[entity_index.value()];
            }
            throw std::out_of_range(std::format("entity {} not found", e));
        }
        error destroy(entity e) override { return remove(e); }

        [[nodiscard]] std::size_t size() const { return m_layout.size(); }
    };
} // namespace ecs
#endif // COMPONENT_HPP
