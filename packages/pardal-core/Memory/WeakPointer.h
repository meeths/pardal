#pragma once
// Created on 2023-12-16 by sisco
#include <EASTL/shared_ptr.h>

namespace pdl
{
    template <class T>
    using WeakPointer = eastl::weak_ptr<T>;
}
