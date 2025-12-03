#pragma once

#include <atomic>
#include <cstddef>
#include <type_traits>
#include "SharedPointer.h"

namespace pdl
{
    template<class T>
    class WeakPointer
    {
        template<class> friend class WeakPointer;
    public:
        using element_type = T;

        // ctors
        constexpr WeakPointer() noexcept = default;

        // from SharedPointer
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        WeakPointer(const SharedPointer<U>& sp) noexcept
        {
            m_ctrl = sp.m_ctrl;
            if (m_ctrl)
            {
                m_ctrl->weak.fetch_add(1, std::memory_order_acq_rel);
            }
        }

        // copy
        WeakPointer(const WeakPointer& other) noexcept { Acquire(other); }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        WeakPointer(const WeakPointer<U>& other) noexcept { m_ctrl = other.m_ctrl; if (m_ctrl) m_ctrl->weak.fetch_add(1, std::memory_order_acq_rel); }
        WeakPointer& operator=(const WeakPointer& other) noexcept
        {
            if (this != &other)
            {
                Release();
                Acquire(other);
            }
            return *this;
        }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        WeakPointer& operator=(const WeakPointer<U>& other) noexcept
        {
            if (reinterpret_cast<const void*>(this) != reinterpret_cast<const void*>(&other))
            {
                Release();
                m_ctrl = other.m_ctrl;
                if (m_ctrl) m_ctrl->weak.fetch_add(1, std::memory_order_acq_rel);
            }
            return *this;
        }

        // move
        WeakPointer(WeakPointer&& other) noexcept { MoveFrom(std::move(other)); }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        WeakPointer(WeakPointer<U>&& other) noexcept { m_ctrl = other.m_ctrl; other.m_ctrl = nullptr; }
        WeakPointer& operator=(WeakPointer&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                MoveFrom(std::move(other));
            }
            return *this;
        }
        template<class U, class = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        WeakPointer& operator=(WeakPointer<U>&& other) noexcept
        {
            Release();
            m_ctrl = other.m_ctrl;
            other.m_ctrl = nullptr;
            return *this;
        }

        ~WeakPointer() { Release(); }

        void Reset() noexcept { Release(); }

        void Swap(WeakPointer& other) noexcept
        {
            auto* c = m_ctrl; m_ctrl = other.m_ctrl; other.m_ctrl = c;
        }

        bool IsExpired() const noexcept { return !m_ctrl || m_ctrl->strong.load(std::memory_order_acquire) == 0; }

        std::size_t UseCount() const noexcept { return m_ctrl ? m_ctrl->strong.load(std::memory_order_acquire) : 0; }

        SharedPointer<T> Lock() const noexcept
        {
            SharedPointer<T> out;
            SharedControl* ctrl = m_ctrl;
            if (!ctrl) return out;

            // Try to increment strong if non-zero
            std::size_t s = ctrl->strong.load(std::memory_order_acquire);
            while (s != 0)
            {
                if (ctrl->strong.compare_exchange_weak(s, s + 1, std::memory_order_acq_rel, std::memory_order_acquire))
                {
                    out.m_ctrl = ctrl;
                    out.m_ptr = static_cast<T*>(ctrl->Get(ctrl));
                    return out;
                }
                // s was updated with current value by failed CAS; loop
            }
            return out; // expired
        }

    private:
        void Acquire(const WeakPointer& other) noexcept
        {
            m_ctrl = other.m_ctrl;
            if (m_ctrl)
            {
                m_ctrl->weak.fetch_add(1, std::memory_order_acq_rel);
            }
        }
        void MoveFrom(WeakPointer&& other) noexcept
        {
            m_ctrl = other.m_ctrl;
            other.m_ctrl = nullptr;
        }
        void Release() noexcept
        {
            if (!m_ctrl) return;
            if (m_ctrl->weak.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                // if last weak and no owners, destroy control
                if (m_ctrl->strong.load(std::memory_order_acquire) == 0)
                {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    m_ctrl->Destroy(m_ctrl);
                }
            }
            m_ctrl = nullptr;
        }

        SharedControl* m_ctrl = nullptr;
    };
}
