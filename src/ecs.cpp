//
// Created by HP on 26.09.2024.
//
#include "ecs.hpp"
#include <format>
std::string hello(std::string const &name) { return std::format("Hello {}", name); }

ecs::entity ecs::ecs::create() { return m_entities.create(); }
