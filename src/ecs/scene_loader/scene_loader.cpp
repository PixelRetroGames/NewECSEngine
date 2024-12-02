#include "scene_loader.h"

namespace ECSScene {

void Scene::Load(std::string path) {
    std::ifstream f(path);
    nlohmann::json data = nlohmann::json::parse(f);

    for (auto elem : data["entities"]) {
        Entity ent(elem);
        entities.push_back(ent);
    }

    for (auto ent : entities) {
        printf("%s\n", ent.ToString2().c_str());
    }
}

void SceneLoader::Load(std::string path) {
    scene.Load(path);
}

};
