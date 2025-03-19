
#pragma once
#include <unordered_set>

// Created on 2023-12-15 by sisco

namespace pdl
{
    template<typename... Ts>
    using UnorderedSet = std::unordered_set<Ts...>;
}

