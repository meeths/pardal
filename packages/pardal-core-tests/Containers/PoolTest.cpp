#include <gtest/gtest.h>
#include "Containers/Pool.h"
#include "Containers/PoolHandle.h"

namespace pdl
{
    namespace Tests
    {
        struct TestObject
        {
            int value = 0;
            bool operator==(const TestObject& other) const = default;
        };

        TEST(PoolHandleTest, Basic)
        {
            PoolHandle<TestObject> handle;
            EXPECT_TRUE(handle.IsEmpty());
            EXPECT_FALSE(handle.IsValid());
            EXPECT_FALSE((bool)handle);

            // Manual creation (internal-ish, but testable via void*)
            void* ptr = reinterpret_cast<void*>((static_cast<ptrdiff_t>(1) << 32) + 10);
            PoolHandle<TestObject> handle2(ptr);
            EXPECT_FALSE(handle2.IsEmpty());
            EXPECT_TRUE(handle2.IsValid());
            EXPECT_EQ(handle2.GetIndex(), 10u);
            EXPECT_EQ(handle2.GetGeneration(), 1u);
            EXPECT_EQ(handle2.GetHandleAsVoid(), ptr);
        }

        TEST(PoolTest, Basic)
        {
            Pool<TestObject, TestObject> pool;
            EXPECT_EQ(pool.GetNumObjects(), 0u);

            PoolHandle<TestObject> h1 = pool.Create(TestObject{ 42 });
            EXPECT_TRUE(h1.IsValid());
            EXPECT_EQ(pool.GetNumObjects(), 1u);

            const TestObject* obj1 = pool.Get(h1);
            ASSERT_NE(obj1, nullptr);
            EXPECT_EQ(obj1->value, 42);

            PoolHandle<TestObject> h2 = pool.Create(TestObject{ 100 });
            EXPECT_EQ(pool.GetNumObjects(), 2u);
            EXPECT_NE(h1, h2);

            pool.Destroy(h1);
            EXPECT_EQ(pool.GetNumObjects(), 1u);
        }

        TEST(PoolTest, ReuseAndGeneration)
        {
            Pool<TestObject, TestObject> pool;
            auto h1 = pool.Create(TestObject{ 1 });
            uint32 index1 = h1.GetIndex();
            uint32 gen1 = h1.GetGeneration();

            pool.Destroy(h1);
            EXPECT_EQ(pool.GetNumObjects(), 0u);

            auto h2 = pool.Create(TestObject{ 2 });
            EXPECT_EQ(pool.GetNumObjects(), 1u);
            EXPECT_EQ(h2.GetIndex(), index1);
            EXPECT_GT(h2.GetGeneration(), gen1);
            EXPECT_NE(h1, h2);

            EXPECT_EQ(pool.Get(h2)->value, 2);
        }

        TEST(PoolTest, FindObject)
        {
            Pool<TestObject, TestObject> pool;
            auto h1 = pool.Create(TestObject{ 10 });
            auto h2 = pool.Create(TestObject{ 20 });

            auto found1 = pool.FindObject(pool.Get(h1));
            EXPECT_EQ(found1, h1);

            auto found2 = pool.FindObject(pool.Get(h2));
            EXPECT_EQ(found2, h2);

            TestObject nonExistent{ 30 };
            EXPECT_TRUE(pool.FindObject(&nonExistent).IsEmpty());
        }

        TEST(PoolTest, Clear)
        {
            Pool<TestObject, TestObject> pool;
            pool.Create(TestObject{ 1 });
            pool.Create(TestObject{ 2 });
            EXPECT_EQ(pool.GetNumObjects(), 2u);

            pool.Clear();
            EXPECT_EQ(pool.GetNumObjects(), 0u);
        }

        TEST(PoolTest, GetPoolHandle)
        {
            Pool<TestObject, TestObject> pool;
            auto h1 = pool.Create(TestObject{ 100 });
            
            auto h1_copy = pool.GetPoolHandle(h1.GetIndex());
            EXPECT_EQ(h1, h1_copy);
        }
    }
}
