#pragma once

#include <queue>
#include "common.h"

namespace ECS {

template <typename ClientT>
class UniqueIdsManager {
  private:
    ID lastId = 0;
    std::queue<ID> reusableIds;

    ID GenerateId() {
        return (++lastId);
    }

  public:
    ID GetId() {
        if (reusableIds.empty()) {
            return GenerateId();
        }

        ID id = reusableIds.front();
        reusableIds.pop();
        return id;
    }

    void FreeId(const ID id) {
        reusableIds.push(id);
    }
};

}
