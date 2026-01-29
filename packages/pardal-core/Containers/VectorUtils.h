#pragma once

#include <vector>
#include <type_traits>
#include "Containers/Vector.h"

namespace pdl
{
    struct VectorUtils
    {
        // Create a pdl::Vector by copying from a std::vector
        template <typename T>
        static Vector<T> FromStd(std::vector<T>& src) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            Vector<T> dst;
            dst.reserve(static_cast<typename Vector<T>::size_type>(src.size()));
            for (auto& x : src)
            {
                dst.push_back(static_cast<T&>(x));
            }
            return dst;
        }

        // Create a pdl::Vector by moving from a std::vector
        // Note: std::vector's storage cannot be stolen; elements are moved individually.
        template <typename T>
        static Vector<T> FromStd(std::vector<T>&& src) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            Vector<T> dst;
            dst.reserve(static_cast<typename Vector<T>::size_type>(src.size()));
            for (auto& x : src)
            {
                dst.emplace_back(static_cast<T&&>(x));
            }
            // Leave src in a valid, empty state
            src.clear();
            return dst;
        }
        
        template <typename T>
        static std::vector<T> ToStd(Vector<T>& src) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            std::vector<T> dst;
            dst.reserve(static_cast<typename std::vector<T>::size_type>(src.size()));
            for (auto& x : src)
            {
                dst.push_back(static_cast<T&>(x));
            }
            return dst;
        }

        template <typename T>
        static std::vector<T> ToStd(Vector<T>&& src) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            std::vector<T> dst;
            dst.reserve(static_cast<typename std::vector<T>::size_type>(src.size()));
            for (auto& x : src)
            {
                dst.emplace_back(static_cast<T&&>(x));
            }
            // Leave src in a valid, empty state
            src.clear();
            return dst;
        }
    };

};

