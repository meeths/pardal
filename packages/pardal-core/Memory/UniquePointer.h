
#pragma once

// Created on 2023-12-16 by sisco
#include <memory>

namespace pdl
{
    template<typename... Ts>
    using UniquePointer = std::unique_ptr<Ts...>;
    #define MakeUniquePointer std::make_unique
}
