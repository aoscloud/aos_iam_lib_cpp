/*
 * Copyright (C) 2023 Renesas Electronics Corporation.
 * Copyright (C) 2023 EPAM Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include "aos/common/tools/memory.hpp"

using namespace aos;

static void OwnUniquePtr(UniquePtr<uint32_t> uPtr)
{
    EXPECT_TRUE(uPtr);
}

static void TakeSharedPtr(SharedPtr<uint32_t> shPtr)
{
    EXPECT_TRUE(shPtr);
}

TEST(MemoryTest, UniquePtr)
{
    StaticAllocator<256> allocator;

    // Basic test

    {
        UniquePtr<uint32_t> uPtr = MakeUnique<uint32_t>(&allocator, 0);
        EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(uint32_t));
    }

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());

    // Move ownership

    UniquePtr<uint32_t> uPtr;

    EXPECT_FALSE(uPtr);
    EXPECT_TRUE(uPtr == nullptr);
    EXPECT_TRUE(nullptr == uPtr);

    {
        uPtr = MakeUnique<uint32_t>(&allocator);
    }

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(uint32_t));

    OwnUniquePtr(Move(uPtr));

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());

    // Make unique

    auto uPtr2 = MakeUnique<uint32_t>(&allocator);

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(uint32_t));

    // Check reset

    uPtr2.Reset();

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());
}

TEST(MemoryTest, SharedPtr)
{
    StaticAllocator<256> allocator;

    // Basic test

    {
        SharedPtr<uint32_t> shPtr(&allocator, new (&allocator) uint32_t());
        EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(uint32_t));
    }

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());

    // Test share

    {
        SharedPtr<uint32_t> shPtr;

        EXPECT_FALSE(shPtr);
        EXPECT_TRUE(shPtr == nullptr);
        EXPECT_TRUE(nullptr == shPtr);

        {
            shPtr = SharedPtr<uint32_t>(&allocator, new (&allocator) uint32_t());
        }

        EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(uint32_t));

        TakeSharedPtr(shPtr);
    }

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());

    // Make shared

    auto shPtr2 = MakeShared<uint32_t>(&allocator);

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(uint32_t));

    // Check reset

    shPtr2.Reset();

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());
}

TEST(MemoryTest, SharedPtrDerivedClass)
{
    class BaseClass {
    public:
        BaseClass() {};
        virtual ~BaseClass() {};
    };

    class NewClass : public BaseClass {
    public:
        NewClass() {};
        virtual ~NewClass() {};
    };

    StaticAllocator<256> allocator;

    {
        SharedPtr<BaseClass> basePtr;

        {
            auto newPtr = MakeShared<NewClass>(&allocator);

            EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(NewClass));

            basePtr = newPtr;
        }

        EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize() - sizeof(NewClass));
    }

    EXPECT_EQ(allocator.FreeSize(), allocator.MaxSize());
}
