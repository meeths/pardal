
#pragma once
// Created on 2023-12-15 by sisco

#include <EASTL/array.h>

namespace pdl
{
	template <typename T, size_t N>
	using Array = eastl::array<T, N>;
}