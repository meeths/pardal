
#pragma once

// Created on 2023-12-16 by sisco
#include <EASTL/unique_ptr.h>

namespace pdl
{
    template <class T>
    using UniquePointer = eastl::unique_ptr<T>;
    
    template <typename T, typename... Args>
    eastl::enable_if<!eastl::is_array<T>::value, eastl::unique_ptr<T>>::type MakeUniquePointer(Args&&... args)
    {
        return eastl::make_unique<T>(eastl::forward<Args>(args)...);
    }

    template <typename T>
    eastl::enable_if<eastl::is_unbounded_array<T>::value, eastl::unique_ptr<T>>::type MakeUniquePointer(size_t n)
    {
        return eastl::make_unique<T>(n);
    }
}
