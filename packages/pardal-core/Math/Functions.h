#pragma once
#include <cmath>

namespace pdl { namespace Math
{
	template <typename T> T Abs(T val) { return std::abs(val); };
	template <typename T> T Sqrt(T val) { return std::sqrt(val); };
	template <typename T> T InvSqrt(T _fVal) { return 1.0f / Sqrt(_fVal); };

	template <typename T> T Pow2(T val) { return val * val; };
	template <typename T> T Pow(T base, T exp) { return std::pow(base, exp); };

	template <typename T> T Cos(T val) { return std::cos(val); };
	template <typename T> T Sin(T val) { return std::sin(val); };
	template <typename T> T Tan(T val) { return std::tan(val); };

    template <typename T> T ASin(T val) { return std::asin(val); };
    template <typename T> T ACos(T val) { return std::acos(val); };
    template <typename T> T ATan(T val) { return std::atan(val); };
    template <typename T> T ATan2(T val, T val2) { return std::atan2(val, val2); };
    template <typename T> T Sign(T val) { return std::signbit(val) ? -1.0f : 1.0f; };

	template <typename T> T Min(const T& a, const T& b) { return a < b ? a : b; };
	template <typename T> T Max(const T& a, const T& b) { return a > b ? a : b; };

	template <typename T> T Floor(const T& a) { return static_cast<T>(std::floor(a)); };

	template <typename T> T Log2(const T& a) { return static_cast<T>(std::log2(a)); };

	inline int NextPowerOf2(int val)
	{
		return static_cast<int>(pow(2, static_cast<int>(ceil(log(val) / log(2)))));
	}

	static constexpr float Pi{ 3.14159265358979f };
	static constexpr float HalfPi{ 3.14159265358979f / 2.0f };
	static constexpr float EpsilonBig{ 0.0001f };
	static constexpr float EpsilonMedium{ 0.000001f };
	static constexpr float EpsilonSmall{ 0.000000001f };

	static constexpr float Rad2Deg = (180.f / Pi);
	static constexpr float Deg2Rad = (Pi / 180.f);
}}
