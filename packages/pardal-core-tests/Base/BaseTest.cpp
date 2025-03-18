
#include <Base/Base.h>
#include <gtest/gtest.h>

// Created on 2025-03-18 by franciscom

namespace pdl
{
namespace Tests
{
struct BaseTest : testing::Test
{
};


		
	TEST(BaseTest, LinkedAgainstCorrectLib )
	{
		ASSERT_TRUE(Base::ReturnTrue());
	}
}}

