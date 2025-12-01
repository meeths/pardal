#include <gtest/gtest.h>
#include "Containers/UnorderedMap.h"
#include <string>
#include <vector>

using pdl::UnorderedMap;

TEST(UnorderedMapTest, BasicInsertAndLookup)
{
    UnorderedMap<std::string, int> m;
    EXPECT_TRUE(m.empty());

    m.insert({"a", 1});
    m.emplace("b", 2);
    m["c"] = 3; // operator[] default-inserts then assigns

    EXPECT_EQ(m.size(), 3u);
    EXPECT_FALSE(m.empty());

    // Lookup
    auto it = m.find("b");
    ASSERT_NE(it, m.end());
    EXPECT_EQ(it->second, 2);

#if __cpp_lib_unordered_map_contains
    EXPECT_TRUE(m.contains("a"));
    EXPECT_FALSE(m.contains("zzz"));
#else
    EXPECT_TRUE(m.find("a") != m.end());
    EXPECT_TRUE(m.find("zzz") == m.end());
#endif

    // at
    EXPECT_EQ(m.at("c"), 3);
}

TEST(UnorderedMapTest, EraseReserveRehash)
{
    UnorderedMap<int, int> m;
    for (int i = 0; i < 100; ++i) m[i] = i * 10;
    EXPECT_EQ(m.size(), 100u);

    // erase by key
    size_t removed = m.erase(5);
    EXPECT_EQ(removed, 1u);
    EXPECT_EQ(m.count(5), 0u);

    // reserve & rehash shouldn't change contents
    m.reserve(256);
    m.rehash(512);

    for (int i = 0; i < 100; ++i)
    {
        if (i == 5) continue;
        auto it = m.find(i);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(it->second, i * 10);
    }
}

TEST(UnorderedMapTest, IterationAndSwap)
{
    UnorderedMap<int, int> a;
    UnorderedMap<int, int> b;
    for (int i = 0; i < 10; ++i) a[i] = i + 1;
    for (int i = 0; i < 5; ++i) b[i] = (i + 1) * 100;

    size_t count = 0;
    long long sum = 0;
    for (const auto& kv : a)
    {
        ++count;
        sum += kv.second;
    }
    EXPECT_EQ(count, 10u);
    EXPECT_EQ(sum, 55);

    a.swap(b);
    EXPECT_EQ(a.size(), 5u);
    EXPECT_EQ(b.size(), 10u);
}
