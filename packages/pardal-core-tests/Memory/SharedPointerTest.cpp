#include <gtest/gtest.h>
#include "Memory/SharedPointer.h"

using pdl::SharedPointer;

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
namespace pdl
{
    namespace Tests
    {
        TEST(SharedPointerTest, MakeSharedAndBasicAccess)
        {
            auto sp = pdl::MakeSharedPointer<Tracker>(7);
            ASSERT_TRUE(bool(sp));
            EXPECT_EQ(sp->value, 7);
            sp->value = 9;
            EXPECT_EQ((*sp).value, 9);
            EXPECT_EQ(sp.use_count(), 1u);
            EXPECT_EQ(Tracker::alive, 1);
        }

        TEST(SharedPointerTest, CopyAndMoveSemantics)
        {
            auto sp1 = pdl::MakeSharedPointer<Tracker>(1);
            EXPECT_EQ(sp1.use_count(), 1u);

            SharedPointer<Tracker> sp2 = sp1; // copy
            EXPECT_EQ(sp1.use_count(), 2u);
            EXPECT_EQ(sp2.use_count(), 2u);
            EXPECT_EQ(Tracker::alive, 1);

            SharedPointer<Tracker> sp3 = std::move(sp2); // move
            EXPECT_EQ(sp1.use_count(), 2u);
            EXPECT_EQ(sp3.use_count(), 2u);
            EXPECT_FALSE(bool(sp2));
        }

        TEST(SharedPointerTest, ResetAndDestroy)
        {
            EXPECT_EQ(Tracker::alive, 0);
            SharedPointer<Tracker> sp;
            {
                sp = pdl::MakeSharedPointer<Tracker>(5);
                EXPECT_EQ(Tracker::alive, 1);
                EXPECT_EQ(sp.use_count(), 1u);
                {
                    SharedPointer<Tracker> sp2 = sp;
                    EXPECT_EQ(sp.use_count(), 2u);
                    EXPECT_EQ(Tracker::alive, 1);
                    sp2.reset();
                    EXPECT_EQ(sp.use_count(), 1u);
                    EXPECT_EQ(Tracker::alive, 1);
                }
                EXPECT_EQ(Tracker::alive, 1);
            }
            // sp still holds
            EXPECT_EQ(Tracker::alive, 1);
            sp.reset();
            EXPECT_EQ(Tracker::alive, 0);
        }

        TEST(SharedPointerTest, ConstructorWithPointer)
        {
            // Start clean
            EXPECT_EQ(Tracker::alive, 0);

            // Construct from raw pointer and verify adoption of ownership
            Tracker* raw = new Tracker(42);
            EXPECT_EQ(Tracker::alive, 1);
            {
                SharedPointer<Tracker> sp(raw);
                EXPECT_TRUE(bool(sp));
                EXPECT_EQ(sp.get(), raw);
                EXPECT_EQ(sp->value, 42);
                EXPECT_EQ(sp.use_count(), 1u);

                // Copy to ensure control block is set up correctly
                {
                    SharedPointer<Tracker> sp2 = sp;
                    EXPECT_EQ(sp.use_count(), 2u);
                    EXPECT_EQ(sp2.use_count(), 2u);
                    EXPECT_EQ(sp2.get(), raw);
                    EXPECT_EQ(Tracker::alive, 1);
                }

                EXPECT_EQ(sp.use_count(), 1u);
            }

            // Upon leaving scope, the object should be destroyed
            EXPECT_EQ(Tracker::alive, 0);

            // Also verify constructing with nullptr yields empty shared pointer
            SharedPointer<Tracker> sp_null(nullptr);
            EXPECT_FALSE(bool(sp_null));
            EXPECT_EQ(sp_null.use_count(), 0u);
        }
    }
}