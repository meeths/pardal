
#pragma once

// Created on 2023-12-16 by sisco

#include <cstddef>
#include <utility>
#include <type_traits>
#include <new>

namespace pdl
{
    // DefaultDeleter for single objects
    template <class T>
    struct DefaultDeleter
    {
        constexpr DefaultDeleter() noexcept = default;
        void operator()(T* p) const noexcept { delete p; }
    };

    // DefaultDeleter partial specialization for arrays
    template <class T>
    struct DefaultDeleter<T[]>
    {
        constexpr DefaultDeleter() noexcept = default;
        template <class U>
        void operator()(U* p) const noexcept { delete[] p; }
    };

    // UniquePointer for single objects
    template <class T, class Deleter = DefaultDeleter<T>>
    class UniquePointer
    {
    public:
        using element_type = T;
        using deleter_type  = Deleter;

        // ctors
        constexpr UniquePointer() noexcept = default;
        constexpr UniquePointer(std::nullptr_t) noexcept {}
        explicit UniquePointer(T* p) noexcept : m_ptr(p) {}

        // move
        UniquePointer(UniquePointer&& other) noexcept : m_ptr(other.release()), m_deleter(std::move(other.m_deleter)) {}
        UniquePointer& operator=(UniquePointer&& other) noexcept
        {
            if (this != &other)
            {
                reset(other.release());
                m_deleter = std::move(other.m_deleter);
            }
            return *this;
        }

        // non-copyable
        UniquePointer(const UniquePointer&) = delete;
        UniquePointer& operator=(const UniquePointer&) = delete;

        ~UniquePointer() { if (m_ptr) m_deleter(m_ptr); }

        // observers
        T* get() const noexcept { return m_ptr; }
        T& operator*() const noexcept { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr; }
        explicit operator bool() const noexcept { return m_ptr != nullptr; }

        deleter_type& get_deleter() noexcept { return m_deleter; }
        const deleter_type& get_deleter() const noexcept { return m_deleter; }

        // modifiers
        T* release() noexcept { T* p = m_ptr; m_ptr = nullptr; return p; }
        void reset(T* p = nullptr) noexcept
        {
            T* old = m_ptr; m_ptr = p;
            if (old) m_deleter(old);
        }
        void swap(UniquePointer& other) noexcept
        {
            using std::swap;
            swap(m_ptr, other.m_ptr);
            swap(m_deleter, other.m_deleter);
        }

    private:
        T* m_ptr = nullptr;
        deleter_type m_deleter{};
    };

    // Partial specialization for arrays T[]
    template <class T, class Deleter>
    class UniquePointer<T[], Deleter>
    {
    public:
        using element_type = T;
        using deleter_type  = Deleter;

        constexpr UniquePointer() noexcept = default;
        constexpr UniquePointer(std::nullptr_t) noexcept {}
        explicit UniquePointer(T* p) noexcept : m_ptr(p) {}

        UniquePointer(UniquePointer&& other) noexcept : m_ptr(other.release()), m_deleter(std::move(other.m_deleter)) {}
        UniquePointer& operator=(UniquePointer&& other) noexcept
        {
            if (this != &other)
            {
                reset(other.release());
                m_deleter = std::move(other.m_deleter);
            }
            return *this;
        }

        UniquePointer(const UniquePointer&) = delete;
        UniquePointer& operator=(const UniquePointer&) = delete;

        ~UniquePointer() { if (m_ptr) m_deleter(m_ptr); }

        // observers
        T* get() const noexcept { return m_ptr; }
        T& operator[](std::size_t i) const noexcept { return m_ptr[i]; }
        explicit operator bool() const noexcept { return m_ptr != nullptr; }

        deleter_type& get_deleter() noexcept { return m_deleter; }
        const deleter_type& get_deleter() const noexcept { return m_deleter; }

        // modifiers
        T* release() noexcept { T* p = m_ptr; m_ptr = nullptr; return p; }
        void reset(T* p = nullptr) noexcept
        {
            T* old = m_ptr; m_ptr = p;
            if (old) m_deleter(old);
        }
        void swap(UniquePointer& other) noexcept
        {
            using std::swap;
            swap(m_ptr, other.m_ptr);
            swap(m_deleter, other.m_deleter);
        }

    private:
        T* m_ptr = nullptr;
        deleter_type m_deleter{};
    };

    // MakeUniquePointer for non-array types
    template <class T, class... Args, class = std::enable_if_t<!std::is_array<T>::value>>
    UniquePointer<T> MakeUniquePointer(Args&&... args) noexcept
    {
        T* p = new (std::nothrow) T(std::forward<Args>(args)...);
        return UniquePointer<T>(p);
    }

    // MakeUniquePointer for array of unknown bound
    template <class T, class = std::enable_if_t<std::is_array<T>::value && std::extent<T>::value == 0>>
    UniquePointer<T> MakeUniquePointer(std::size_t n) noexcept
    {
        using E = std::remove_extent_t<T>;
        E* p = new (std::nothrow) E[n]{}; // value-initialize
        return UniquePointer<T>(p);
    }

    // Disable MakeUniquePointer for arrays of known bound
    template <class T, class... Args, class = std::enable_if_t<std::is_array<T>::value && (std::extent<T>::value != 0)>, class = void>
    void MakeUniquePointer(Args&&...) = delete;
}
