#include "../../src/ecs/component.h"

#include "gtest/gtest.h"

TEST(ECSComponentTest, TypeId) {
    class A {};
    class B {};
    class C {};

    auto a = ECS::Component<A>(A());
    auto b = new ECS::Component<B>(B());
    auto c = new ECS::Component<C>(C());

    A sq;
    ECS::Component<A> anotherA(sq);

    EXPECT_NE(ECS::Component<A>::GetTypeId(), ECS::Component<B>::GetTypeId());
    EXPECT_NE(ECS::Component<B>::GetTypeId(), ECS::Component<C>::GetTypeId());
    EXPECT_NE(ECS::Component<C>::GetTypeId(), ECS::Component<A>::GetTypeId());

    // Test Object type
    EXPECT_EQ(a.GetTypeId(), ECS::Component<A>::GetTypeId());
    EXPECT_EQ(b->GetTypeId(), ECS::Component<B>::GetTypeId());
    EXPECT_EQ(c->GetTypeId(), ECS::Component<C>::GetTypeId());
    EXPECT_EQ(anotherA.GetTypeId(), ECS::Component<A>::GetTypeId());
}

TEST(ECSComponentTest, CreateMaskInitializerList) {
    auto mask = ECS::CreateMask({0, 7, 3});

    ECS::ComponentMask expectedMask;
    expectedMask.insert(0);
    expectedMask.insert(3);
    expectedMask.insert(7);

    EXPECT_EQ(mask, expectedMask);
}

TEST(ECSComponentTest, CreateMaskTemplate) {
    auto mask = ECS::CreateMask<int, float, double>();
    auto maskInt = ECS::CreateMask<int>();
    auto maskFloat = ECS::CreateMask<float>();
    auto maskDouble = ECS::CreateMask<double>();

    ECS::ComponentMask expectedMaskInt;
    ECS::ComponentMask expectedMaskFloat;
    ECS::ComponentMask expectedMaskDouble;

    expectedMaskInt.insert(ECS::Component<int>::GetTypeId());
    expectedMaskFloat.insert(ECS::Component<float>::GetTypeId());
    expectedMaskDouble.insert(ECS::Component<double>::GetTypeId());

    EXPECT_EQ(maskInt, expectedMaskInt);
    EXPECT_EQ(maskFloat, expectedMaskFloat);
    EXPECT_EQ(maskDouble, expectedMaskDouble);

    ECS::ComponentMask maskUnion;
    maskUnion.insert(maskInt.begin(), maskInt.end());
    maskUnion.insert(maskFloat.begin(), maskFloat.end());
    maskUnion.insert(maskDouble.begin(), maskDouble.end());

    ECS::ComponentMask expectedMaskUnion;
    expectedMaskUnion.insert(expectedMaskInt.begin(), expectedMaskInt.end());
    expectedMaskUnion.insert(expectedMaskFloat.begin(), expectedMaskFloat.end());
    expectedMaskUnion.insert(expectedMaskDouble.begin(), expectedMaskDouble.end());

    EXPECT_EQ(mask, maskUnion);
    EXPECT_EQ(mask, expectedMaskUnion);
}
