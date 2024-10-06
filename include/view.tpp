#ifndef VIEW_TPP
#define VIEW_TPP

namespace ecs {

    template<typename T>
    T &view::get(entity e) {
        if (!m_entities.contains(e)) {
            throw std::out_of_range(std::format("entity {} not in view", static_cast<int>(e)));
        }
        return m_ecs->get<T>(e);
    }

    template<typename... Components>
    std::tuple<Components &...> view::get_multiple(entity e) {
        return {get<Components>(e)...};
    }
} // namespace ecs


#endif
