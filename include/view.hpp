//
// Created by HP on 06.10.2024.
//

#ifndef VIEW_HPP
#define VIEW_HPP
#include <memory>
#include <tl/expected.hpp>
#include <unordered_set>
#include "types.hpp"
namespace ecs {
    class ecs;

    class view {
    private:
        std::unordered_set<entity> m_entities{};
        ecs *m_ecs{nullptr};

        explicit view(std::unordered_set<entity> const &entities, ecs *ecs) : m_entities{entities}, m_ecs{ecs} {}

    public:
        view() = delete;
        view &operator=(view const &) = delete;
        view(view const &) = delete;

        view(view &&other) noexcept : m_entities(std::exchange(other.m_entities, {})), m_ecs(other.m_ecs) {}

        view &operator=(view &&other) noexcept {
            if (this == std::addressof(other)) {
                return *this;
            }
            using std::swap;
            swap(m_entities, other.m_entities);
            swap(m_ecs, other.m_ecs);
            return *this;
        }

        static tl::expected<view, error> create_view(std::unordered_set<entity> const &entities, ecs *ecs) {
            if (!ecs) {
                return tl::unexpected(error::failed);
            }
            return view{entities, ecs};
        }

        template<typename T>
        T &get(entity e);

        template<typename... Components>
        std::tuple<Components &...> get_multiple(entity e);

        std::unordered_set<entity>::iterator begin() { return m_entities.begin(); }
        std::unordered_set<entity>::iterator end() { return m_entities.end(); }

        [[nodiscard]] std::unordered_set<entity>::const_iterator begin() const { return m_entities.begin(); }
        [[nodiscard]] std::unordered_set<entity>::const_iterator end() const { return m_entities.end(); }
    };
} // namespace ecs
#endif // VIEW_HPP
