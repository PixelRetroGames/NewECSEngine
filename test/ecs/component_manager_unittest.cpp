#include "gtest/gtest.h"

#include "../../src/ecs/component_manager.h"
#include <memory>

template <int unused>
struct Component {
    int x, y;
    Component(int x, int y) : x(x), y(y) {}
    operator==(const Component &other) const {
        return x == other.x && y == other.y;
    }

    operator!=(const Component &other) const {
        return !(*this == other);
    }

    operator<(const Component &other) const {
        if (x == other.x)
            return y < other.y;

        return x < other.x;
    }

    friend void PrintTo(const Component<unused> &component, std::ostream* os) {
        *os << "(" << component.x << "," << component.y << ")";
    }
};

class ECSComponentManagerTest : public ::testing::Test {
  protected:
    std::unique_ptr<ECS::ComponentManager<Component<1>>> manager;

    ECSComponentManagerTest() {
        manager = std::make_unique<ECS::ComponentManager<Component<1>>>();
    }
};

TEST_F(ECSComponentManagerTest, AddComponent) {
    const int numEntities = 5;

    for (int i = 1; i <= numEntities; i++) {
        manager->AddComponent(ECS::Entity(i), Component<1>(i, i * 2));
    }

    for (int i = 1; i <= numEntities; i++) {
        auto component = manager->GetComponent(ECS::Entity(i));
        EXPECT_EQ(*component, Component<1>(i, i * 2));

        component->x = i * 10;
        component->y = i * 20;
    }

    for (int i = 1; i <= numEntities; i++) {
        auto component = manager->GetComponent(ECS::Entity(i));
        EXPECT_EQ(*component, Component<1>(i * 10, i * 20));
    }
}

TEST_F(ECSComponentManagerTest, RemoveComponent) {
    const int numEntities = 3 * 10;

    for (int i = 1; i <= numEntities; i++) {
        if (i % 3 == 1) {
            manager->AddComponent(ECS::Entity(i), Component<1>(i, i * 1));
        } else if (i % 3 == 2) {
            manager->AddComponent(ECS::Entity(i), Component<1>(i, i * 2));
        }
    }

    for (int i = 1; i <= numEntities; i += 3) {
        manager->RemoveComponent(ECS::Entity(i));
    }

    for (int i = 1; i <= numEntities; i++) {
        auto component = manager->GetComponent(ECS::Entity(i));

        if (i % 3 == 2) {
            EXPECT_EQ(*component, Component<1>(i, i * 2));
        } else {
            EXPECT_EQ(component, nullptr);
        }
    }

    for (int i = 3; i <= numEntities; i += 3) {
        manager->AddComponent(ECS::Entity(i), Component<1>(i, i * 3));
    }

    for (int i = 1; i <= numEntities; i++) {
        auto component = manager->GetComponent(ECS::Entity(i));

        if (i % 3 == 1) {
            EXPECT_EQ(component, nullptr);
        } else if (i % 3 == 2) {
            EXPECT_EQ(*component, Component<1>(i, i * 2));
        } else {
            EXPECT_EQ(*component, Component<1>(i, i * 3));
        }
    }
}

TEST_F(ECSComponentManagerTest, GetComponentsAndEntities) {
    const int numEntities = 20;
    std::vector<std::pair<ECS::Entity, Component<1>>> expectedComponentsEntities;

    for (int i = 1; i <= numEntities; i++) {
        manager->AddComponent(ECS::Entity(i), Component<1>(i, i * 2));

        if (i % 2 == 0) {
            expectedComponentsEntities.emplace_back(std::make_pair(ECS::Entity(i), Component<1>(i, i * 2)));
        }
    }

    for (int i = 1; i <= numEntities; i += 2) {
        manager->RemoveComponent(ECS::Entity(i));
    }

    auto componentsEntities = std::vector<std::pair<ECS::Entity, Component<1>>>(*manager->GetComponentEntityVector());

    EXPECT_EQ(componentsEntities, expectedComponentsEntities);
}

TEST(ECSComponentIterator, Iterate) {
    std::unique_ptr<ECS::ComponentManagerBase> managerBase;
    ECS::ComponentManager<Component<1>>* manager;

    managerBase = std::make_unique<ECS::ComponentManager<Component<1>>>();
    manager = static_cast<ECS::ComponentManager<Component<1>>*>(managerBase.get());

    const int numEntities = 20;
    std::vector<ECS::Entity> entities;

    for (int i = 1; i < numEntities; i += 4) {
        entities.push_back(i);
    }

    std::vector<Component<1>> expectedComponents;

    for (int i = 1; i <= numEntities; i++) {
        manager->AddComponent(ECS::Entity(i), Component<1>(i, i * 2));

        if (i % 4 == 1) {
            expectedComponents.push_back(Component<1>(i, i * 2));
        }
    }

    auto view = manager->GetView(entities);
    std::vector<Component<1>> components;

    int i = 1;

    for (auto it : view) {
        components.push_back(it);
        i++;
    }

    std::sort(components.begin(), components.end());

    EXPECT_EQ(components, expectedComponents);
}

TEST(ECSComponentGroupIterator, IterateOneComponentManager) {
    std::vector<std::unique_ptr<ECS::ComponentManagerBase>> managersBase;
    std::vector<ECS::ComponentManager<Component<1>>*> managers;

    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<1>>>());
    managers.push_back(static_cast<ECS::ComponentManager<Component<1>>*>(managersBase[0].get()));

    const int numEntities = 20;
    std::vector<ECS::Entity> entities;

    for (int i = 1; i < numEntities / 4; i++) {
        entities.push_back(i);
    }

    for (int i = 1; i <= numEntities; i++) {
        managers[0]->AddComponent(ECS::Entity(i), Component<1>(i, i * 2));
    }

    std::vector<ECS::ComponentManagerBase*> basePtr;
    basePtr.push_back(managersBase[0].get());
    //auto it = ECS::ComponentGroupView<Component<1>>(entities, basePtr);
    ECS::ComponentGroupView<std::tuple<Component<1>>> view(entities, managers[0]);
    int i = 1;

    for (auto it : view) {
        auto [c1] = it;
        EXPECT_EQ(c1, Component<1>(i, i * 2));
        i++;
    }
}

/*
TEST(ECSComponentGroupIterator, IterateMultipleComponentManagers) {
    const int numManagers = 4;
    std::vector<std::unique_ptr<ECS::ComponentManagerBase>> managersBase;
    ECS::ComponentManager<Component<1>>* manager1;
    ECS::ComponentManager<Component<2>>* manager2;
    ECS::ComponentManager<Component<3>>* manager3;
    ECS::ComponentManager<Component<4>>* manager4;

    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<1>>>());
    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<2>>>());
    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<3>>>());
    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<4>>>());

    manager1 = static_cast<ECS::ComponentManager<Component<1>>*>(managersBase[0].get());
    manager2 = static_cast<ECS::ComponentManager<Component<2>>*>(managersBase[1].get());
    manager3 = static_cast<ECS::ComponentManager<Component<3>>*>(managersBase[2].get());
    manager4 = static_cast<ECS::ComponentManager<Component<4>>*>(managersBase[3].get());

    const int numEntities = 20;
    std::vector<ECS::Entity> entities[numManagers];

    /*
    Component distribution:
    Entity 1: C1, C2, C3, C4, ...
    Entity 2: C2, C3, C4, ...
    Entity 3: C3, C4, ...
    ...
    Entity numManagers: C numManagers



    for (int i = 1; i <= numEntities; i++) {
        switch ((i - 1) % numManagers) {
        case 0:
            manager1->AddComponent(ECS::Entity(i), Component<1>(i, i * 1));
            entities[0].push_back(i);

        case 1:
            manager2->AddComponent(ECS::Entity(i), Component<2>(i, i * 2));
            entities[1].push_back(i);

        case 2:
            manager3->AddComponent(ECS::Entity(i), Component<3>(i, i * 3));
            entities[2].push_back(i);

        case 3:
            manager4->AddComponent(ECS::Entity(i), Component<4>(i, i * 4));
            entities[3].push_back(i);
        }
    }

    std::vector<ECS::ComponentManagerBase*> basePtr;

    for (int i = 0; i < numManagers; i++) {
        basePtr.push_back(managersBase[i].get());
    }

    std::vector<ECS::Entity> entityIntersection = entities[0];

    for (int i = 1; i < numManagers; i++) {
        std::vector<ECS::Entity> aux;
        std::set_intersection(entityIntersection.begin(), entityIntersection.end(),
            entities[i].begin(), entities[i].end(),
            std::back_inserter(aux));
        std::swap(aux, entityIntersection);
    }

    //entityIntersection.erase(entityIntersection.begin() + 1, entityIntersection.end());

    auto it = ECS::ComponentGroupIterator<Component<1>, Component<2>, Component<3>, Component<4>>(entityIntersection, basePtr);

    it.start();
    int i = 1;
    while (!it.finished()) {
        auto [c1, c2, c3, c4] = it.get();
        EXPECT_EQ(*c1, Component<1>(i, i * 1));
        EXPECT_EQ(*c2, Component<2>(i, i * 2));
        EXPECT_EQ(*c3, Component<3>(i, i * 3));
        EXPECT_EQ(*c4, Component<4>(i, i * 4));
        it.advance();
        i += numManagers;
    }
}*/

TEST(ECSComponentGroupIterator, IterateMultipleComponentManagers) {
    const int numManagers = 4;
    std::vector<std::unique_ptr<ECS::ComponentManagerBase>> managersBase;
    ECS::ComponentManager<Component<1>>* manager1;
    ECS::ComponentManager<Component<2>>* manager2;
    ECS::ComponentManager<Component<3>>* manager3;
    ECS::ComponentManager<Component<4>>* manager4;

    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<1>>>());
    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<2>>>());
    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<3>>>());
    managersBase.push_back(std::make_unique<ECS::ComponentManager<Component<4>>>());

    manager1 = static_cast<ECS::ComponentManager<Component<1>>*>(managersBase[0].get());
    manager2 = static_cast<ECS::ComponentManager<Component<2>>*>(managersBase[1].get());
    manager3 = static_cast<ECS::ComponentManager<Component<3>>*>(managersBase[2].get());
    manager4 = static_cast<ECS::ComponentManager<Component<4>>*>(managersBase[3].get());

    const int numEntities = 100;
    std::vector<ECS::Entity> entities[numManagers];

    /*
    Component distribution:
    Entity 1: C1, C2, C3, C4, ...
    Entity 2: C2, C3, C4, ...
    Entity 3: C3, C4, ...
    ...
    Entity numManagers: C numManagers
    */

    for (int i = 1; i <= numEntities; i++) {
        switch ((i - 1) % numManagers) {
        case 0:
            manager1->AddComponent(ECS::Entity(i), Component<1>(i, i * 1));
            entities[0].push_back(i);
            [[fallthrough]];

        case 1:
            manager2->AddComponent(ECS::Entity(i), Component<2>(i, i * 2));
            entities[1].push_back(i);
            [[fallthrough]];

        case 2:
            manager3->AddComponent(ECS::Entity(i), Component<3>(i, i * 3));
            entities[2].push_back(i);
            [[fallthrough]];

        case 3:
            manager4->AddComponent(ECS::Entity(i), Component<4>(i, i * 4));
            entities[3].push_back(i);
            [[fallthrough]];
        }
    }

    std::vector<ECS::ComponentManagerBase*> basePtr;

    for (int i = 0; i < numManagers; i++) {
        basePtr.push_back(managersBase[i].get());
    }

    std::vector<ECS::Entity> entityIntersection = entities[0];

    for (int i = 1; i < numManagers; i++) {
        std::vector<ECS::Entity> aux;
        std::set_intersection(entityIntersection.begin(), entityIntersection.end(),
                              entities[i].begin(), entities[i].end(),
                              std::back_inserter(aux));
        std::swap(aux, entityIntersection);
    }

    //entityIntersection.erase(entityIntersection.begin() + 1, entityIntersection.end());

    auto view = ECS::ComponentGroupView<std::tuple<Component<1>, Component<2>, Component<3>, Component<4>>>(
                    entityIntersection,
                    manager1,
                    manager2,
                    manager3,
                    manager4);

    int i = 1;

    for (auto it : view) {
        auto [c1, c2, c3, c4] = it;
        EXPECT_EQ(c1, Component<1>(i, i * 1));
        EXPECT_EQ(c2, Component<2>(i, i * 2));
        EXPECT_EQ(c3, Component<3>(i, i * 3));
        EXPECT_EQ(c4, Component<4>(i, i * 4));
        i += numManagers;
    }
}
