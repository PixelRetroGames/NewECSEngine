#include "component.h"

namespace ECS {

ComponentMask CreateMask(std::initializer_list<ID> ids) {
    ComponentMask mask;

    for (auto id : ids) {
        mask.insert(id);
    }

    return mask;
}

}
