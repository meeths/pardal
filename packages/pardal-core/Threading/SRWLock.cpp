
#include <Threading/SRWLock.h>
#ifdef _WIN32
#include <windows.h>
#endif
// Created on 2023-10-17 by sisco

namespace pdl
{
    SRWLock::SRWLock()
    {
#ifdef _WIN32
        InitializeSRWLock(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    SRWLock::~SRWLock()
    {
    }

    void SRWLock::LockRead()
    {
#ifdef _WIN32
        AcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    void SRWLock::UnlockRead()
    {
#ifdef _WIN32
        ReleaseSRWLockShared(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    void SRWLock::LockWrite()
    {
#ifdef _WIN32
        AcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    void SRWLock::UnlockWrite()
    {
#ifdef _WIN32
        ReleaseSRWLockExclusive(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }
}

