
#pragma once

// Created on 2023-12-16 by sisco
#include <functional>

// Created on 2023-12-15 by sisco

namespace pdl
{
    template <typename... Ts>
    using Function = std::function<Ts...>;
}