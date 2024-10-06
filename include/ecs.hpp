//
// Created by HP on 26.09.2024.
//

#ifndef ESC_HPP
#define ESC_HPP
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "component.hpp"
#include "entity.hpp"
#include "view.hpp"

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

        /**
         * @brief Creates a new entity in the ECS system.
         *
         * @return The newly created entity.
         */
        [[nodiscard]] entity create();

        /**
         * @brief Destroys an entity and its associated components.
         *
         * @param entity The entity to be destroyed.
         * @return An error indicating the result of the operation.
         *         Success: error::ok
         *         Else:    error::not_found or error::failed
         */
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

        /**
         * @brief Clears all entities and components from the ECS system.
         *
         * @return An error indicating the result of the operation.
         *         Success: error::ok
         *         Else:    error::failed
         */
        error clear() {
            m_entities.clear();
            for (auto [key, components]: m_components) {
                components->clear();
            }
            return error::ok;
        }

        /**
         * @brief Emplaces multiple default-constructible components to an entity.
         *
         * @tparam Components The types of the components to be emplaced.
         * @param entity The entity to which the components will be added.
         * @return An error indicating the result of the operation.
         *         Success: error::ok
         *         Else:    error::failed
         */
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

        /**
         * @brief Inserts a component owned by the entity.
         *
         * @tparam T The type of the component to insert.
         * @param entity The entity to which the component will be added.
         * @param component The component to be added.
         * @return An error indicating the result of the operation.
         *         Success: error::ok
         *         Else:    error::not_found or error::exists or error::max_entities
         */
        template<typename T>
        error insert(entity e, T const &component) {
            create_component<T>();
            try {
                return get_component_ptr<T>()->add(e, component);
            } catch (std::out_of_range const &) {
                return error::not_found;
            }
        }

        /**
         * @brief Checks if an entity owns a specific component.
         *
         * @tparam T The type of the component to check.
         * @param entity The entity to check.
         * @return true if the entity contains the component, false otherwise.
         */
        template<typename T>
        [[nodiscard]] bool contains(entity e) const {
            auto const type = typeid(T).name();
            if (!m_components.contains(type)) {
                return false;
            }
            return m_components.at(type)->contains(e);
        }

        /**
         * @brief Checks if an entity owns all specified components.
         *
         * @tparam Components The types of the components to check.
         * @param entity The entity to check.
         * @return true if the entity contains all specified components, false otherwise.
         */
        template<typename... Components>
        [[nodiscard]] bool all_of(entity e) const {
            try {
                bool const ok = ((contains<Components>(e)) && ...);
                return ok;
            } catch (std::out_of_range const &) {
                return false;
            }
        }

        /**
         * @brief Checks if an entity owns any of the specified components.
         *
         * @tparam Components The types of the components to check.
         * @param entity The entity to check.
         * @return true if the entity contains any of the specified components, false otherwise.
         */
        template<typename... Components>
        [[nodiscard]] bool any_of(entity e) const {
            try {
                bool const ok = ((contains<Components>(e)) || ...);
                return ok;
            } catch (std::out_of_range const &) {
                return false;
            }
        }

        /**
         * @brief Removes a component owned by the entity.
         *
         * @tparam T The type of the component to remove.
         * @param entity The entity from which the component will be removed.
         * @return An error indicating the result of the operation.
         *         Success: error::ok
         *         Else:    error::not_found
         */
        template<typename T>
        error erase(entity e) {
            return get_component_ptr<T>()->remove(e);
        }

        /**
         * @brief Retrieves a reference to a component of the specified type owned by an entity.
         *
         * @tparam T The type of the component to retrieve.
         * @param entity The entity from which the component will be retrieved.
         * @return A reference to the component.
         * @throws If the entity is not present an std::out_of_range exception is thrown
         */
        template<typename T>
        T &get(entity e) {
            return get_component_ptr<T>()->get(e);
        }

        /**
         * @brief Retrieves a copy to a component of the specified type owned by an entity.
         *
         * @tparam T The type of the component to retrieve.
         * @param entity The entity from which the component will be retrieved.
         * @return A reference to the component.
         * @throws If the entity is not present an std::out_of_range exception is thrown
         */
        template<typename T>
        T get(entity e) const {
            return get_component_ptr<T>()->get(e);
        }

        /**
         * @brief Retrieves tuple of references to components of the specified types owned by an entity.
         *
         * @tparam Components The type of the components to retrieve.
         * @param entity The entity from owning the components.
         * @return A tuple of references to the components.
         * @throws If one component for the entity is not present an std::out_of_range exception is thrown
         */
        template<typename... Components>
        std::tuple<Components &...> get_multiple(entity e) {
            return {get<Components>(e)...};
        }

        /**
         * @brief Retrieves tuple of copies to components of the specified types owned by an entity.
         *
         * @tparam Components The type of the components to retrieve.
         * @param entity The entity from owning the components.
         * @return A tuple of copies to the components.
         * @throws If one component for the entity is not present an std::out_of_range exception is thrown
         */
        template<typename... Components>
        std::tuple<Components...> get_multiple(entity e) const {
            return {get<Components>(e)...};
        }

        /**
         * @brief Retrieves a view of all entities that contain the specified components.
         *
         * @tparam Components The types of the components to filter by.
         * @return A view containing all entities that have the specified components.
         * @throws std::invalid_argument if the view cannot be created.
         */
        template<typename... Components>
        [[nodiscard]] view view() {
            std::unordered_set<entity> e;

            for (auto const entity: m_entities) {
                bool const ok = all_of<Components...>(entity);
                if (ok) {
                    e.emplace(entity);
                }
            }

            auto view = view::create_view(e, this);
            if (view.has_value()) {
                return std::move(view.value());
            }
            throw std::invalid_argument("could not create view, invalid ecs");
        }
    };

} // namespace ecs
#include "view.tpp"

#endif // ESC_HPP
