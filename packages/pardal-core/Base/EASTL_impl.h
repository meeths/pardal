
#pragma once
#ifdef PDL_USING_EASTD
// Created on 2026-01-21 by franciscom
// Implement the functions that EASTL expects 
#define _CRT_SECURE_NO_WARNINGS 1
#include <EASTL/string.h>

int Vsnprintf8 (char*  pDestination, size_t n, const char*  pFormat, va_list arguments);
int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments);
int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments);
#if EA_CHAR8_UNIQUE
int Vsnprintf8 (char8_t*  pDestination, size_t n, const char8_t*  pFormat, va_list arguments);
#endif

#undef _CRT_SECURE_NO_WARNINGS
#endif