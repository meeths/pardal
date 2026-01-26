
#pragma once
#include "Base/BaseDefines.h"
#include "Base/DebugHelpers.h"
#include "Memory/SharedPointer.h"
#include "Memory/WeakPointer.h"

// Created on 2026-01-23 by sisco

namespace pdl
{

template <typename T>
class ServiceLocator
{
public:
    pdlNoDiscard static WeakPointer<T> Get()
    {
        return m_service;
    }
    
    pdlNoDiscard static T& Ref()
    {
        pdlAssert(m_service != nullptr && "ServiceLocator::Ref() called before ServiceLocator::Create()");
        return *m_service;
    }
    
    template<typename... Args>
    static void Create(Args && ...args)
    {
        m_service = MakeSharedPointer<T>(std::forward<Args>(args)...);
    }

    static void Destroy()
    {
        m_service = nullptr;
    }

private:
    inline static SharedPointer<T> m_service;
};

}

