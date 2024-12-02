#include "../../src/ecs/entity.h"
#include "../../src/ecs/engine.h"

#include "gtest/gtest.h"

TEST(ECSEntityTest, Id) {
    auto engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 3;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), (ECS::ID)i + 1u);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }
}

TEST(ECSEntityTest, IdReusedTrivial) {
    auto engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 10;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), (ECS::ID)i + 1u);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }

    for (int i = 0; i < numEntities; i++) {
        engine->DeleteEntity(entities[i].GetId());
    }

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ((int) entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }
}

TEST(ECSEntityTest, IdReusedIntertwined) {
    auto engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 10;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ((int) entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }

    for (int i = numEntities - 1; i >= 0; i--) {
        ECS::ID id = entities[i].GetId();

        engine->DeleteEntity(entities[i].GetId());

        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), id);

        for (int j = 0; j < numEntities; j++) {
            if (j == i) {
                continue;
            }

            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }
}
