
#pragma once

// Created on 2023-12-16 by sisco
#include <EASTL/shared_ptr.h>

namespace pdl
{
    template <class T>
    using SharedPointer = eastl::shared_ptr<T>;
    
    template <typename T, typename... Args>
    SharedPointer<T> MakeSharedPointer(Args&&... args)
    {
        return eastl::make_shared<T>(eastl::forward<Args>(args)...);
    }
}
