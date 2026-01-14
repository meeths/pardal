
#pragma once
#include <EASTL/unordered_map.h>

// Created on 2023-12-15 by sisco

namespace pdl
{
	template <typename K, typename T>
	using UnorderedMap = eastl::unordered_map<K, T>;
	
	
	template<typename From, typename To, typename Func>
	auto ReverseMap(Func func, From min, From max, From defaultValue = From(0))
	{
		static UnorderedMap<To, From> reverseMap = [&]()
		{
			UnorderedMap<To, From> map;
			for (int i = int(min); i <= int(max); i++)
			{
				map[func(From(i))] = From(i);
			}
			return map;
		}();
		return [defaultValue](To value) -> From
		{
			auto it = reverseMap.find(value);
			return it == reverseMap.end() ? defaultValue : it->second;
		};
	}

}
              
