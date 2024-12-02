#pragma once

#include <string>
#include <unordered_map>

#include <fstream>
#include "json.hpp"

namespace ECSScene {

struct Component {
    std::string name;
    std::string args;

    Component(nlohmann::json data) {
        name = data["name"];
        args = data["args"];
    }

    std::string ToString2() {
        return "name : " + name + "\n" + "args : " + args;
    }
};

struct Entity {
    std::string tag;
    std::vector<Component> components;

    Entity(nlohmann::json data) {
        tag = data["tag"];

        for (auto elem : data["components"]) {
            Component component(elem);
            components.push_back(component);
        }
    }

    std::string ToString2() {
        auto str = "tag : " + tag + "\n" + "components : ";

        for (auto component : components) {
            str += component.ToString2();
        }

        return str;
    }
};

class Scene {
  private:
    std::vector<Entity> entities;

  public:
    void Load(std::string path);
};

class SceneLoader {
  private:
    Scene scene;

  public:
    void Load(std::string path);
};

};
