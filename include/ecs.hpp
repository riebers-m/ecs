//
// Created by HP on 26.09.2024.
//

#ifndef ESC_HPP
#define ESC_HPP
#include <memory>
#include <stdexcept>
#include <string>
#include "component.hpp"
#include "entity.hpp"

std::string hello(std::string const &name);

namespace ecs {
    class ecs {
        using component_store = std::unordered_map<std::string, std::shared_ptr<base_component>>;

    private:
        entity_store m_entities;
        component_store m_components;

        template<typename T>
        std::shared_ptr<component<T, memory_layout::compressed>> get_component_ptr() {
            auto const type = typeid(T).name();
            return std::static_pointer_cast<component<T, memory_layout::compressed>>(m_components.at(type));
        }

        template<typename T>
        void create_component() {
            auto const type_id = typeid(T).name();
            if (!m_components.contains(type_id)) {
                m_components[type_id] = std::make_shared<component<T, memory_layout::compressed>>();
            }
        }

        template<typename T>
        error emplace_component(entity e) {
            static_assert(std::is_default_constructible_v<T>, "component has to be default constructable");
            create_component<T>();
            try {
                return get_component_ptr<T>()->add(e, T{});
            } catch (std::out_of_range const &) {
                return error::failed;
            }
        }

    public:
        ecs() = default;

        [[nodiscard]] entity create();
        error destroy(entity e) {
            auto err = error::ok;
            if (err = m_entities.destroy(e); err != error::ok) {
                return err;
            }

            for (auto [key, array]: m_components) {
                if (err = array->destroy(e); err != error::ok) {
                    return err;
                }
            }
            return err;
        }

        error clear() {
            m_entities.clear();
            for (auto [key, components]: m_components) {
                components->clear();
            }
            return error::ok;
        }

        template<typename... Components>
        error emplace(entity e) {
            static_assert((std::is_default_constructible_v<Components> && ...),
                          "components has to be default constructable");
            bool const all_ok = ((emplace_component<Components>(e) == error::ok) && ...);
            if (!all_ok) {
                return error::failed;
            }

            return error::ok;
        }

        template<typename T>
        error insert(entity e, T const &component) {
            create_component<T>();
            try {
                return get_component_ptr<T>()->add(e, component);
            } catch (std::out_of_range const &) {
                return error::not_found;
            }
        }

        template<typename T>
        [[nodiscard]] bool contains(entity e) const {
            auto const type = typeid(T).name();
            if (!m_components.contains(type)) {
                return false;
            }
            return m_components.at(type)->contains(e);
        }

        template<typename... Components>
        [[nodiscard]] bool all_of(entity e) const {
            try {
                bool const ok = ((contains<Components>(e)) && ...);
                return ok;
            } catch (std::out_of_range const &) {
                return false;
            }
        }

        template<typename... Components>
        [[nodiscard]] bool any_of(entity e) const {
            try {
                bool const ok = ((contains<Components>(e)) || ...);
                return ok;
            } catch (std::out_of_range const &) {
                return false;
            }
        }

        template<typename T>
        error erase(entity e) {
            return get_component_ptr<T>()->remove(e);
        }

        template<typename T>
        T &get(entity e) {
            return get_component_ptr<T>()->get(e);
        }

        template<typename T>
        T get(entity e) const {
            return get_component_ptr<T>()->get(e);
        }

        template<typename... Components>
        std::tuple<Components &...> get_multiple(entity e) {
            return {get<Components>(e)...};
        }

        template<typename... Components>
        std::tuple<Components...> get_multiple(entity e) const {
            return {get<Components>(e)...};
        }
    };

} // namespace ecs

#endif // ESC_HPP
