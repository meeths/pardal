
#include <Threading/SRWLock.h>
#ifdef PDL_PLATFORM_WINDOWS
#include <windows.h>
#endif
// Created on 2023-10-17 by sisco

namespace pdl
{
    SRWLock::SRWLock()
    {
#ifdef PDL_PLATFORM_WINDOWS
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
#ifdef PDL_PLATFORM_WINDOWS
        AcquireSRWLockShared(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    void SRWLock::UnlockRead()
    {
#ifdef PDL_PLATFORM_WINDOWS
        ReleaseSRWLockShared(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    void SRWLock::LockWrite()
    {
#ifdef PDL_PLATFORM_WINDOWS
        AcquireSRWLockExclusive(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }

    void SRWLock::UnlockWrite()
    {
#ifdef PDL_PLATFORM_WINDOWS
        ReleaseSRWLockExclusive(reinterpret_cast<PSRWLOCK>(&m_lock));
#else
        static_assert(false, "SRWLock not implemented on this platform");
#endif
    }
}

