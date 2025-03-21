
#include <Math/Easings.h>
#include <Math/Functions.h>
#include <Base/DebugHelpers.h>
// Created on 2019-02-26 by fmacias

namespace pdl
{
namespace Math
{
namespace Easings
{
float Interpolate(float p, Function _function)
{
	switch (_function)
	{
	case Function::Linear: return LinearInterpolation(p); break;

	case Function::Quadratic_EaseIn: return QuadraticEaseIn(p); break;
	case Function::Quadratic_EaseOut: return QuadraticEaseOut(p); break;
	case Function::Quadratic_EaseInOut: return QuadraticEaseInOut(p); break;

	case Function::Cubic_EaseIn: return CubicEaseIn(p); break;
	case Function::Cubic_EaseOut: return CubicEaseOut(p); break;
	case Function::Cubic_EaseInOut: return CubicEaseInOut(p); break;

	case Function::Quartic_EaseIn: return QuarticEaseIn(p); break;
	case Function::Quartic_EaseOut: return QuarticEaseOut(p); break;
	case Function::Quartic_EaseInOut: return QuarticEaseInOut(p); break;

	case Function::Quintic_EaseIn: return QuinticEaseIn(p); break;
	case Function::Quintic_EaseOut: return QuinticEaseOut(p); break;
	case Function::Quintic_EaseInOut: return QuinticEaseInOut(p); break;

	case Function::Sine_EaseIn: return SineEaseIn(p); break;
	case Function::Sine_EaseOut: return SineEaseOut(p); break;
	case Function::Sine_EaseInOut: return SineEaseInOut(p); break;

	case Function::Circular_EaseIn: return CircularEaseIn(p); break;
	case Function::Circular_EaseOut: return CircularEaseOut(p); break;
	case Function::Circular_EaseInOut: return CircularEaseInOut(p); break;

	case Function::Exponential_EaseIn: return ExponentialEaseIn(p); break;
	case Function::Exponential_EaseOut: return ExponentialEaseOut(p); break;
	case Function::Exponential_EaseInOut: return ExponentialEaseInOut(p); break;

	case Function::Elastic_EaseIn: return ElasticEaseIn(p); break;
	case Function::Elastic_EaseOut: return ElasticEaseOut(p); break;
	case Function::Elastic_EaseInOut: return ElasticEaseInOut(p); break;

	case Function::Back_EaseIn: return BackEaseIn(p); break;
	case Function::Back_EaseOut: return BackEaseOut(p); break;
	case Function::Back_EaseInOut: return BackEaseInOut(p); break;

	case Function::Bounce_EaseIn: return BounceEaseIn(p); break;
	case Function::Bounce_EaseOut: return BounceEaseOut(p); break;
	case Function::Bounce_EaseInOut: return BounceEaseInOut(p); break;

	default: pdlAssert(0); return 0; break;

	}
}
// Modeled after the line y = x
float LinearInterpolation(float p)
{
	return p;
}

// Modeled after the parabola y = x^2
float QuadraticEaseIn(float p)
{
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
float QuadraticEaseOut(float p)
{
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5f)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5f, 1]
float QuadraticEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3
float CubicEaseIn(float p)
{
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
float CubicEaseOut(float p)
{
	float f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5f)
// y = (1/2)((2x-2)^3 + 2) ; [0.5f, 1]
float CubicEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 4 * p * p * p;
	}
	else
	{
		float f = ((2 * p) - 2);
		return 0.5f * f * f * f + 1;
	}
}

// Modeled after the quartic x^4
float QuarticEaseIn(float p)
{
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
float QuarticEaseOut(float p)
{
	float f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5f)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5f, 1]
float QuarticEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		float f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5
float QuinticEaseIn(float p)
{
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
float QuinticEaseOut(float p)
{
	float f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5f)
// y = (1/2)((2x-2)^5 + 2) ; [0.5f, 1]
float QuinticEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		float f = ((2 * p) - 2);
		return  0.5f * f * f * f * f * f + 1;
	}
}

// Modeled after quarter-cycle of sine wave
float SineEaseIn(float p)
{
	return Math::Sin((p - 1) * Math::HalfPi) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
float SineEaseOut(float p)
{
	return Math::Sin(p * Math::HalfPi);
}

// Modeled after half sine wave
float SineEaseInOut(float p)
{
	return 0.5f * (1 - Math::Cos(p * Math::Pi));
}

// Modeled after shifted quadrant IV of unit circle
float CircularEaseIn(float p)
{
	return 1 - Math::Sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
float CircularEaseOut(float p)
{
	return Math::Sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - Math::Sqrt(1 - 4x^2))           ; [0, 0.5f)
// y = (1/2)(Math::Sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5f, 1]
float CircularEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 0.5f * (1 - Math::Sqrt(1 - 4 * (p * p)));
	}
	else
	{
		return 0.5f * (Math::Sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

// Modeled after the exponential function y = 2^(10(x - 1))
float ExponentialEaseIn(float p)
{
	return (p == 0.0f) ? p : pow(2.0f, 10.0f * (p - 1.0f));
}

// Modeled after the exponential function y = -2^(-10x) + 1
float ExponentialEaseOut(float p)
{
	return (p == 1.0f) ? p : 1.0f - pow(2.0f, -10.0f * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5f)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5f,1]
float ExponentialEaseInOut(float p)
{
	if (p == 0.0 || p == 1.0) return p;

	if (p < 0.5f)
	{
		return 0.5f * pow(2.0f, (20.0f * p) - 10.0f);
	}
	else
	{
		return -0.5f * pow(2.0f, (-20.0f * p) + 10.0f) + 1.0f;
	}
}

// Modeled after the damped sine wave y = Math::Sin(13pi/2*x)*pow(2, 10 * (x - 1))
float ElasticEaseIn(float p)
{
	return Math::Sin(13.0f * Math::HalfPi * p) * pow(2.0f, 10.0f * (p - 1.0f));
}

// Modeled after the damped sine wave y = Math::Sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
float ElasticEaseOut(float p)
{
	return Math::Sin(-13.0f * Math::HalfPi * (p + 1.0f)) * pow(2.0f, -10.0f * p) + 1.0f;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*Math::Sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5f)
// y = (1/2)*(Math::Sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5f, 1]
float ElasticEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 0.5f * Math::Sin(13.0f * Math::HalfPi * (2.0f * p)) * pow(2.0f, 10.0f * ((2.0f * p) - 1));
	}
	else
	{
		return 0.5f * (Math::Sin(-13.0f * Math::HalfPi * ((2.0f * p - 1.0f) + 1.0f)) * pow(2.0f, -10.0f * (2.0f * p - 1.0f)) + 2.0f);
	}
}

// Modeled after the overshooting cubic y = x^3-x*Math::Sin(x*pi)
float BackEaseIn(float p)
{
	return p * p * p - p * Math::Sin(p * Math::Pi);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*Math::Sin((1-x)*pi))
float BackEaseOut(float p)
{
	float f = (1 - p);
	return 1 - (f * f * f - f * Math::Sin(f * Math::Pi));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*Math::Sin(2*x*pi))           ; [0, 0.5f)
// y = (1/2)*(1-((1-x)^3-(1-x)*Math::Sin((1-x)*pi))+1) ; [0.5f, 1]
float BackEaseInOut(float p)
{
	if (p < 0.5f)
	{
		float f = 2 * p;
		return 0.5f * (f * f * f - f * Math::Sin(f * Math::Pi));
	}
	else
	{
		float f = (1 - (2 * p - 1));
		return 0.5f * (1 - (f * f * f - f * Math::Sin(f * Math::Pi))) + 0.5f;
	}
}

float BounceEaseIn(float p)
{
	return 1 - BounceEaseOut(1 - p);
}

float BounceEaseOut(float p)
{
	if (p < 4 / 11.0)
	{
		return (121.0f * p * p) / 16.0f;
	}
	else if (p < 8 / 11.0)
	{
		return (363 / 40.0f * p * p) - (99 / 10.0f * p) + 17 / 5.0f;
	}
	else if (p < 9 / 10.0)
	{
		return (4356 / 361.0f * p * p) - (35442 / 1805.0f * p) + 16061 / 1805.0f;
	}
	else
	{
		return (54 / 5.0f * p * p) - (513 / 25.0f * p) + 268 / 25.0f;
	}
}

float BounceEaseInOut(float p)
{
	if (p < 0.5f)
	{
		return 0.5f * BounceEaseIn(p * 2);
	}
	else
	{
		return 0.5f * BounceEaseOut(p * 2 - 1) + 0.5f;
	}
}
}
}

}

