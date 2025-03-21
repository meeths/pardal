
#pragma once

// Created on 2023-12-29 by sisco
#include <expected>

namespace pdl
{

    template<class T, class E>
    using Expected = std::expected<T, E>;
    template<class T>
    using Unexpected = std::unexpected<T>;

}

