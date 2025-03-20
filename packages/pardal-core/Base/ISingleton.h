
#pragma once
#include <Memory/UniquePointer.h>

// Created on 2020-04-30 by sisco
namespace pdl
{
	template<typename T>
    class ISingleton {
	public:
		static T& Instance();

		ISingleton(const ISingleton&) = delete;
		ISingleton& operator= (const ISingleton) = delete;

	protected:
		ISingleton() {}
	};

	template<typename T>
    T& ISingleton<T>::Instance()
	{
		static const UniquePointer<T> instance = MakeUniquePointer<T>();
		return *instance;
	}
	
}
