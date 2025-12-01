#include <gtest/gtest.h>
#include "Memory/SharedPointer.h"
#include "Memory/WeakPointer.h"

using pdl::SharedPointer;
using pdl::WeakPointer;

namespace {
struct Tracker
{
    static int alive;
    int value = 0;
    explicit Tracker(int v = 0) : value(v) { ++alive; }
    ~Tracker() { --alive; }
};
int Tracker::alive = 0;
}

TEST(WeakPointerTest, BasicLockAndExpire)
{
    EXPECT_EQ(Tracker::alive, 0);
    WeakPointer<Tracker> wp;
    {
        auto sp = pdl::MakeSharedPointer<Tracker>(42);
        EXPECT_EQ(sp.use_count(), 1u);
        EXPECT_EQ(Tracker::alive, 1);

        wp = WeakPointer<Tracker>(sp);
        EXPECT_FALSE(wp.expired());
        EXPECT_EQ(wp.use_count(), 1u);

        // lock should produce owning copy
        auto sp2 = wp.lock();
        ASSERT_TRUE(bool(sp2));
        EXPECT_EQ(sp.use_count(), 2u);
        EXPECT_EQ(sp2->value, 42);
    }
    // sp is out of scope, last strong released; object destroyed
    EXPECT_EQ(Tracker::alive, 0);
    EXPECT_TRUE(wp.expired());
    auto sp3 = wp.lock();
    EXPECT_FALSE(bool(sp3));
}

TEST(WeakPointerTest, CopyMoveSemantics)
{
    auto sp = pdl::MakeSharedPointer<Tracker>(7);
    WeakPointer<Tracker> w1(sp);
    WeakPointer<Tracker> w2 = w1; // copy
    EXPECT_FALSE(w1.expired());
    EXPECT_FALSE(w2.expired());
    EXPECT_EQ(w1.use_count(), 1u);

    WeakPointer<Tracker> w3 = std::move(w2);
    EXPECT_TRUE(w2.lock().get() == nullptr);
    EXPECT_FALSE(w3.expired());

    w1.reset();
    EXPECT_FALSE(w3.expired());
    sp.reset();
    EXPECT_TRUE(w3.expired());
}
