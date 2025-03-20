
#pragma once

// Created on 2023-12-16 by sisco

#include <memory>

namespace pdl
{
    template <typename... Ts>
    using SharedPointer = std::shared_ptr<Ts...>;
    #define MakeSharedPointer std::make_shared
}
