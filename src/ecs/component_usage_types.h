#pragma once

#include "component.h"

namespace ECS {

//void PrintEntityVector(const std::vector<Entity> &entities);

template <typename... T>
class ComponentPack {
  public:
    ComponentMask GetMask() {
        return CreateMask<T...>();
    }
};

template <typename... T>
class Unused : public ComponentPack<T...> {};

template <typename... T>
class Optional : public ComponentPack<T...> {};

class ComponentGroupMask {
  public:
    ComponentMask component;
    ComponentMask unused;
    ComponentMask optional;

    template <typename ComponentT, typename UnusedT, typename OptionalT>
    ComponentGroupMask(ComponentT component, UnusedT unused, OptionalT optional) {
        this->component = component.GetMask();
        this->unused = unused.GetMask();
        this->optional = optional.GetMask();
    }

    ComponentGroupMask() {
    }
};

}
