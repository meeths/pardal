#pragma once

#include <cstddef>
#include <new>
#include <utility>
#include <type_traits>
#include <iterator>
#include <initializer_list>

namespace pdl
{
    template <typename T, std::size_t InlineCapacity = 8>
    class Vector
    {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;

        Vector() noexcept
            : m_data(inline_ptr()), m_size(0), m_capacity(InlineCapacity)
        {
        }

        explicit Vector(size_type count) noexcept
            : Vector()
        {
            resize(count);
        }

        // Initializer-list constructor to support brace initialization
        Vector(std::initializer_list<T> init) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : Vector()
        {
            reserve(static_cast<size_type>(init.size()));
            for (const auto& v : init)
            {
                push_back(v);
            }
        }

        // Iterator range constructor: constructs from [first, last)
        // SFINAE to avoid ambiguity with integral types (size constructor)
        template <class InputIt,
                  class = std::enable_if_t<!std::is_integral_v<InputIt>>>
        Vector(InputIt first, InputIt last) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : Vector()
        {
            // If we have random-access iterators, reserve upfront
            using Cat = typename std::iterator_traits<InputIt>::iterator_category;
            if constexpr (std::is_base_of_v<std::random_access_iterator_tag, Cat>)
            {
                const auto n = static_cast<size_type>(last - first);
                reserve(n);
            }
            for (auto it = first; it != last; ++it)
            {
                push_back(*it);
            }
        }

        Vector(const Vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : Vector()
        {
            reserve(other.m_size);
            uninitialized_copy(other.m_data, other.m_size);
            m_size = other.m_size;
        }

        Vector(Vector&& other) noexcept
            : m_size(other.m_size)
        {
            if (other.using_inline())
            {
                // Move-construct into our inline storage
                m_data = inline_ptr();
                m_capacity = InlineCapacity;
                uninitialized_move(other.m_data, other.m_size, inline_ptr());
                // Destroy elements in other's inline storage
                other.destroy_range(0, other.m_size);
                other.m_size = 0;
                other.m_capacity = InlineCapacity;
            }
            else
            {
                // Steal heap buffer without touching original memory
                m_data = other.m_data;
                m_capacity = other.m_capacity;
                other.m_data = other.inline_ptr();
                other.m_size = 0;
                other.m_capacity = InlineCapacity;
            }
        }

        ~Vector()
        {
            destroy_range(0, m_size);
            if (!using_inline())
            {
                deallocate(m_data, m_capacity);
            }
        }

        Vector& operator=(const Vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            if (this == &other) return *this;
            clear();
            reserve(other.m_size);
            uninitialized_copy(other.m_data, other.m_size);
            m_size = other.m_size;
            return *this;
        }

        Vector& operator=(Vector&& other) noexcept
        {
            if (this == &other) return *this;
            // Destroy current
            destroy_range(0, m_size);
            if (!using_inline())
                deallocate(m_data, m_capacity);

            if (other.using_inline())
            {
                m_data = inline_ptr();
                m_capacity = InlineCapacity;
                m_size = other.m_size;
                uninitialized_move(other.m_data, other.m_size, inline_ptr());
                other.destroy_range(0, other.m_size);
                other.m_size = 0;
            }
            else
            {
                m_data = other.m_data;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                other.m_data = other.inline_ptr();
                other.m_size = 0;
                other.m_capacity = InlineCapacity;
            }
            return *this;
        }

        // Capacity & size
        size_type size() const noexcept { return m_size; }
        size_type capacity() const noexcept { return m_capacity; }
        bool empty() const noexcept { return m_size == 0; }

        // Element access
        reference operator[](size_type idx) noexcept { return m_data[idx]; }
        const_reference operator[](size_type idx) const noexcept { return m_data[idx]; }
        pointer data() noexcept { return m_data; }
        const_pointer data() const noexcept { return m_data; }

        // Optional bounds-checked accessors (no exceptions; behaves like operator[])
        reference at(size_type idx) noexcept { return m_data[idx]; }
        const_reference at(size_type idx) const noexcept { return m_data[idx]; }

        // Iterators
        iterator begin() noexcept { return m_data; }
        const_iterator begin() const noexcept { return m_data; }
        const_iterator cbegin() const noexcept { return m_data; }
        iterator end() noexcept { return m_data + m_size; }
        const_iterator end() const noexcept { return m_data + m_size; }
        const_iterator cend() const noexcept { return m_data + m_size; }

        // Modifiers
        void clear() noexcept { destroy_range(0, m_size); m_size = 0; }

        void reserve(size_type new_cap) noexcept
        {
            if (new_cap <= m_capacity) return;
            reallocate(new_cap);
        }

        void resize(size_type count) noexcept(std::is_nothrow_default_constructible_v<T>)
        {
            if (count < m_size)
            {
                destroy_range(count, m_size);
                m_size = count;
                return;
            }
            if (count > m_capacity)
            {
                size_type new_cap = grow_to_fit(count);
                reallocate(new_cap);
            }
            // Default-construct new elements
            for (; m_size < count; ++m_size)
            {
                ::new (static_cast<void*>(m_data + m_size)) T();
            }
        }

        // Resize to count elements, value-initializing added elements with a copy of 'value'
        void resize(size_type count, const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            if (count < m_size)
            {
                destroy_range(count, m_size);
                m_size = count;
                return;
            }
            if (count > m_capacity)
            {
                size_type new_cap = grow_to_fit(count);
                reallocate(new_cap);
            }
            for (; m_size < count; ++m_size)
            {
                ::new (static_cast<void*>(m_data + m_size)) T(value);
            }
        }

        void push_back(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            ensure_growth(1);
            ::new (static_cast<void*>(m_data + m_size)) T(value);
            ++m_size;
        }

        void push_back(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            ensure_growth(1);
            ::new (static_cast<void*>(m_data + m_size)) T(static_cast<T&&>(value));
            ++m_size;
        }

        template <class... Args>
        reference emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        {
            ensure_growth(1);
            ::new (static_cast<void*>(m_data + m_size)) T(static_cast<Args&&>(args)...);
            ++m_size;
            return back();
        }

        void pop_back() noexcept
        {
            if (m_size == 0) return;
            --m_size;
            destroy_at(m_data + m_size);
        }

        // Access first element (no bounds checking; undefined if empty, like std::vector)
        reference front() noexcept { return m_data[0]; }
        const_reference front() const noexcept { return m_data[0]; }

        reference back() noexcept { return m_data[m_size - 1]; }
        const_reference back() const noexcept { return m_data[m_size - 1]; }

        // For tests/inspection only; not part of std::vector API.
        bool using_inline() const noexcept { return m_data == inline_ptr(); }

    private:
        // Storage utilities
        static void destroy_at(T* p) noexcept
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                p->~T();
            }
        }

        void destroy_range(size_type first, size_type last) noexcept
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                for (size_type i = first; i < last; ++i)
                    (m_data + i)->~T();
            }
        }

        void* allocate_raw(size_type count) noexcept
        {
            // Allocate uninitialized raw memory for count T objects.
            return ::operator new[](count * sizeof(T), std::nothrow);
        }

        void deallocate(T* ptr, size_type /*cap*/) noexcept
        {
            ::operator delete[](ptr, std::nothrow);
        }

        void uninitialized_copy(const T* src, size_type n, T* dst) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            for (size_type i = 0; i < n; ++i)
                ::new (static_cast<void*>(dst + i)) T(src[i]);
        }

        void uninitialized_copy(const T* src, size_type n) noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            uninitialized_copy(src, n, m_data);
        }

        void uninitialized_move(T* src, size_type n, T* dst) noexcept
        {
            for (size_type i = 0; i < n; ++i)
                ::new (static_cast<void*>(dst + i)) T(static_cast<T&&>(src[i]));
        }

        size_type grow_to_fit(size_type min_cap) const noexcept
        {
            size_type new_cap = m_capacity;
            while (new_cap < min_cap)
                new_cap = new_cap ? new_cap * 2 : InlineCapacity;
            return new_cap;
        }

        void ensure_growth(size_type add) noexcept
        {
            if (m_size + add > m_capacity)
            {
                size_type new_cap = grow_to_fit(m_size + add);
                reallocate(new_cap);
            }
        }

        void reallocate(size_type new_cap) noexcept
        {
            T* new_mem;
            if (new_cap <= InlineCapacity)
            {
                new_mem = inline_ptr();
            }
            else
            {
                new_mem = static_cast<T*>(allocate_raw(new_cap));
                if (!new_mem)
                {
                    // Allocation failed; keep existing storage. No exceptions available.
                    return;
                }
            }

            // Move-construct into new storage
            if (new_mem != m_data)
            {
                uninitialized_move(m_data, m_size, new_mem);
                // Destroy old and free if heap
                destroy_range(0, m_size);
                if (!using_inline())
                    deallocate(m_data, m_capacity);
                m_data = new_mem;
            }
            m_capacity = new_cap < InlineCapacity ? InlineCapacity : new_cap;
        }

        T* inline_ptr() noexcept { return reinterpret_cast<T*>(&m_inline[0]); }
        const T* inline_ptr() const noexcept { return reinterpret_cast<const T*>(&m_inline[0]); }

        T* m_data;
        size_type m_size{0};
        size_type m_capacity{InlineCapacity};
        alignas(T) unsigned char m_inline[sizeof(T) * InlineCapacity];
    };
}
