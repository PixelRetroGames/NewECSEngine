#include "../../src/engine/resource_manager.h"
#include <functional>

#include "gtest/gtest.h"

int numLoads = 0;
int numDeletes = 0;

struct Dummy {
  public:

    bool operator==(const Dummy& other) const {
        return x == other.x && y == other.y;
    }

    int x, y;
    Dummy(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Dummy(const Dummy &args) {
        this->x = args.x;
        this->y = args.y;
    }

    Dummy() {
        x = y = 0;
    }

    std::string toString() {
        char buffer[256] = {0};
        sprintf(buffer, "{x = %d, y = %d}", x, y);
        return std::string(buffer);
    }
};

namespace std {
template <>
struct hash<Dummy> {
    size_t operator()(const Dummy &p) const {
        size_t h1 = hash<int>()(p.x);
        size_t h2 = hash<int>()(p.y);
        return h1 ^ (h2 << 1);  // Combine the hash values
    }
};
}

void DummyFreeFn(Dummy *dummy) {
    numDeletes++;
    delete dummy;
}

Dummy *DummyLoadFn(Dummy loadArgs) {
    numLoads++;
    return new Dummy(loadArgs);
}

namespace {

TEST(SharedResourceManagerTest, SameArgsLoadTheSameResourceOnlyOnce) {
    numLoads = 0;
    numDeletes = 0;

    auto dummyManager = Engine::MakeSharedResourceManager(DummyLoadFn, DummyFreeFn);

    {
        const int numDummies = 3;
        Engine::SharedResource<Dummy> dummies[numDummies];

        for (int i = 0; i < numDummies; ++i) {
            dummies[i] = dummyManager->Load(Dummy(10, 20));
            EXPECT_EQ(dummies[i].get(), dummies[0].get());
        }

        EXPECT_EQ(numLoads, 1);

        Engine::SharedResource<Dummy> otherDummy = dummyManager->Load(Dummy(20, 30));
        EXPECT_NE(dummies[0], otherDummy);
        EXPECT_EQ(numLoads, 2);
    }

    // Now that all pointers are freed, all resources should be free
    EXPECT_EQ(numDeletes, 2);

    delete dummyManager;
}

TEST(SharedResourceManagerTest, Dealloc) {
    numLoads = 0;
    numDeletes = 0;

    auto dummyManager = Engine::MakeSharedResourceManager(DummyLoadFn, DummyFreeFn);

    {
        auto dummy1 = dummyManager->Load(Dummy(10, 20));
        EXPECT_EQ(dummyManager->size(), (size_t) 1);

        {
            auto dummy2 = dummyManager->Load(Dummy(10, 20));
            EXPECT_EQ(numLoads, 1);
            EXPECT_EQ(dummyManager->size(), (size_t) 1);
            EXPECT_EQ(dummy1.get(), dummy2.get());
        }
        EXPECT_EQ(dummyManager->size(), (size_t) 1);
        EXPECT_EQ(numDeletes, 0);

        {
            auto dummy2 = dummyManager->Load(Dummy(10, 20));
            EXPECT_EQ(numLoads, 1);
            EXPECT_EQ(dummyManager->size(), (size_t) 1);
            EXPECT_EQ(dummy1.get(), dummy2.get());
        }
        EXPECT_EQ(dummyManager->size(), (size_t) 1);
        EXPECT_EQ(numDeletes, 0);
    }

    EXPECT_EQ(numDeletes, 1);
    EXPECT_EQ(dummyManager->size(), (size_t) 0);

    delete dummyManager;
}

TEST(SharedResourceManagerTest, LoadWithParameterPack) {
    numLoads = 0;
    numDeletes = 0;

    auto dummyManager = Engine::MakeSharedResourceManager(DummyLoadFn, DummyFreeFn);

    {
        auto dummy1 = dummyManager->Load(10, 20);
        EXPECT_EQ(dummyManager->size(), (size_t) 1);

        {
            auto dummy2 = dummyManager->Load(10, 20);
            EXPECT_EQ(numLoads, 1);
            EXPECT_EQ(dummyManager->size(), (size_t) 1);
            EXPECT_EQ(dummy1.get(), dummy2.get());
        }
        EXPECT_EQ(dummyManager->size(), (size_t) 1);
        EXPECT_EQ(numDeletes, 0);
    }

    EXPECT_EQ(numDeletes, 1);
    EXPECT_EQ(dummyManager->size(), (size_t) 0);

    delete dummyManager;
}

TEST(SharedResourceManagerTest, DeleteManagerBeforeResources) {
    numLoads = 0;
    numDeletes = 0;

    auto dummyManager = Engine::MakeSharedResourceManager(DummyLoadFn, DummyFreeFn);

    {
        auto dummy1 = dummyManager->Load(10, 20);
        EXPECT_EQ(dummyManager->size(), (size_t) 1);

        {
            auto dummy2 = dummyManager->Load(10, 20);
            EXPECT_EQ(numLoads, 1);
            EXPECT_EQ(dummyManager->size(), (size_t) 1);
            EXPECT_EQ(dummy1.get(), dummy2.get());
        }
        EXPECT_EQ(dummyManager->size(), (size_t) 1);
        EXPECT_EQ(numDeletes, 0);
        /// TODO
        //delete dummyManager;
        EXPECT_EQ(numDeletes, 0);
    }

    EXPECT_EQ(numDeletes, 1);
    EXPECT_EQ(dummyManager->size(), (size_t) 0);
}

// Unique Resource Manager
TEST(UniqueResourceManagerTest, Trivial) {
    numLoads = 0;
    numDeletes = 0;

    auto dummyManager = Engine::MakeUniqueResourceManager(DummyLoadFn, DummyFreeFn);

    const int numDummies = 3;
    {
        Engine::UniqueResource<Dummy> dummies[numDummies];

        dummies[0] = dummyManager->Load(10, 20);

        for (int i = 1; i < numDummies; ++i) {
            dummies[i] = dummyManager->Load(Dummy(10, 20));
            EXPECT_NE(dummies[i - 1].get(), dummies[i].get());
        }

        EXPECT_EQ(numLoads, numDummies);

        auto otherDummy = dummyManager->Load(Dummy(20, 30));
        EXPECT_NE(dummies[0], otherDummy);
        EXPECT_EQ(numLoads, numDummies + 1);
        EXPECT_EQ(numDeletes, 0);

        delete dummyManager;
        EXPECT_EQ(numDeletes, 0);
    }

    // Now that all pointers are freed, all resources should be free
    EXPECT_EQ(numDeletes, numDummies + 1);
}

}  // namespace
