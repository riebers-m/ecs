//
// Created by HP on 27.09.2024.
//

#ifndef ERROR_HPP
#define ERROR_HPP

namespace ecs {
    enum class error {
        ok,
        not_found,
        empty,
        max_entities,
        failed,
    };
}
#endif // ERROR_HPP
