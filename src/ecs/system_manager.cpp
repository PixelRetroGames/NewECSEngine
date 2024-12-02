#include "system_manager.h"

namespace ECS {

void SystemManager::Register(System system) {
    systemsMap.insert({idManager.GetId(), system});
}

void SystemManager::Register(SystemInterface *sysInt) {
    auto id = idManager.GetId();
    systemsMap.insert({id, System([sysInt]() {
        sysInt->Update();
    })});
}

void SystemManager::Unregister(ID id) {
    systemsMap.erase(id);
}

void SystemManager::CallAll() {
    for (auto &it : systemsMap) {
        it.second.fn();
    }
}

}
