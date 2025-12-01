
#pragma once
// Created on 2023-12-15 by sisco

#include <cstddef>
#include <type_traits>
#include <utility>
#include <algorithm>

namespace pdl
{
    template <class T, std::size_t N>
    struct Array
    {
        // Member types
        using value_type             = T;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using reference              = value_type&;
        using const_reference        = const value_type&;
        using pointer                = value_type*;
        using const_pointer          = const value_type*;
        using iterator               = value_type*;
        using const_iterator         = const value_type*;

        // Aggregate storage to allow brace-initialization
        value_type m_data[N > 0 ? N : 1];

        // Size queries
        static constexpr size_type size() noexcept { return N; }
        static constexpr bool empty() noexcept { return N == 0; }

        // Element access
        constexpr reference operator[](size_type idx) noexcept { return m_data[idx]; }
        constexpr const_reference operator[](size_type idx) const noexcept { return m_data[idx]; }

        // Like std::array::at but without exceptions; same as operator[] in this codebase
        constexpr reference at(size_type idx) noexcept { return m_data[idx]; }
        constexpr const_reference at(size_type idx) const noexcept { return m_data[idx]; }

        constexpr reference front() noexcept { return m_data[0]; }
        constexpr const_reference front() const noexcept { return m_data[0]; }
        constexpr reference back() noexcept { return m_data[N - 1]; }
        constexpr const_reference back() const noexcept { return m_data[N - 1]; }

        // Data access
        constexpr pointer data() noexcept { return m_data; }
        constexpr const_pointer data() const noexcept { return m_data; }

        // Iterators
        constexpr iterator begin() noexcept { return m_data; }
        constexpr const_iterator begin() const noexcept { return m_data; }
        constexpr const_iterator cbegin() const noexcept { return m_data; }
        constexpr iterator end() noexcept { return m_data + N; }
        constexpr const_iterator end() const noexcept { return m_data + N; }
        constexpr const_iterator cend() const noexcept { return m_data + N; }

        // Operations
        constexpr void fill(const value_type& value) noexcept(std::is_nothrow_copy_assignable_v<value_type>)
        {
            for (size_type i = 0; i < N; ++i) m_data[i] = value;
        }

        constexpr void swap(Array& other) noexcept(std::is_nothrow_swappable_v<value_type>)
        {
            if constexpr (N > 0)
            {
                for (size_type i = 0; i < N; ++i) std::swap(m_data[i], other.m_data[i]);
            }
        }

        // Comparisons (lexicographical, like std::array)
        friend constexpr bool operator==(const Array& a, const Array& b)
        {
            for (size_type i = 0; i < N; ++i) if (!(a.m_data[i] == b.m_data[i])) return false;
            return true;
        }
        friend constexpr bool operator!=(const Array& a, const Array& b) { return !(a == b); }
        friend constexpr bool operator<(const Array& a, const Array& b)
        {
            for (size_type i = 0; i < N; ++i)
            {
                if (a.m_data[i] < b.m_data[i]) return true;
                if (b.m_data[i] < a.m_data[i]) return false;
            }
            return false;
        }
        friend constexpr bool operator>(const Array& a, const Array& b) { return b < a; }
        friend constexpr bool operator<=(const Array& a, const Array& b) { return !(b < a); }
        friend constexpr bool operator>=(const Array& a, const Array& b) { return !(a < b); }
    };
}

// Provide ADL-visible get<I>(Array) overloads for structured bindings
namespace pdl
{
    template <std::size_t I, class T, std::size_t N>
    constexpr T& get(Array<T, N>& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return a.m_data[I]; }
    template <std::size_t I, class T, std::size_t N>
    constexpr const T& get(const Array<T, N>& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return a.m_data[I]; }
    template <std::size_t I, class T, std::size_t N>
    constexpr T&& get(Array<T, N>&& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return static_cast<T&&>(a.m_data[I]); }
    template <std::size_t I, class T, std::size_t N>
    constexpr const T&& get(const Array<T, N>&& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return static_cast<const T&&>(a.m_data[I]); }
}

// ADL swap
namespace std
{
    template <class T, std::size_t N>
    inline constexpr void swap(::pdl::Array<T, N>& a, ::pdl::Array<T, N>& b) noexcept(noexcept(a.swap(b)))
    {
        a.swap(b);
    }

    // tuple-like interface for structured bindings and std::get
    template <class T, std::size_t N>
    struct tuple_size<::pdl::Array<T, N>> : std::integral_constant<std::size_t, N> { };

    template <std::size_t I, class T, std::size_t N>
    struct tuple_element<I, ::pdl::Array<T, N>> { using type = T; };

    template <std::size_t I, class T, std::size_t N>
    constexpr T& get(::pdl::Array<T, N>& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return a.m_data[I]; }
    template <std::size_t I, class T, std::size_t N>
    constexpr const T& get(const ::pdl::Array<T, N>& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return a.m_data[I]; }
    template <std::size_t I, class T, std::size_t N>
    constexpr T&& get(::pdl::Array<T, N>&& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return static_cast<T&&>(a.m_data[I]); }
    template <std::size_t I, class T, std::size_t N>
    constexpr const T&& get(const ::pdl::Array<T, N>&& a) noexcept { static_assert(I < N, "pdl::Array get index out of range"); return static_cast<const T&&>(a.m_data[I]); }
}