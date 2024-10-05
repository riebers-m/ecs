//
// Created by HP on 27.09.2024.
//

#ifndef COMPRESSOR_HPP
#define COMPRESSOR_HPP

#include <tl/expected.hpp>
#include "error.hpp"
#include "types.hpp"

namespace memory_layout {
    class base_layout {
    public:
        virtual ~base_layout() = default;

        // Returns new array index of added entity
        virtual tl::expected<size_t, ecs::error> add(ecs::entity) = 0;
        // Returns index of given entity
        [[nodiscard]] virtual tl::expected<size_t, ecs::error> get(ecs::entity) const = 0;
        // Returns index of removed entity
        virtual tl::expected<size_t, ecs::error> remove(ecs::entity) = 0;
        virtual ecs::error clear() = 0;
        // Current size of entities
        [[nodiscard]] virtual size_t size() const = 0;
        [[nodiscard]] virtual bool contains(ecs::entity) const = 0;
    };

    class compressed : public base_layout {
    private:
        std::unordered_map<ecs::entity, std::size_t> m_entity_to_index{};
        std::unordered_map<std::size_t, ecs::entity> m_index_to_entity{};
        std::size_t m_entity_count{};

    public:
        compressed() = default;

        tl::expected<size_t, ecs::error> add(ecs::entity) override;
        [[nodiscard]] tl::expected<size_t, ecs::error> get(ecs::entity) const override;
        tl::expected<size_t, ecs::error> remove(ecs::entity) override;
        ecs::error clear() override;
        [[nodiscard]] size_t size() const override;
        [[nodiscard]] bool contains(ecs::entity) const override;
    };

} // namespace memory_layout
#endif // COMPRESSOR_HPP
