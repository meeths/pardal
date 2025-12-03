#include <gtest/gtest.h>
#include "Memory/UniquePointer.h"

using pdl::UniquePointer;

namespace {
struct Tracker
{
    static int alive;
    int value = 0;
    Tracker() { ++alive; }
    explicit Tracker(int v) : value(v) { ++alive; }
    ~Tracker() { --alive; }
};
int Tracker::alive = 0;

struct CountedDeleter
{
    int* counter = nullptr;
    void operator()(Tracker* p) const noexcept
    {
        if (counter) ++*counter;
        delete p;
    }
};
}
namespace pdl
{
    namespace Tests
    {
        TEST(UniquePointerTest, MakeUniqueAndBasicAccess)
        {
            EXPECT_EQ(Tracker::alive, 0);
            auto up = pdl::MakeUniquePointer<Tracker>(7);
            ASSERT_TRUE(bool(up));
            EXPECT_EQ(up->value, 7);
            up->value = 11;
            EXPECT_EQ((*up).value, 11);
            EXPECT_EQ(Tracker::alive, 1);
        }

        TEST(UniquePointerTest, MoveResetReleaseSwap)
        {
            EXPECT_EQ(Tracker::alive, 0);
            UniquePointer<Tracker> a = pdl::MakeUniquePointer<Tracker>(1);
            EXPECT_EQ(Tracker::alive, 1);

            UniquePointer<Tracker> b = std::move(a);
            EXPECT_FALSE(bool(a));
            ASSERT_TRUE(bool(b));
            EXPECT_EQ(b->value, 1);
            EXPECT_EQ(Tracker::alive, 1);

            // release prevents deletion
            Tracker* raw = b.Release();
            EXPECT_FALSE(bool(b));
            ASSERT_NE(raw, nullptr);
            EXPECT_EQ(Tracker::alive, 1);

            // adopt and reset
            b.Reset(raw);
            EXPECT_TRUE(bool(b));
            b.Reset();
            EXPECT_EQ(Tracker::alive, 0);

            // swap
            UniquePointer<Tracker> c = pdl::MakeUniquePointer<Tracker>(42);
            UniquePointer<Tracker> d;
            c.Swap(d);
            EXPECT_FALSE(bool(c));
            ASSERT_TRUE(bool(d));
            EXPECT_EQ(d->value, 42);
        }

        TEST(UniquePointerTest, CustomDeleter)
        {
            int deletes = 0;
            {
                UniquePointer<Tracker, CountedDeleter> p(new Tracker(5));
                p.GetDeleter().counter = &deletes;
                EXPECT_TRUE(bool(p));
                EXPECT_EQ(Tracker::alive, 1);
            }
            EXPECT_EQ(deletes, 1);
            EXPECT_EQ(Tracker::alive, 0);
        }

        TEST(UniquePointerTest, ArrayOverload)
        {
            EXPECT_EQ(Tracker::alive, 0);
            {
                auto arr = pdl::MakeUniquePointer<Tracker[]>(4);
                ASSERT_TRUE(bool(arr));
                // 4 default-constructed
                EXPECT_EQ(Tracker::alive, 4);
                arr[0].value = 1; arr[1].value = 2; arr[2].value = 3; arr[3].value = 4;
                EXPECT_EQ(arr[2].value, 3);
            }
            EXPECT_EQ(Tracker::alive, 0);
        }
    }
}