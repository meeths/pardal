#include <String/StringHash.h>
#include <gtest/gtest.h>

#include <Math/Interpolator.h>

#include "Math/Vector3.h"

namespace pdl { namespace Tests
{

struct InterpolatorTest : testing::Test
{
};
	
	
	TEST(InterpolatorTest, FloatIsCorrectlyInterpolated )
	{
		Math::Interpolator<float> interpolator1;
		Math::Interpolator<float> interpolatorMinus1;
		Math::Interpolator<float> interpolator2;
		interpolator1.SetOrigin(0);
		interpolator1.SetTarget(1.0f);
		interpolatorMinus1.SetOrigin(0);
		interpolatorMinus1.SetTarget(-1.0f);
		interpolator2.SetOrigin(0);
		interpolator2.SetTarget(2.0f);
		ASSERT_FLOAT_EQ(interpolator1.GetInterpolatedValue(0.5f), 0.5f);
		ASSERT_FLOAT_EQ(interpolator1.GetInterpolatedValue(0.3f), 0.3f);
		ASSERT_FLOAT_EQ(interpolatorMinus1.GetInterpolatedValue(0.5f), -0.5f);
		ASSERT_FLOAT_EQ(interpolatorMinus1.GetInterpolatedValue(0.3f), -0.3f);
		ASSERT_FLOAT_EQ(interpolator2.GetInterpolatedValue(0.5f), 1.0f);
		ASSERT_FLOAT_EQ(interpolator2.GetInterpolatedValue(0.3f), 0.6f);
	}

	TEST(InterpolatorTest, Vector3IsCorrectlyInterpolated )
	{
		Math::Interpolator<Math::Vector3> interpolator1;
		interpolator1.SetOrigin({0,0,0});
		interpolator1.SetTarget({1.0f,2.0f,3.0f});
		ASSERT_FLOAT_EQ(interpolator1.GetInterpolatedValue(0.5f).x, 0.5f);
		ASSERT_FLOAT_EQ(interpolator1.GetInterpolatedValue(0.5f).y, 1.0f);
		ASSERT_FLOAT_EQ(interpolator1.GetInterpolatedValue(0.5f).z, 1.5f);
	}

	TEST(InterpolatorTest, StateSavingInterpolatorKeepsState)
	{
		Math::StateSavingInterpolator<float> interpolator;
		interpolator.SetOrigin(0);
		interpolator.SetTarget(2.0f);
		ASSERT_FLOAT_EQ(interpolator.GetCurrentInterpolatedValue(), 0);
		interpolator.IncrementFactor(0.123f);
		ASSERT_FLOAT_EQ(interpolator.GetCurrentInterpolatedValue(), 0.246f);
		
	}
}}
