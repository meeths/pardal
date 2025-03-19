
#pragma once

#include <cstdint>
#include "Base/BaseDefines.h"

// Created on 2023-10-17 by sisco

namespace pdl
{

class SRWLock
{
public:
    SRWLock();
    ~SRWLock();
    DeclareNonCopyable(SRWLock);
    DeclareNonMoveable(SRWLock);

    void LockRead();
    void UnlockRead();
    
    void LockWrite();
    void UnlockWrite();

private:
    uint64_t m_lock;
};

class SRWScopedReadLock
{
public:
    SRWScopedReadLock(SRWLock& lock) : m_lock(lock){ m_lock.LockRead(); }
    ~SRWScopedReadLock() { m_lock.UnlockRead(); }
    DeclareNonCopyable(SRWScopedReadLock);
    DeclareNonMoveable(SRWScopedReadLock);
private:
    SRWLock& m_lock;
};
    
class SRWScopedWriteLock
{
public:
    SRWScopedWriteLock(SRWLock& lock) : m_lock(lock){ m_lock.LockWrite(); }
    ~SRWScopedWriteLock() { m_lock.UnlockWrite(); }
    DeclareNonCopyable(SRWScopedWriteLock);
    DeclareNonMoveable(SRWScopedWriteLock);
private:
    SRWLock& m_lock;
};
    
    
}

