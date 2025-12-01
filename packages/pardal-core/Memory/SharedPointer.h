
#pragma once

// Created on 2023-12-16 by sisco

#include <atomic>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace pdl
{
    #ifdef MakeSharedPointer
    #undef MakeSharedPointer
    #endif
    struct SharedControl
    {
        std::atomic<std::size_t> ref{1};
        void (*destroy)(SharedControl*) = nullptr;
    };
    // Lightweight, fast shared pointer without using std::shared_ptr.
    // - No exceptions are thrown.
    // - Thread-safe reference counting via atomics.
    // - Optimized MakeSharedPointer performs a single allocation for control + object.
    template <class T>
    class SharedPointer
    {
        template<class> friend class SharedPointer;

        template <class U>
        struct RawControl : SharedControl
        {
            U* p = nullptr;
        };

        template <class U>
        struct InplaceBlock
        {
            // Keep ctrl first for natural alignment and address computations
            SharedControl ctrl{};
            alignas(U) unsigned char storage[sizeof(U)];

            U* obj_ptr() noexcept { return reinterpret_cast<U*>(storage); }
        };

    public:
        using element_type = T;

        // ctors
        constexpr SharedPointer() noexcept = default;

        // Construct from raw pointer (adopts ownership). Prefer MakeSharedPointer for efficiency.
        explicit SharedPointer(T* p) noexcept
        {
            if (!p) return;
            using RC = RawControl<T>;
            RC* rc = static_cast<RC*>(::operator new(sizeof(RC), std::nothrow));
            if (!rc)
            {
                // Allocation failed; leak p to avoid calling delete with OOM in no-except builds.
                // Leave this as empty pointer to avoid UB.
                return;
            }
            rc->ref.store(1, std::memory_order_relaxed);
            rc->p = p;
            rc->destroy = [](SharedControl* base)
            {
                RC* self = static_cast<RC*>(base);
                delete self->p;
                ::operator delete(self);
            };
            m_ptr = p;
            m_ctrl = rc;
        }

        // Copy
        SharedPointer(const SharedPointer& other) noexcept { acquire(other); }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        SharedPointer(const SharedPointer<U>& other) noexcept { acquire_alias(other.m_ctrl, static_cast<T*>(other.get())); }
        SharedPointer& operator=(const SharedPointer& other) noexcept
        {
            if (this != &other)
            {
                release();
                acquire(other);
            }
            return *this;
        }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        SharedPointer& operator=(const SharedPointer<U>& other) noexcept
        {
            if (reinterpret_cast<const void*>(this) != reinterpret_cast<const void*>(&other))
            {
                release();
                acquire_alias(other.m_ctrl, static_cast<T*>(other.get()));
            }
            return *this;
        }

        // Move
        SharedPointer(SharedPointer&& other) noexcept { move_from(std::move(other)); }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        SharedPointer(SharedPointer<U>&& other) noexcept { m_ptr = static_cast<T*>(other.get()); m_ctrl = other.m_ctrl; other.m_ptr = nullptr; other.m_ctrl = nullptr; }
        SharedPointer& operator=(SharedPointer&& other) noexcept
        {
            if (this != &other)
            {
                release();
                move_from(std::move(other));
            }
            return *this;
        }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        SharedPointer& operator=(SharedPointer<U>&& other) noexcept
        {
            release();
            m_ptr = static_cast<T*>(other.get());
            m_ctrl = other.m_ctrl;
            other.m_ptr = nullptr; other.m_ctrl = nullptr;
            return *this;
        }

        ~SharedPointer() { release(); }

        // Observers
        T* get() const noexcept { return m_ptr; }
        T& operator*() const noexcept { return *m_ptr; }
        T* operator->() const noexcept { return m_ptr; }
        explicit operator bool() const noexcept { return m_ptr != nullptr; }
        std::size_t use_count() const noexcept { return m_ctrl ? m_ctrl->ref.load(std::memory_order_acquire) : 0; }

        // (No member casting helpers; see std::static_pointer_cast adapters below)

        // Modifiers
        void reset() noexcept { release(); }
        void reset(T* p) noexcept
        {
            release();
            if (p)
            {
                using RC = RawControl<T>;
                RC* rc = static_cast<RC*>(::operator new(sizeof(RC), std::nothrow));
                if (!rc)
                {
                    return;
                }
                rc->ref.store(1, std::memory_order_relaxed);
                rc->p = p;
                rc->destroy = [](SharedControl* base)
                {
                    RC* self = static_cast<RC*>(base);
                    delete self->p;
                    ::operator delete(self);
                };
                m_ptr = p;
                m_ctrl = rc;
            }
        }
        void swap(SharedPointer& other) noexcept
        {
            auto* p = m_ptr; m_ptr = other.m_ptr; other.m_ptr = p;
            auto* c = m_ctrl; m_ctrl = other.m_ctrl; other.m_ctrl = c;
        }

        // Create an aliasing SharedPointer<U> that shares ownership but points to p
        template<class U>
        SharedPointer<U> alias_cast(U* p) const noexcept
        {
            SharedPointer<U> out;
            if (m_ctrl)
            {
                out.m_ctrl = m_ctrl;
                out.m_ptr = p;
                out.m_ctrl->ref.fetch_add(1, std::memory_order_acq_rel);
            }
            return out;
        }

        // Friend factory uses single allocation
        template <class U, class... Args>
        friend SharedPointer<U> MakeSharedPointer(Args&&... args) noexcept;

    private:
        void acquire(const SharedPointer& other) noexcept
        {
            m_ptr = other.m_ptr;
            m_ctrl = other.m_ctrl;
            if (m_ctrl)
            {
                m_ctrl->ref.fetch_add(1, std::memory_order_acq_rel);
            }
        }

        void acquire_alias(SharedControl* ctrl, T* ptr) noexcept
        {
            m_ptr = ptr;
            m_ctrl = ctrl;
            if (m_ctrl)
            {
                m_ctrl->ref.fetch_add(1, std::memory_order_acq_rel);
            }
        }

        void move_from(SharedPointer&& other) noexcept
        {
            m_ptr = other.m_ptr;
            m_ctrl = other.m_ctrl;
            other.m_ptr = nullptr;
            other.m_ctrl = nullptr;
        }

        void release() noexcept
        {
            if (!m_ctrl) return;
            if (m_ctrl->ref.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                // Last owner
                std::atomic_thread_fence(std::memory_order_acquire);
                m_ctrl->destroy(m_ctrl);
            }
            m_ptr = nullptr;
            m_ctrl = nullptr;
        }

    private:
        T* m_ptr = nullptr;
        SharedControl* m_ctrl = nullptr;
    };

    // Single-allocation factory. No exceptions; returns empty on allocation failure.
    template <class T, class... Args>
    SharedPointer<T> MakeSharedPointer(Args&&... args) noexcept
    {
        using Block = typename SharedPointer<T>::template InplaceBlock<T>;
        // Allocate one block for control + object
        Block* blk = static_cast<Block*>(::operator new(sizeof(Block), std::nothrow));
        if (!blk)
        {
            return SharedPointer<T>();
        }
        // Initialize control
        blk->ctrl.ref.store(1, std::memory_order_relaxed);
        blk->ctrl.destroy = [](SharedControl* base)
        {
            Block* self = reinterpret_cast<Block*>(reinterpret_cast<char*>(base) - offsetof(Block, ctrl));
            self->obj_ptr()->~T();
            ::operator delete(self);
        };
        // Construct object in-place
        T* obj = new (blk->storage) T(std::forward<Args>(args)...);

        SharedPointer<T> sp;
        sp.m_ptr = obj;
        sp.m_ctrl = &blk->ctrl;
        return sp;
    }

}

// Casting helpers in pdl namespace
namespace pdl
{
    template<class To, class From>
    inline SharedPointer<To> StaticPointerCast(const SharedPointer<From>& sp) noexcept
    {
        return sp.template alias_cast<To>(static_cast<To*>(sp.get()));
    }
    template<class To, class From>
    inline SharedPointer<To> StaticPointerCast(SharedPointer<From>& sp) noexcept
    {
        return sp.template alias_cast<To>(static_cast<To*>(sp.get()));
    }
    template<class To, class From>
    inline SharedPointer<To> StaticPointerCast(SharedPointer<From>&& sp) noexcept
    {
        // alias and let the temporary decrement at end of function
        return sp.template alias_cast<To>(static_cast<To*>(sp.get()));
    }
}

// Provide std::static_pointer_cast adapters for pdl::SharedPointer
namespace std
{
    template<class To, class From>
    inline ::pdl::SharedPointer<To> static_pointer_cast(const ::pdl::SharedPointer<From>& sp) noexcept
    {
        return ::pdl::StaticPointerCast<To>(sp);
    }
    template<class To, class From>
    inline ::pdl::SharedPointer<To> static_pointer_cast(::pdl::SharedPointer<From>& sp) noexcept
    {
        return ::pdl::StaticPointerCast<To>(sp);
    }
    template<class To, class From>
    inline ::pdl::SharedPointer<To> static_pointer_cast(::pdl::SharedPointer<From>&& sp) noexcept
    {
        return ::pdl::StaticPointerCast<To>(sp);
    }
}
