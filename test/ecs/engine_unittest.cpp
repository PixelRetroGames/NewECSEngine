#include "gtest/gtest.h"

#include "../../src/ecs/engine.h"

TEST(ECSEngineTest, AddComponent) {
    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 3 * 3;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ((int) entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i], entities[j]);
        }
    }

    std::vector<ECS::Entity> expectedInt;
    std::vector<ECS::Entity> expectedFloat;
    std::vector<ECS::Entity> expectedIntAndFloat;

    {
        int i = 0;

        for (; i < numEntities / 3; i++) {
            engine->AddComponent(entities[i], i);
            expectedInt.push_back(entities[i]);
        }

        for (; i < 2 * numEntities / 3; i++) {
            engine->AddComponent(entities[i], (1.0f * i));
            expectedFloat.push_back(entities[i]);
        }

        for (; i < numEntities; i++) {
            engine->AddComponent(entities[i], (i));
            engine->AddComponent(entities[i], (1.0f * i));
            expectedIntAndFloat.push_back(entities[i].GetId());
            expectedInt.push_back(entities[i]);
            expectedFloat.push_back(entities[i]);
        }
    }

    auto matchingInt = engine->GetEntities(ECS::CreateMask<int>());
    std::sort(matchingInt.begin(), matchingInt.end());
    EXPECT_EQ(matchingInt, expectedInt);

    auto matchingFloat = engine->GetEntities(ECS::CreateMask<float>());
    std::sort(matchingFloat.begin(), matchingFloat.end());
    EXPECT_EQ(matchingFloat, expectedFloat);

    auto matchingIntAndFloat = engine->GetEntities(
                                   ECS::CreateMask<int, float>());
    std::sort(matchingIntAndFloat.begin(), matchingIntAndFloat.end());
    EXPECT_EQ(matchingIntAndFloat, expectedIntAndFloat);
}

TEST(ECSEngineTest, GetComponentEmpty) {
    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());
    auto matchingInt = engine->GetEntities(ECS::CreateMask<int>());
    EXPECT_EQ(matchingInt.size(), (size_t) 0);
}

/*
TEST(ECSEngineTest, EntitiesCallFor) {
    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 3 * 3;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }

    {
        int i = 0;

        for (; i < numEntities / 3; i++) {
            engine->AddComponent(entities[i], (i));
        }

        for (; i < 2 * numEntities / 3; i++) {
            engine->AddComponent(entities[i], (1.0f * i));
        }

        for (; i < numEntities; i++) {
            engine->AddComponent(entities[i], (i));
            engine->AddComponent(entities[i], (1.0f * i));
        }
    }

    std::vector<ECS::ID> sortedIds;
    std::vector<int> componentValues;
    bool skip = false;

    // Call for all entities with Component<int>
    engine->EntitiesCallFor(ECS::CreateMask<int>(),
    [&sortedIds, &componentValues, &skip](std::vector<ECS::Entity> entities, decltype(engine) engine) {
        for (auto entity : entities) {
            sortedIds.push_back(entity.GetId());
            auto component = engine->GetComponent<int>(entity);
            EXPECT_NE(component, nullptr);

            if (component == nullptr) {
                skip = true;
                return;
            }

            componentValues.push_back(*engine->GetComponent<int>(entity));
        }

        std::sort(sortedIds.begin(), sortedIds.end());
        std::sort(componentValues.begin(), componentValues.end());
    });

    if (skip) {
        GTEST_FAIL();
        GTEST_SKIP();
    }

    {
        std::vector<ECS::ID> expectedIds;
        std::vector<int> expectedComponentValues;

        for (int i = 0; i < numEntities / 3; i++) {
            expectedIds.push_back(entities[i].GetId());
            expectedComponentValues.push_back(i);
        }

        for (int i = 2 * numEntities / 3; i < numEntities; i++) {
            expectedIds.push_back(entities[i].GetId());
            expectedComponentValues.push_back(i);
        }

        EXPECT_EQ(sortedIds, expectedIds);
        EXPECT_EQ(componentValues, expectedComponentValues);
    }
}

TEST(ECSEngineTest, ComponentsCallForGetComponents) {
    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 3 * 3;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }

    {
        int i = 0;

        for (; i < numEntities / 3; i++) {
            engine->AddComponent(entities[i], (i));
        }

        for (; i < 2 * numEntities / 3; i++) {
            engine->AddComponent(entities[i], (2.0f * i));
        }

        for (; i < numEntities; i++) {
            engine->AddComponent(entities[i], (i));
            engine->AddComponent(entities[i], (2.0f * i));
        }
    }

    std::vector<ECS::ID> sortedIds;
    std::vector<int> componentValuesInt;
    std::vector<float> componentValuesFloat;
    bool skip = false;

    auto mask = ECS::CreateMask<int, float>();

    // Call for all entities with Component<int>
    engine->ComponentsCallFor(mask,
    [mask, &sortedIds, &componentValuesInt, &componentValuesFloat, &skip](decltype(engine) engine) {
        EXPECT_NE(engine->GetComponents<int>(), nullptr);
        EXPECT_NE(engine->GetComponents<float>(), nullptr);

        if (engine->GetComponents<int>() == nullptr
                || engine->GetComponents<float>() == nullptr) {
            skip = true;
            return;
        }

        auto group = engine->GetGroupView<int, float>();
        auto [componentInt, componentFloat] = group.GetViews();

        for (auto component : *componentInt) {
            componentValuesInt.push_back(component);
        }

        for (auto component : *componentFloat) {
            componentValuesFloat.push_back(component);
        }

        std::sort(componentValuesInt.begin(), componentValuesInt.end());
        std::sort(componentValuesFloat.begin(), componentValuesFloat.end());
    });

    if (skip) {
        GTEST_FAIL();
        GTEST_SKIP();
    }

    {
        std::vector<ECS::Entity> expectedIds;
        std::vector<int> expectedComponentValuesInt;
        std::vector<float> expectedComponentValuesFloat;

        for (int i = 2 * numEntities / 3; i < numEntities; i++) {
            expectedIds.push_back(entities[i]);
            expectedComponentValuesInt.push_back(i);
            expectedComponentValuesFloat.push_back(2.0f * i);
        }

        auto entitiesGot = engine->GetEntities(mask);
        std::sort(entitiesGot.begin(), entitiesGot.end());
        EXPECT_EQ(entitiesGot, expectedIds);
        EXPECT_EQ(componentValuesInt, expectedComponentValuesInt);
        EXPECT_EQ(componentValuesFloat, expectedComponentValuesFloat);
    }
}

TEST(ECSEngineTest, ComponentsCallForGetComponentsTuple) {
    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 3 * 10;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }

    {
        int i = 0;

        for (; i < numEntities / 3; i++) {
            engine->AddComponent(entities[i], (i));
        }

        for (; i < 2 * numEntities / 3; i++) {
            engine->AddComponent(entities[i], (2.0f * i));
        }

        for (; i < numEntities; i++) {
            engine->AddComponent(entities[i], (i));
            engine->AddComponent(entities[i], (2.0f * i));
        }
    }

    std::vector<ECS::ID> sortedIds;
    std::vector<int> componentValuesInt;
    std::vector<float> componentValuesFloat;
    bool skip = false;

    auto mask = ECS::CreateMask<int, float>();

    auto entities2 = engine->GetEntities(mask);
    std::sort(entities2.begin(), entities2.end());

    auto viewInt = engine->GetComponentManagerView<int>(entities2);
    auto viewFloat = engine->GetComponentManagerView<float>(entities2);

    for (auto component : viewInt) {
        componentValuesInt.push_back(component);
    }

    for (auto component : viewFloat) {
        componentValuesFloat.push_back(component);
    }

    std::sort(componentValuesInt.begin(), componentValuesInt.end());
    std::sort(componentValuesFloat.begin(), componentValuesFloat.end());


    {
        std::vector<ECS::Entity> expectedIds;
        std::vector<int> expectedComponentValuesInt;
        std::vector<float> expectedComponentValuesFloat;

        for (int i = 2 * numEntities / 3; i < numEntities; i++) {
            expectedIds.push_back(entities[i]);
            expectedComponentValuesInt.push_back(i);
            expectedComponentValuesFloat.push_back(2.0f * i);
        }

        EXPECT_EQ(entities2, expectedIds);
        EXPECT_EQ(componentValuesInt, expectedComponentValuesInt);
        EXPECT_EQ(componentValuesFloat, expectedComponentValuesFloat);
    }

    componentValuesInt.clear();
    componentValuesFloat.clear();


    // Call for all entities with Component<int>
    engine->ComponentsCallFor(mask,
    [&mask, &sortedIds, &componentValuesInt, &componentValuesFloat, &skip](decltype(engine) engine) {
        EXPECT_NE(engine->GetComponents<int>(), nullptr);
        EXPECT_NE(engine->GetComponents<float>(), nullptr);

        if (engine->GetComponents<int>() == nullptr
                || engine->GetComponents<float>() == nullptr) {
            skip = true;
            return;
        }

        auto group = engine->GetGroupView<int, float>();
        auto [componentInt, componentFloat] = group.GetViews();

        for (auto component : *componentInt) {
            componentValuesInt.push_back(component);
        }

        for (auto component : *componentFloat) {
            componentValuesFloat.push_back(component);
        }

        std::sort(componentValuesInt.begin(), componentValuesInt.end());
        std::sort(componentValuesFloat.begin(), componentValuesFloat.end());
    });

    if (skip) {
        GTEST_FAIL();
        GTEST_SKIP();
    }

    {
        std::vector<ECS::ID> expectedIds;
        std::vector<int> expectedComponentValuesInt;
        std::vector<float> expectedComponentValuesFloat;

        for (int i = 2 * numEntities / 3; i < numEntities; i++) {
            expectedIds.push_back(entities[i].GetId());
            expectedComponentValuesInt.push_back(i);
            expectedComponentValuesFloat.push_back(2.0f * i);
        }

        //EXPECT_EQ(sortedIds, expectedIds);
        EXPECT_EQ(componentValuesInt, expectedComponentValuesInt);
        EXPECT_EQ(componentValuesFloat, expectedComponentValuesFloat);
    }
}*/

/*
TEST(ECSEngineTest, DeleteEntities) {
    ECS::Engine<ECS::DefaultEngineCore> *engine = new ECS::Engine(new ECS::DefaultEngineCore());

    const int numEntities = 3 * 10;
    ECS::Entity entities[numEntities];

    for (int i = 0; i < numEntities; i++) {
        entities[i] = engine->CreateEntity();
        EXPECT_EQ(entities[i].GetId(), i + 1);

        for (int j = 0; j < i; j++) {
            EXPECT_NE(entities[i].GetId(), entities[j].GetId());
        }
    }

    {
        int i = 0;

        for (; i < numEntities / 3; i++) {
            engine->AddComponent(entities[i], (i));
        }

        for (; i < 2 * numEntities / 3; i++) {
            engine->AddComponent(entities[i], (2.0f * i));
        }

        for (; i < numEntities; i++) {
            engine->AddComponent(entities[i], (i));
            engine->AddComponent(entities[i], (2.0f * i));
        }
    }

    std::vector<ECS::ID> sortedIds;
    std::vector<int> componentValuesInt;
    std::vector<float> componentValuesFloat;
    bool skip = false;

    auto mask = ECS::CreateMask<int, float>();

    auto entities2 = engine->GetEntities(mask);
    std::sort(entities2.begin(), entities2.end());

    auto viewInt = engine->GetComponentManagerView<int>(entities2);
    auto viewFloat = engine->GetComponentManagerView<float>(entities2);

    for (auto component : viewInt) {
        componentValuesInt.push_back(component);
    }

    for (auto component : viewFloat) {
        componentValuesFloat.push_back(component);
    }

    std::sort(componentValuesInt.begin(), componentValuesInt.end());
    std::sort(componentValuesFloat.begin(), componentValuesFloat.end());


    {
        std::vector<ECS::Entity> expectedIds;
        std::vector<int> expectedComponentValuesInt;
        std::vector<float> expectedComponentValuesFloat;

        for (int i = 2 * numEntities / 3; i < numEntities; i++) {
            expectedIds.push_back(entities[i]);
            expectedComponentValuesInt.push_back(i);
            expectedComponentValuesFloat.push_back(2.0f * i);
        }

        EXPECT_EQ(entities2, expectedIds);
        EXPECT_EQ(componentValuesInt, expectedComponentValuesInt);
        EXPECT_EQ(componentValuesFloat, expectedComponentValuesFloat);
    }

    componentValuesInt.clear();
    componentValuesFloat.clear();


    // Call for all entities with Component<int>
    engine->ComponentsCallFor(mask,
    [&mask, &sortedIds, &componentValuesInt, &componentValuesFloat, &skip](decltype(engine) engine) {
        EXPECT_NE(engine->GetComponents<int>(), nullptr);
        EXPECT_NE(engine->GetComponents<float>(), nullptr);

        if (engine->GetComponents<int>() == nullptr
                || engine->GetComponents<float>() == nullptr) {
            skip = true;
            return;
        }

        auto group = engine->GetGroupView<int, float>();
        auto [componentInt, componentFloat] = group.GetViews();

        for (auto component : *componentInt) {
            componentValuesInt.push_back(component);
        }

        for (auto component : *componentFloat) {
            componentValuesFloat.push_back(component);
        }

        std::sort(componentValuesInt.begin(), componentValuesInt.end());
        std::sort(componentValuesFloat.begin(), componentValuesFloat.end());
    });

    if (skip) {
        GTEST_FAIL();
        GTEST_SKIP();
    }

    {
        std::vector<ECS::ID> expectedIds;
        std::vector<int> expectedComponentValuesInt;
        std::vector<float> expectedComponentValuesFloat;

        for (int i = 2 * numEntities / 3; i < numEntities; i++) {
            expectedIds.push_back(entities[i].GetId());
            expectedComponentValuesInt.push_back(i);
            expectedComponentValuesFloat.push_back(2.0f * i);
        }

        //EXPECT_EQ(sortedIds, expectedIds);
        EXPECT_EQ(componentValuesInt, expectedComponentValuesInt);
        EXPECT_EQ(componentValuesFloat, expectedComponentValuesFloat);
    }
}*/

