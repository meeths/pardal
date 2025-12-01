#include <gtest/gtest.h>
#include "Containers/Vector.h"
#include <memory>

using pdl::Vector;

TEST(VectorTest, PushPopAndSBO)
{
    Vector<int, 8> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0u);
    EXPECT_GE(v.capacity(), 8u);
    EXPECT_TRUE(v.using_inline());

    for (int i = 0; i < 8; ++i)
    {
        v.push_back(i * 2);
        EXPECT_EQ(v[i], i * 2);
        EXPECT_EQ(v.size(), static_cast<size_t>(i + 1));
        EXPECT_TRUE(v.using_inline());
    }

    // Exceed inline capacity to force heap allocation
    v.push_back(16);
    EXPECT_FALSE(v.using_inline());
    EXPECT_EQ(v.back(), 16);
    EXPECT_EQ(v.size(), 9u);

    // Pop and check
    v.pop_back();
    EXPECT_EQ(v.size(), 8u);
    EXPECT_EQ(v.back(), 14);
}

TEST(VectorTest, ResizeAndIterate)
{
    Vector<int, 4> v;
    v.resize(3);
    EXPECT_EQ(v.size(), 3u);
    int idx = 0;
    for (auto &x : v)
    {
        x = ++idx; // 1,2,3
    }
    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it) sum += *it;
    EXPECT_EQ(sum, 6);

    v.reserve(10);
    EXPECT_GE(v.capacity(), 10u);
}

struct MoveOnly
{
    std::unique_ptr<int> p;
    MoveOnly() : p(std::make_unique<int>(0)) {}
    explicit MoveOnly(int v) : p(std::make_unique<int>(v)) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

TEST(VectorTest, SupportsMoveOnlyTypes)
{
    Vector<MoveOnly, 2> v;
    v.emplace_back(1);
    v.emplace_back(2);
    // Trigger reallocation and moves
    v.emplace_back(3);
    EXPECT_EQ(*v[0].p, 1);
    EXPECT_EQ(*v[1].p, 2);
    EXPECT_EQ(*v[2].p, 3);

    // Move the vector itself
    Vector<MoveOnly, 2> v2 = std::move(v);
    EXPECT_EQ(*v2[0].p, 1);
    EXPECT_EQ(*v2[1].p, 2);
    EXPECT_EQ(*v2[2].p, 3);
    EXPECT_TRUE(v.empty());
}
