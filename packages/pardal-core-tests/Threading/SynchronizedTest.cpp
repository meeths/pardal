
#include <gtest/gtest.h>
#include "Containers/Vector.h"
#include "Threading/Synchronized.h"

// Created on 2025-03-18 by franciscom

namespace pdl
{
namespace Tests
{
struct SynchronizedTest : testing::Test
{
};

	TEST(SynchronizedTest, LocksOperateInTheSameData)
	{
		Synchronized<Vector<int>> sync;
		{
			auto lockedSync = sync.Lock();
			lockedSync->push_back(1);
			lockedSync->push_back(2);
			lockedSync->push_back(3);
			lockedSync->push_back(4);
			lockedSync->push_back(5);
		}
		{
			auto lockedSync = sync.Lock();
			ASSERT_EQ(lockedSync->size(), 5);
			ASSERT_EQ(lockedSync->at(0), 1);
			ASSERT_EQ(lockedSync->at(1), 2);
			ASSERT_EQ(lockedSync->at(2), 3);
			ASSERT_EQ(lockedSync->at(3), 4);
			ASSERT_EQ(lockedSync->at(4), 5);
		}
	}
}}

