#pragma once

#include <set>
#include <typeinfo>
#include <typeindex>
#include "common.h"

namespace ECS {

/*class TypeCounter {
  public:
    static ID GetTypeId() {
        static ID counter = 0u;
        return (++counter);
    }
};*/

template <typename T>
class Component {
  public:
    T data;

    Component(T data) : data(data) {
        GetTypeId();
    }

    static ID GetTypeId() {
        return std::type_index(typeid(Component<T>)).hash_code();
    }
};

typedef std::set<ID> ComponentMask;

ComponentMask CreateMask(std::initializer_list<ID> ids);

template <typename... T>
ComponentMask CreateMask() {
    return CreateMask({ECS::Component<T>::GetTypeId()...});
}

}

namespace std {
template <>
struct hash<ECS::ComponentMask> {
    size_t operator()(const ECS::ComponentMask &p) const {
        std::string acc;

        for (auto id : p) {
            acc += std::to_string(id);
        }

        return std::hash<string>()(acc);
    }
};
}
