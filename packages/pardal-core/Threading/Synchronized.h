
#pragma once
#include <algorithm>

#include "Mutex.h"
#include "Base/BaseDefines.h"
#include "Base/DebugHelpers.h"

// Created on 2025-03-19 by sisco

namespace pdl
{
template <class T>
class Synchronized
{
public:
    class Locked;

    Synchronized() = default;
    Synchronized(T value) : m_value(std::move(value)) {}

    DeclareNonCopyable(Synchronized);

    Locked Lock();
private:
    Mutex m_mutex;
    T m_value;
};

template <class T>
class Synchronized<T>::Locked
{
public:
    Locked(Synchronized<T>& synchronized) : m_synchronized(&synchronized) { m_synchronized->m_mutex.Lock(); }

    DeclareNonCopyable(Locked);
    Locked(const Locked&& other) : m_synchronized(other.m_synchronized) { other.m_synchronized = nullptr; }
    Locked& operator=(Locked&& other) { std::swap(m_synchronized, other.m_synchronized); return *this; }

    T* operator -> () const & { pdlAssert(m_synchronized); return &m_synchronized->m_value; }
    T& operator * () const & { pdlAssert(m_synchronized); return m_synchronized->m_value; }
    T* operator -> () const && = delete;
    T& operator * () const && = delete;

    ~Locked() { if (m_synchronized) m_synchronized->m_mutex.Unlock(); }
private:
    Synchronized<T>* m_synchronized;
};
    
template <class T>
typename Synchronized<T>::Locked Synchronized<T>::Lock()
{
    return Locked(*this);
}
    
}

