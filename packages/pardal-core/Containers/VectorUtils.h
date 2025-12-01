#pragma once

#include <vector>
#include <type_traits>
#include "Containers/Vector.h"

namespace pdl
{
    struct VectorUtils
    {
        // Create a pdl::Vector by copying from a std::vector
        template <typename T, std::size_t InlineCapacity = 8>
        static Vector<T, InlineCapacity> FromStd(const std::vector<T>& src) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            // Leverage Vector's iterator range constructor (reserves when random-access)
            return Vector<T, InlineCapacity>(src.begin(), src.end());
        }

        // Create a pdl::Vector by moving from a std::vector
        // Note: std::vector's storage cannot be stolen; elements are moved individually.
        template <typename T, std::size_t InlineCapacity = 8>
        static Vector<T, InlineCapacity> FromStd(std::vector<T>&& src) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            Vector<T, InlineCapacity> dst;
            dst.reserve(static_cast<typename Vector<T, InlineCapacity>::size_type>(src.size()));
            for (auto& x : src)
            {
                dst.emplace_back(static_cast<T&&>(x));
            }
            // Leave src in a valid, empty state
            src.clear();
            return dst;
        }
    };
}
