#include <gtest/gtest.h>
#include "Containers/Array.h"
#include <tuple>

using pdl::Array;

TEST(ArrayTest, BraceInitAndAccess)
{
    Array<int, 4> a{1, 2, 3, 4};
    EXPECT_EQ(a.size(), 4u);
    EXPECT_FALSE(a.empty());
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a.at(1), 2);
    EXPECT_EQ(a.front(), 1);
    EXPECT_EQ(a.back(), 4);
}

TEST(ArrayTest, IterateAndSum)
{
    Array<int, 5> a{1, 2, 3, 4, 5};
    int sum = 0;
    for (auto v : a) sum += v;
    EXPECT_EQ(sum, 15);

    int sum2 = 0;
    for (auto it = a.begin(); it != a.end(); ++it) sum2 += *it;
    EXPECT_EQ(sum2, 15);
}

TEST(ArrayTest, FillAndSwap)
{
    Array<int, 3> a{0,0,0};
    a.fill(7);
    EXPECT_EQ(a[0], 7);
    EXPECT_EQ(a[1], 7);
    EXPECT_EQ(a[2], 7);

    Array<int, 3> b{1,2,3};
    a.swap(b);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
}

TEST(ArrayTest, Comparisons)
{
    Array<int, 3> a{1,2,3};
    Array<int, 3> b{1,2,3};
    Array<int, 3> c{1,2,4};
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a < c);
    EXPECT_TRUE(c > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(c >= b);
}

TEST(ArrayTest, StructuredBindingsAndGet)
{
    Array<int, 3> a{9,8,7};
    auto [x,y,z] = a; // structured binding copies
    EXPECT_EQ(x, 9);
    EXPECT_EQ(y, 8);
    EXPECT_EQ(z, 7);

    // std::get by index
    EXPECT_EQ(std::get<0>(a), 9);
    EXPECT_EQ(std::get<1>(a), 8);
    EXPECT_EQ(std::get<2>(a), 7);
}
