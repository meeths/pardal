
#pragma once

// Created on 2019-02-26 by fmacias

namespace pdl
{
namespace Math
{
namespace Easings
{
	enum class Function
	{
		Linear,

		Quadratic_EaseIn,
		Quadratic_EaseOut,
		Quadratic_EaseInOut,

		Cubic_EaseIn,
		Cubic_EaseOut,
		Cubic_EaseInOut,

		Quartic_EaseIn,
		Quartic_EaseOut,
		Quartic_EaseInOut,

		Quintic_EaseIn,
		Quintic_EaseOut,
		Quintic_EaseInOut,

		Sine_EaseIn,
		Sine_EaseOut,
		Sine_EaseInOut,

		Circular_EaseIn,
		Circular_EaseOut,
		Circular_EaseInOut,

		Exponential_EaseIn,
		Exponential_EaseOut,
		Exponential_EaseInOut,

		Elastic_EaseIn,
		Elastic_EaseOut,
		Elastic_EaseInOut,

		Back_EaseIn,
		Back_EaseOut,
		Back_EaseInOut,

		Bounce_EaseIn,
		Bounce_EaseOut,
		Bounce_EaseInOut
	};

	float Interpolate(float p, Function _function);

	// Linear interpolation (no easing)
	float LinearInterpolation(float p);

	// Quadratic easing; p^2
	float QuadraticEaseIn(float p);
	float QuadraticEaseOut(float p);
	float QuadraticEaseInOut(float p);

	// Cubic easing; p^3
	float CubicEaseIn(float p);
	float CubicEaseOut(float p);
	float CubicEaseInOut(float p);

	// Quartic easing; p^4
	float QuarticEaseIn(float p);
	float QuarticEaseOut(float p);
	float QuarticEaseInOut(float p);

	// Quintic easing; p^5
	float QuinticEaseIn(float p);
	float QuinticEaseOut(float p);
	float QuinticEaseInOut(float p);

	// Sine wave easing; sin(p * PI/2)
	float SineEaseIn(float p);
	float SineEaseOut(float p);
	float SineEaseInOut(float p);

	// Circular easing; sqrt(1 - p^2)
	float CircularEaseIn(float p);
	float CircularEaseOut(float p);
	float CircularEaseInOut(float p);

	// Exponential easing, base 2
	float ExponentialEaseIn(float p);
	float ExponentialEaseOut(float p);
	float ExponentialEaseInOut(float p);

	// Exponentially-damped sine wave easing
	float ElasticEaseIn(float p);
	float ElasticEaseOut(float p);
	float ElasticEaseInOut(float p);

	// Overshooting cubic easing; 
	float BackEaseIn(float p);
	float BackEaseOut(float p);
	float BackEaseInOut(float p);

	// Exponentially-decaying bounce easing
	float BounceEaseIn(float p);
	float BounceEaseOut(float p);
	float BounceEaseInOut(float p);
};
}

}

