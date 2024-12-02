#pragma once

#include <functional>
#include "common.h"

namespace ECS {

class Entity {
    ID id;

  public:
    Entity() : id(0u) {}
    Entity(const ID id) : id(id) {}

    ID GetId() const;

    operator int() const {
        return id;
    }

    bool operator==(const Entity& other) const {
        return GetId() == other.GetId();
    }

    bool operator!=(const Entity& other) const {
        return GetId() != other.GetId();
    }

    bool operator<(const Entity& other) const {
        return GetId() < other.GetId();
    }
};

}

namespace std {
template <>
struct hash<ECS::Entity> {
    size_t operator()(const ECS::Entity &p) const {
        return hash<unsigned int>()(p.GetId());
    }
};
}
