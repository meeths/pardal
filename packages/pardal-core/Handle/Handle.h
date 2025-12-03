#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include "Memory/SharedPointer.h"
#include "Memory/WeakPointer.h"

namespace pdl
{
    template<class T>
    class WeakHandle;

    template<class T>
    class Handle
    {
        template<class> friend class Handle;
        template<class> friend class WeakHandle;
        template<class U, class... Args>
        friend Handle<U> MakeHandle(Args&&...) noexcept;
    public:
        using element_type = T;

        // ctors
        constexpr Handle() noexcept = default;

        // construct from raw pointer (adopts ownership)
        explicit Handle(T* ptr) noexcept : m_ptr(ptr) {}
        // construct from SharedPointer (shares ownership)
        explicit Handle(SharedPointer<T> sp) noexcept : m_ptr(std::move(sp)) {}
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        explicit Handle(SharedPointer<U> sp) noexcept : m_ptr(std::move(sp)) {}

        // copy/move
        Handle(const Handle&) noexcept = default;
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        Handle(const Handle<U>& other) noexcept : m_ptr(other.m_ptr) {}
        Handle& operator=(const Handle&) noexcept = default;
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        Handle& operator=(const Handle<U>& other) noexcept { m_ptr = other.m_ptr; return *this; }
        Handle(Handle&&) noexcept = default;
        Handle& operator=(Handle&&) noexcept = default;

        // modifiers
        void Reset() noexcept { m_ptr.Reset(); }
        void Swap(Handle& other) noexcept { m_ptr.Swap(other.m_ptr); }

        // observers
        T* Get() const noexcept { return m_ptr.Get(); }
        T& operator*() const noexcept { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr.Get(); }
        explicit operator bool() const noexcept { return m_ptr.Get() != nullptr; }

        std::size_t UseCount() const noexcept { return m_ptr.UseCount(); }
        bool IsUnique() const noexcept { return UseCount() == 1; }

        // Lifecycle helpers
        bool IsValid() const noexcept { return m_ptr.Get() != nullptr; }

        WeakHandle<T> Weak() const noexcept;

    private:
        SharedPointer<T> m_ptr{};
    };

    template<class T>
    class WeakHandle
    {
        template<class> friend class Handle;
    public:
        constexpr WeakHandle() noexcept = default;

        // from Handle
        explicit WeakHandle(const Handle<T>& h) noexcept : m_weak(h.m_ptr) {}
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        explicit WeakHandle(const Handle<U>& h) noexcept : m_weak(h.m_ptr) {}

        // copy/move
        WeakHandle(const WeakHandle&) noexcept = default;
        WeakHandle& operator=(const WeakHandle&) noexcept = default;
        WeakHandle(WeakHandle&&) noexcept = default;
        WeakHandle& operator=(WeakHandle&&) noexcept = default;

        void Reset() noexcept { m_weak.Reset(); }
        bool IsExpired() const noexcept { return m_weak.IsExpired(); }
        std::size_t UseCount() const noexcept { return m_weak.UseCount(); }

        Handle<T> Lock() const noexcept { Handle<T> h; h.m_ptr = m_weak.Lock(); return h; }

    private:
        WeakPointer<T> m_weak{};
    };

    template<class T>
    inline WeakHandle<T> Handle<T>::Weak() const noexcept { return WeakHandle<T>(*this); }

    // Factory akin to std::make_shared — constructs T and returns Handle<T>
    template<class T, class... Args>
    inline Handle<T> MakeHandle(Args&&... args) noexcept
    {
        Handle<T> h;
        h.m_ptr = MakeSharedPointer<T>(std::forward<Args>(args)...);
        return h;
    }
}
