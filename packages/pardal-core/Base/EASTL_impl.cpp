
#include <Base/EASTL_impl.h>
#include <Base/DebugHelpers.h>

// Created on 2026-01-21 by franciscom

int Vsnprintf8(char* pDestination, size_t n, const char* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	return _vsnprintf(pDestination, n, pFormat, arguments);
#else
	return vsnprintf(pDestination, n, pFormat, arguments);
#endif
}

int Vsnprintf16(char16_t* pDestination, size_t n, const char16_t* pFormat, va_list arguments)
{
#ifdef _MSC_VER
	static_assert(sizeof(wchar_t) == sizeof(char16_t));
	return _vsnwprintf(reinterpret_cast<wchar_t*>(pDestination), n, reinterpret_cast<const wchar_t*>(pFormat), arguments);
#else
	static_assert(sizeof(wchar_t) == sizeof(char16_t));
	static_assert(0, "Not implemented");
#endif
}

int Vsnprintf32(char32_t* pDestination, size_t n, const char32_t* pFormat, va_list arguments)
{
	pdlNotImplemented();
	return 0;
}

#if EA_CHAR8_UNIQUE
int Vsnprintf8(char8_t* pDestination, size_t n, const char8_t* pFormat, va_list arguments)
{
	return vsnprintf(reinterpret_cast<char*>(pDestination), n, reinterpret_cast<const char*>(pFormat), arguments);
}
#endif