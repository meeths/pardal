
#pragma once
#include <algorithm>
#include <Base/Function.h>
#include <Containers/Vector.h>

// Created on 2025-03-22 by sisco

namespace pdl
{

template <typename... Ts>
using Callback = Function<Ts ...>;
    
template <class T>
class EventCallbacks
{
public:
    EventCallbacks& operator += (const T& func)
    {
        m_callbacks.push_back(func);
        return *this;
    }

    template <typename... Ts>
    void operator () (Ts... args)
    {
        std::for_each(m_callbacks.begin(), m_callbacks.end(), [args ...](auto& callback) { callback(args...); });
    }

    EventCallbacks& operator -= (const T& func)
    {
        m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), func), m_callbacks.end());
        return *this;
    }
    
                
private:
    Vector<T> m_callbacks;     
};

}

