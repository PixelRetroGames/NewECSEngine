#pragma once

#include "unique_ids_manager.h"
#include <functional>
#include <unordered_map>
#include "component_usage_types.h"

namespace ECS {

typedef std::function<void(void)> CallbackType;

class System {
  private:
    friend class SystemManager;
    CallbackType fn;

  public:
    System(CallbackType fn) : fn(fn) {}
};

class SystemInterface {
  public:
    virtual ~SystemInterface() {}
    virtual void Update() = 0;
};

class SystemManager {
  private:
    UniqueIdsManager<SystemManager> idManager;
    std::unordered_map<ID, System> systemsMap;

  public:
    void Unregister(ID id);
    void Register(System system);
    void Register(SystemInterface *sysInt);

    void CallAll();
};

#define StatelessSystem(Name, GameContext) \
class Name : public ECS::SystemInterface {\
private:\
    GameContext *ctx;\
\
public:\
    Name (GameContext *ctx):\
        ctx(ctx) {}\
\
    virtual void Update() override;\
}

}
