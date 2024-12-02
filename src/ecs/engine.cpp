#include "engine.h"

namespace ECS {

Entity DefaultEngineCore::CreateEntity() {
    auto entity = Entity(entityIdManager.GetId());
    AddComponent(entity, entity);
    return entity;
}

void DefaultEngineCore::DeleteEntity(Entity entity) {
    for (auto &cm : componentManagers) {
        cm.second->RemoveComponent(entity);
    }

    entityIdManager.FreeId(entity.GetId());
    entityTypeMap.erase(entity);
}

void DefaultEngineCore::DeleteEntities(const std::vector<Entity> &entities) {
    for (auto &entity : entities) {
        DeleteEntity(entity);
    }
}

std::vector<Entity> DefaultEngineCore::GetEntities(ComponentMask mask) {
    std::vector<Entity> matchingEntities;

    for (auto &it : entityTypeMap) {
        std::vector<ID> intersection;
        std::set_intersection(it.second.begin(), it.second.end(), mask.begin(), mask.end(), std::back_inserter(intersection));

        if (intersection.size() == mask.size()) {
            matchingEntities.push_back(it.first);
        }
    }

    return matchingEntities;
}

void DefaultEngineCore::EntitiesCallFor(ComponentMask mask,
                                        std::function<void (std::vector<Entity>, Engine<DefaultEngineCore> *engine)> fn, Engine<DefaultEngineCore> *engine) {
    auto entities = GetEntities(mask);
    fn(entities, engine);
}

void DefaultEngineCore::Register(System system) {
    systemManager.Register(system);
}

void DefaultEngineCore::Register(SystemInterface *sysInt) {
    systemManager.Register(sysInt);
}

void DefaultEngineCore::Unregister(ID id) {
    systemManager.Unregister(id);
}

void DefaultEngineCore::CallAll() {
    systemManager.CallAll();
}

}
