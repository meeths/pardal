#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "Handle/Handle.h"

using pdl::Handle;
using pdl::WeakHandle;

namespace {
struct Tracker
{
    static std::atomic<int> alive;
    int id;
    explicit Tracker(int v = 0) : id(v) { alive.fetch_add(1, std::memory_order_relaxed); }
    ~Tracker() { alive.fetch_sub(1, std::memory_order_relaxed); }
};
std::atomic<int> Tracker::alive{0};
}

TEST(HandleTest, MakeHandleAndBasicAccess)
{
    auto h = pdl::make_handle<Tracker>(42);
    ASSERT_TRUE(h.valid());
    EXPECT_EQ(h->id, 42);
    EXPECT_EQ(h.use_count(), 1u);
    EXPECT_EQ(Tracker::alive.load(), 1);
}

TEST(HandleTest, CopyMoveAndWeak)
{
    EXPECT_EQ(Tracker::alive.load(), 0);
    auto h1 = pdl::make_handle<Tracker>(7);
    EXPECT_EQ(Tracker::alive.load(), 1);
    EXPECT_EQ(h1.use_count(), 1u);

    Handle<Tracker> h2 = h1; // copy
    EXPECT_EQ(h1.use_count(), 2u);
    EXPECT_EQ(h2.use_count(), 2u);

    WeakHandle<Tracker> w = h1.weak();
    EXPECT_FALSE(w.expired());
    EXPECT_EQ(w.use_count(), 2u);

    Handle<Tracker> h3 = std::move(h2);
    EXPECT_TRUE(bool(h3));
    EXPECT_FALSE(bool(h2));
    EXPECT_EQ(h1.use_count(), 2u);

    auto h4 = w.lock();
    EXPECT_TRUE(bool(h4));
    EXPECT_EQ(h4.use_count(), 3u);

    h1.reset();
    h3.reset();
    h4.reset();
    EXPECT_TRUE(w.expired());
    EXPECT_EQ(Tracker::alive.load(), 0);
}

namespace { static std::atomic<int> g_countDtor{0}; struct CountDtor { ~CountDtor(){ g_countDtor.fetch_add(1, std::memory_order_relaxed); } }; }
TEST(HandleTest, DestructorCalledOnce)
{
    g_countDtor.store(0);
    {
        auto h = pdl::make_handle<CountDtor>();
        Handle<CountDtor> h2 = h;
        EXPECT_EQ(h.use_count(), 2u);
    }
    // both handles out of scope -> exactly one destruction
    EXPECT_EQ(g_countDtor.load(), 1);
}

TEST(HandleTest, MultithreadedRefcountChurn)
{
    auto h = pdl::make_handle<Tracker>(1);
    constexpr int kThreads = 8;
    constexpr int kIters = 1000;
    std::vector<std::thread> ts;
    ts.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t)
    {
        ts.emplace_back([h]() mutable {
            for (int i = 0; i < kIters; ++i)
            {
                // copy then move then reset
                Handle<Tracker> a = h;
                Handle<Tracker> b = a;
                Handle<Tracker> c = std::move(b);
                EXPECT_TRUE(bool(c));
                if ((i & 3) == 0)
                {
                    auto w = c.weak();
                    auto lk = w.lock();
                    if (lk)
                    {
                        EXPECT_GE(lk.use_count(), 1u);
                    }
                }
                a.reset();
                c.reset();
            }
        });
    }
    for (auto& th : ts) th.join();

    // original h still alive here
    EXPECT_EQ(Tracker::alive.load(), 1);
    h.reset();
    EXPECT_EQ(Tracker::alive.load(), 0);
}
