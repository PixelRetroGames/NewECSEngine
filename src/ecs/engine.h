#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>
#include "entity.h"
#include "component.h"
#include "component_manager.h"
#include "../engine/utils.h"
#include "unique_ids_manager.h"
#include "system_manager.h"
#include "component_usage_types.h"

#include "../logging/logging.h"

namespace ECS {

//void PrintEntityVector(const std::vector<Entity> &entities);

template <typename EngineCoreT>
class Engine {
  private:
    EngineCoreT *core;

  public:
    Engine() {}
    Engine(EngineCoreT *core) : core(core) {}

    Entity CreateEntity() {
        return core->CreateEntity();
    }

    template<typename... ComponentsT>
    void DeleteComponents(Entity entity) {
        core->template DeleteComponents<ComponentsT...>(entity);
    }

    void DeleteEntity(Entity entity) {
        core->DeleteEntity(entity);
    }

    void DeleteEntities(const std::vector<Entity> &entities) {
        core->DeleteEntities(entities);
    }

    template <typename T>
    void AddComponent(const Entity &entity, const T &data) {
        core->AddComponent(entity, data);
    }

    template <typename T>
    T *GetComponent(Entity entity) {
        return core->template GetComponent<T>(entity);
    }

    std::vector<Entity> GetEntities(ComponentMask mask) {
        return core->GetEntities(mask);
    }

    void EntitiesCallFor(ComponentMask mask, std::function<void (std::vector<Entity>, Engine<EngineCoreT>*)> fn) {
        return core->EntitiesCallFor(mask, fn, this);
    }

    template <typename T>
    const std::vector<std::pair<Entity, T>> *GetComponents() {
        return core->template GetComponentsEntities<T>();
    }

    template <typename T>
    const ComponentManagerView<T> GetComponentManagerView(std::vector<Entity> entities) {
        return core->template GetComponentManagerView<T>(entities);
    }

    template <typename... T>
    ComponentGroupView<std::tuple<T...>, std::tuple<>> GetGroupView() {
        return core->template GetGroupView<T...>();
    }

    template <typename... T, typename UnusedT1, typename... UnusedT>
    ComponentGroupView<std::tuple<T...>, std::tuple<>> GetGroupView(Unused<UnusedT1, UnusedT...> unused) {
        return core->template GetGroupView<T...>(unused);
    }


    template <typename... T, typename OptionalT1, typename... OptionalT>
    ComponentGroupView<std::tuple<T...>, std::tuple<OptionalT1, OptionalT...>> GetGroupView(Optional<OptionalT1, OptionalT...> opt) {
        return core->template GetGroupView<T...>(opt);
    }

    template <typename... T, typename OptionalT1, typename... OptionalT, typename... UnusedT>
    ComponentGroupView<std::tuple<T...>, std::tuple<OptionalT1, OptionalT...>> GetGroupView(Optional<OptionalT1, OptionalT...> opt, Unused<UnusedT...> unused) {
        return core->template GetGroupView<T...>(opt, unused);
    }

    template <typename T>
    ComponentManager<T> *GetComponentManager() {
        return core->template GetComponentManager<T>();
    }

    void Register(System system) {
        core->Register(system);
    }

    void Register(SystemInterface *sysInt) {
        core->Register(sysInt);
    }

    void Register(const CallbackType &fn) {
        core->Register(System(fn));
    }

    void Unregister(ID id) {
        core->Unregister(id);
    }

    void CallAll() {
        core->CallAll();
    }
};

class DefaultEngineCore {
  private:
    SystemManager systemManager;
    UniqueIdsManager<DefaultEngineCore> entityIdManager;
    std::map<Entity, ComponentMask> entityTypeMap;
    std::unordered_map<ID, std::unique_ptr<ComponentManagerBase>> componentManagers;

  public:
    Entity CreateEntity();
    void DeleteEntity(Entity entity);
    void DeleteEntities(const std::vector<Entity> &entities);

    template <typename T>
    void DeleteComponent(Entity entity) {
        entityTypeMap[entity].erase(Component<T>::GetTypeId());
        GetComponentManager<T>()->RemoveComponent(entity);
    }

    template <typename... ComponentsT>
    void DeleteComponents(Entity entity) {
        (DeleteComponent<ComponentsT>(entity), ...);
    }

    template <typename T>
    void AddComponent(const Entity &entity, const T &data) {
        // add a new entity
        if (entityTypeMap.count(entity) == 0) {
            entityTypeMap[entity].insert(Component<T>::GetTypeId());
        } else {
            entityTypeMap[entity].insert(Component<T>::GetTypeId());
        }

        GetComponentManager<T>()->AddComponent(entity, data);
    }

    template <typename T>
    ComponentManager<T> *GetComponentManager() {
        if (componentManagers.count(Component<T>::GetTypeId()) == 0) {
            LOG_INFO("debug", "Added component manager for %u", Component<T>::GetTypeId());
            componentManagers[Component<T>::GetTypeId()] = std::make_unique<ComponentManager<T>>();
        }

        return static_cast<ComponentManager<T>*>(componentManagers[Component<T>::GetTypeId()].get());
    }

    template <typename T>
    T *GetComponent(Entity entity) {
        return GetComponentManager<T>()->GetComponent(entity);
    }

    std::vector<Entity> GetEntities(ComponentMask mask);

    void EntitiesCallFor(ComponentMask mask, std::function<void (std::vector<Entity>, Engine<DefaultEngineCore>*)> fn, Engine<DefaultEngineCore> *engine);

    template <typename T>
    const std::vector<std::pair<Entity, T>> *GetComponentsEntities() {
        return GetComponentManager<T>()->GetComponentEntityVector();
    }

    template <typename T>
    ComponentManagerView<T> GetComponentManagerView(std::vector<Entity> entities) {
        return GetComponentManager<T>()->GetView(entities);
    }

    template <typename... T>
    ComponentGroupView<std::tuple<T...>, std::tuple<>> GetGroupView() {
        auto entities = GetEntities(CreateMask<T...>());
        return ComponentGroupView<std::tuple<T...>, std::tuple<>>(entities, GetComponentManager<T>()...);
    }

    template <typename... T, typename UnusedT1, typename... UnusedT>
    ComponentGroupView<std::tuple<T...>, std::tuple<>> GetGroupView(__attribute__((unused)) Unused<UnusedT1, UnusedT...> unused) {
        auto entities = GetEntities(CreateMask<T..., UnusedT1, UnusedT...>());
        return ComponentGroupView<std::tuple<T...>, std::tuple<>>(entities, GetComponentManager<T>()...);
    }

    template <typename... T, typename OptionalT1, typename... OptionalT>
    ComponentGroupView<std::tuple<T...>, std::tuple<OptionalT1, OptionalT...>> GetGroupView(__attribute__((unused)) Optional<OptionalT1, OptionalT...> opt) {
        auto entities = GetEntities(CreateMask<T...>());
        std::vector<Entity> entitiesWithOptional;

        {
            std::vector<Entity> aux;
            aux = GetEntities(CreateMask<OptionalT1>());
            entitiesWithOptional.insert(entitiesWithOptional.end(), aux.begin(), aux.end());

            ((aux = GetEntities(CreateMask<OptionalT>()), entitiesWithOptional.insert(entitiesWithOptional.end(), aux.begin(), aux.end())), ...);
        }

        return ComponentGroupView<std::tuple<T...>, std::tuple<OptionalT1, OptionalT...>>(entities, GetComponentManager<T>()..., GetComponentManager<OptionalT1>(), GetComponentManager<OptionalT>()...);
    }

    template <typename... T, typename UnusedT1, typename... UnusedT, typename OptionalT1, typename... OptionalT>
    ComponentGroupView<std::tuple<T...>, std::tuple<OptionalT1, OptionalT...>> GetGroupView(Optional<OptionalT1, OptionalT...> opt, Unused<UnusedT1, UnusedT...> unused) {
        auto entities = GetEntities(CreateMask<T..., UnusedT1, UnusedT...>());
        std::vector<Entity> entitiesWithOptional;

        {
            std::vector<Entity> aux;
            aux = GetEntities(CreateMask<OptionalT1>());
            entitiesWithOptional.insert(entitiesWithOptional.end(), aux.begin(), aux.end());

            ((aux = GetEntities(CreateMask<OptionalT>()), entitiesWithOptional.insert(entitiesWithOptional.end(), aux.begin(), aux.end())), ...);
        }

        return ComponentGroupView<std::tuple<T...>, std::tuple<OptionalT1, OptionalT...>>(entities, GetComponentManager<T>()..., GetComponentManager<OptionalT1>(), GetComponentManager<OptionalT>()...);
    }

    void Register(System system);
    void Register(SystemInterface *sysInt);
    void Unregister(ID id);
    void CallAll();
};

}
