
#include <Base/Base.h>
#include <gtest/gtest.h>
#include "Containers/Vector.h"
#include "Threading/SRWSynchronized.h"

// Created on 2025-03-18 by franciscom

namespace pdl
{
namespace Tests
{
struct SRWSynchronizedTest : testing::Test
{
};

		
	TEST(SRWSynchronizedTest, LocksOperateInTheSameDataAndRespectConst )
	{
		SRWSynchronized<Vector<int>> sync;
		{
			auto lockedSync = sync.LockForWrite();
			lockedSync->push_back(1);
			lockedSync->push_back(2);
			lockedSync->push_back(3);
			lockedSync->push_back(4);
		}
		{
			auto lockedSync = sync.LockForRead();
			ASSERT_EQ(lockedSync->size(), 4);
			ASSERT_EQ(lockedSync->at(0), 1);
			ASSERT_EQ(lockedSync->at(1), 2);
			ASSERT_EQ(lockedSync->at(2), 3);
			ASSERT_EQ(lockedSync->at(3), 4);
		}
	}
}}

