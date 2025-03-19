
#pragma once
#include <algorithm>

#include "Mutex.h"
#include "SRWLock.h"
#include "Base/BaseDefines.h"
#include "Base/DebugHelpers.h"

// Created on 2025-03-19 by sisco

namespace pdl
{
    template <class T>
    class SRWSynchronized
    {
    public:
        class LockedForRead;
        class LockedForWrite;

        SRWSynchronized() = default;
        ~SRWSynchronized() = default;
        SRWSynchronized(T value) : m_value(std::move(value)) {}

        DeclareNonCopyable(SRWSynchronized);
        DeclareDefaultMoveable(SRWSynchronized);

        LockedForRead LockForRead();
        LockedForWrite LockForWrite();
    private:
        SRWLock m_lock;
        T m_value;
    };

    template <class T>
    class SRWSynchronized<T>::LockedForRead
    {
    public:
        LockedForRead(SRWSynchronized<T>& synchronized) : m_synchronized(&synchronized) { m_synchronized->m_lock.LockRead(); }

        DeclareNonCopyable(LockedForRead);
        
        LockedForRead(const LockedForRead&& other) : m_synchronized(other.m_synchronized) { other.m_synchronized = nullptr; }
        LockedForRead& operator=(LockedForRead&& other) { std::swap(m_synchronized, other.m_synchronized); return *this; }

        const T* operator -> () const & { pdlAssert(m_synchronized); return &m_synchronized->m_value; }
        const T& operator * () const & { pdlAssert(m_synchronized); return m_synchronized->m_value; }
        T* operator -> () const && = delete;
        T& operator * () const && = delete;
        
        ~LockedForRead() { if (m_synchronized) m_synchronized->m_lock.UnlockRead(); }
    private:
        SRWSynchronized<T>* m_synchronized;
    };

    template <class T>
    class SRWSynchronized<T>::LockedForWrite
    {
    public:
        LockedForWrite(SRWSynchronized<T>& synchronized) : m_synchronized(&synchronized) { m_synchronized->m_lock.LockWrite(); }

        DeclareNonCopyable(LockedForWrite);
        
        LockedForWrite(const LockedForWrite&& other) : m_synchronized(other.m_synchronized) { other.m_synchronized = nullptr; }
        LockedForWrite& operator=(LockedForWrite&& other) { std::swap(m_synchronized, other.m_synchronized); return *this; }

        T* operator -> () const & { pdlAssert(m_synchronized); return &m_synchronized->m_value; }
        T& operator * () const & { pdlAssert(m_synchronized); return m_synchronized->m_value; }
        T* operator -> () const && = delete;
        T& operator * () const && = delete;
        
        ~LockedForWrite() { if (m_synchronized) m_synchronized->m_lock.UnlockWrite(); }
    private:
        SRWSynchronized<T>* m_synchronized;
    };

    template <class T>
    typename SRWSynchronized<T>::LockedForRead SRWSynchronized<T>::LockForRead()
    {
        return LockedForRead(*this);
    }
    template <class T>
    typename SRWSynchronized<T>::LockedForWrite SRWSynchronized<T>::LockForWrite()
    {
        return LockedForWrite(*this);
    }
    
}

