#include <gtest/gtest.h>
#include "Containers/CircularVector.h"

namespace pdl
{
    namespace Tests
    {
        TEST(CircularVectorTest, BasicPush)
        {
            CircularVector<int> sv(5);
            EXPECT_EQ(sv.GetSamples().size(), 0u);
            EXPECT_EQ(sv.GetOffset(), 0);

            sv.Push(1);
            sv.Push(2);
            sv.Push(3);

            EXPECT_EQ(sv.GetSamples().size(), 3u);
            EXPECT_EQ(sv.GetOffset(), 0);
            EXPECT_EQ(sv.GetSamples()[0], 1);
            EXPECT_EQ(sv.GetSamples()[1], 2);
            EXPECT_EQ(sv.GetSamples()[2], 3);
        }

        TEST(CircularVectorTest, ScrollingPush)
        {
            CircularVector<int> sv(3);
            sv.Push(1);
            sv.Push(2);
            sv.Push(3);

            EXPECT_EQ(sv.GetSamples().size(), 3u);
            EXPECT_EQ(sv.GetOffset(), 0);

            // This should overwrite index 0
            sv.Push(4);
            EXPECT_EQ(sv.GetSamples().size(), 3u);
            EXPECT_EQ(sv.GetOffset(), 1);
            EXPECT_EQ(sv.GetSamples()[0], 4);
            EXPECT_EQ(sv.GetSamples()[1], 2);
            EXPECT_EQ(sv.GetSamples()[2], 3);

            // This should overwrite index 1
            sv.Push(5);
            EXPECT_EQ(sv.GetOffset(), 2);
            EXPECT_EQ(sv.GetSamples()[1], 5);

            // This should overwrite index 2
            sv.Push(6);
            EXPECT_EQ(sv.GetOffset(), 0);
            EXPECT_EQ(sv.GetSamples()[2], 6);
        }

        TEST(CircularVectorTest, Erase)
        {
            CircularVector<int> sv(5);
            sv.Push(1);
            sv.Push(2);
            sv.Erase();

            EXPECT_EQ(sv.GetSamples().size(), 0u);
            EXPECT_EQ(sv.GetOffset(), 0);

            // Push again after erase
            sv.Push(10);
            EXPECT_EQ(sv.GetSamples().size(), 1u);
            EXPECT_EQ(sv.GetSamples()[0], 10);
        }

        TEST(CircularVectorTest, DefaultMaxSize)
        {
            CircularVector<int> sv; // Default is 100
            for (int i = 0; i < 150; ++i)
            {
                sv.Push(i);
            }
            EXPECT_EQ(sv.GetSamples().size(), 100u);
            EXPECT_EQ(sv.GetOffset(), 50); // 150 % 100
        }
    }
}
