
#pragma once
#include "BaseDefines.h"

// Created on 2026-02-02 by franciscom

namespace pdl
{

template<typename T>
struct AtScopeExit
{
	T lambda;
	AtScopeExit(T lambda):lambda(lambda){}
	~AtScopeExit()
	{
		lambda();
	}

	DeclareNonCopyable(AtScopeExit);
	
private:
	AtScopeExit& operator =(const AtScopeExit&);
};

class ScopeHelper
{
public:
	template<typename T>
	AtScopeExit<T> operator+(T t)
	{
		return t;
	}
};

#define pdlAtScopeExit const auto& CONCAT(AtScopeExit__,__LINE__) = pdl::ScopeHelper() + [&]()

}

