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
namespace pdl
{
    namespace Tests
    {
        TEST(HandleTest, MakeHandleAndBasicAccess)
        {
            auto h = pdl::MakeHandle<Tracker>(42);
            ASSERT_TRUE(h.IsValid());
            EXPECT_EQ(h->id, 42);
            EXPECT_EQ(h.UseCount(), 1u);
            EXPECT_EQ(Tracker::alive.load(), 1);
        }

        TEST(HandleTest, CopyMoveAndWeak)
        {
            EXPECT_EQ(Tracker::alive.load(), 0);
            auto h1 = pdl::MakeHandle<Tracker>(7);
            EXPECT_EQ(Tracker::alive.load(), 1);
            EXPECT_EQ(h1.UseCount(), 1u);

            Handle<Tracker> h2 = h1; // copy
            EXPECT_EQ(h1.UseCount(), 2u);
            EXPECT_EQ(h2.UseCount(), 2u);

            WeakHandle<Tracker> w = h1.Weak();
            EXPECT_FALSE(w.IsExpired());
            EXPECT_EQ(w.UseCount(), 2u);

            Handle<Tracker> h3 = std::move(h2);
            EXPECT_TRUE(bool(h3));
            EXPECT_FALSE(bool(h2));
            EXPECT_EQ(h1.UseCount(), 2u);

            auto h4 = w.Lock();
            EXPECT_TRUE(bool(h4));
            EXPECT_EQ(h4.UseCount(), 3u);

            h1.Reset();
            h3.Reset();
            h4.Reset();
            EXPECT_TRUE(w.IsExpired());
            EXPECT_EQ(Tracker::alive.load(), 0);
        }

        namespace { static std::atomic<int> g_countDtor{0}; struct CountDtor { ~CountDtor(){ g_countDtor.fetch_add(1, std::memory_order_relaxed); } }; }
        TEST(HandleTest, DestructorCalledOnce)
        {
            g_countDtor.store(0);
            {
                auto h = pdl::MakeHandle<CountDtor>();
                Handle<CountDtor> h2 = h;
                EXPECT_EQ(h.UseCount(), 2u);
            }
            // both handles out of scope -> exactly one destruction
            EXPECT_EQ(g_countDtor.load(), 1);
        }

        TEST(HandleTest, MultithreadedRefcountChurn)
        {
            auto h = pdl::MakeHandle<Tracker>(1);
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
                            auto w = c.Weak();
                            auto lk = w.Lock();
                            if (lk)
                            {
                                EXPECT_GE(lk.UseCount(), 1u);
                            }
                        }
                        a.Reset();
                        c.Reset();
                    }
                });
            }
            for (auto& th : ts) th.join();

            // original h still alive here
            EXPECT_EQ(Tracker::alive.load(), 1);
            h.Reset();
            EXPECT_EQ(Tracker::alive.load(), 0);
        }
    }
}