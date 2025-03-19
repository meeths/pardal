
#pragma once
#include <mutex>
#include "Base/BaseDefines.h"

// Created on 2025-03-19 by sisco

namespace pdl
{

class Mutex
{
public:
    Mutex() = default;
    ~Mutex() = default;
    DeclareNonCopyable(Mutex);
    
    void Lock() { m_mutex.lock(); }
    void Unlock() { m_mutex.unlock(); }
    bool TryLock() { return m_mutex.try_lock(); }

    // Named requirement
    void lock() { Lock(); }
    void unlock() { Unlock(); }
    bool try_lock() { return TryLock(); }
private:
    std::mutex m_mutex;
};

}

