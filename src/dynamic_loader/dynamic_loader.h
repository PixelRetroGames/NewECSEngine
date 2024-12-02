#pragma once

#include "dynamic_script_interface.h"
#include <windows.h>

/*EVAL(CREATE_WRAPPER(Engine,
                    (AddComponent, (ComponentT), (void), (unsigned int, entity), (ComponentT, component)),
                    (GetGroupView, (Transform2D, RenderRect, Scaling, PlayerMovement), (ECS::ComponentGroupView<std::tuple<Transform2D, RenderRect, Scaling>, std::tuple<>>), (ECS::Unused<PlayerMovement>, unused)),
                    (CreateEntity, (), (unsigned int), (void,))))

EVAL(CREATE_WRAPPER(ResourceManager,
                    (GetSharedManager, (ResourceT, LoadArgsT), (Engine::SharedResourceManager<ResourceT, LoadArgsT> *), (void,)),
                    (GetUniqueManager, (ResourceT, LoadArgsT), (Engine::UniqueResourceManager<ResourceT, LoadArgsT> *), (void,))))

EVAL(CREATE_WRAPPER(Window,
                (GetPosition, (), (Engine::Rectangle), (void, )),
                (GetKeystate, (), (bool), (const Engine::Scancode, key))))

EVAL(CREATE_DYNAMIC_CONTEXT(DynamicGameContext, ((float, dt), (ECS::Engine<ECS::DefaultEngineCore>, engine)), (ResourceManager, resourceManager), (Window, window)))*/

namespace DynamicLoader {

template <typename Context>
using ScriptLoadFn = DynamicScriptInterface * (*)(Context*);

template <typename Context>
void LoadAndExecuteEngine(std::string scriptPath, Context *ctx) {
    ScriptLoadFn<Context> loadScriptFn;
    HINSTANCE dynamicLib = LoadLibrary(scriptPath.c_str());

    if (dynamicLib) {
        loadScriptFn = (ScriptLoadFn<Context>)GetProcAddress(dynamicLib, "CreateScript");

        if (loadScriptFn) {
            DynamicScriptInterface *script = loadScriptFn();
            script->Run(ctx);
        } else {
            std::cout << "LoadScene not found in dll";
        }

        FreeLibrary(dynamicLib);
    } else {
        std::cout << "Script not found!\n";
    }
}

struct ScriptLoadInfo {
    std::string libPath;
    std::string fnName;

    ScriptLoadInfo(std::string libPath, std::string fnName) : libPath(libPath), fnName(fnName) {}
};

template <typename Context>
class DynamicScriptManager {
  private:
    struct LibInstance {
        HINSTANCE lib;
        std::unordered_map<std::string, std::shared_ptr<DynamicScriptInterface>> scripts;

        LibInstance(HINSTANCE lib) : lib(lib) {}
    };

    std::unordered_map<std::string, LibInstance> libs;
    Context *ctx;

  public:
    DynamicScriptManager() {}
    DynamicScriptManager(Context *ctx) : ctx(ctx) {}

    void SetContext(Context *ctx) {
        this->ctx = ctx;
    }

    std::weak_ptr<DynamicScriptInterface> Load(ScriptLoadInfo &&info) {
        auto libIt = libs.find(info.libPath);
        HINSTANCE dynamicLib;

        // The library was already loaded
        if (libIt != libs.end()) {
            auto scriptIt = libIt->second.scripts.find(info.fnName);

            // The script was already loaded
            if (scriptIt != libIt->second.scripts.end()) {
                return std::weak_ptr(scriptIt->second);
            }

            dynamicLib = libIt->second.lib;
        } else {
            dynamicLib = LoadLibrary(info.libPath.c_str());

            if (!dynamicLib) {
                std::cout << "Lib can't be loaded!\n";
                return std::weak_ptr<DynamicScriptInterface>();
            }

            auto [it, inserted] = libs.insert({info.libPath, dynamicLib});
            libIt = it;
        }

        ScriptLoadFn<Context> loadScriptFn;

#pragma GCC diagnostic ignored "-Wcast-function-type"
        loadScriptFn = (ScriptLoadFn<Context>)GetProcAddress(dynamicLib, info.fnName.c_str());
#pragma GCC diagnostic pop

        if (!loadScriptFn) {
            std::cout << "Script not found in dll\n";
            return std::weak_ptr<DynamicScriptInterface>();
        }

        std::shared_ptr<DynamicScriptInterface> script = std::shared_ptr<DynamicScriptInterface>(loadScriptFn(ctx));

        if (!script) {
            std::cout << "Script can't be loaded!\n";
            return std::weak_ptr<DynamicScriptInterface>();
        }

        libIt->second.scripts.insert({info.fnName, script});
        std::cout << "Loaded " << info.fnName << '\n';
        return std::weak_ptr(script);
    }

    std::weak_ptr<DynamicScriptInterface> Load(std::string libPath, std::string fnName) {
        ScriptLoadInfo info(libPath, fnName);
        return Load(ScriptLoadInfo(libPath, fnName));
    }

    void UnLoadLib(std::string libPath) {
        auto libIt = libs.find(libPath);

        if (libIt == libs.end()) {
            return;
        }

        HINSTANCE lib = libIt->second.lib;

        for (auto script : libIt->second.scripts) {
            auto sc = script.second.get();
            sc->TearDown();
        }

        libs.erase(libIt);

        FreeLibrary(lib);

        std::cout << "Unloaded " << libPath << '\n';
    }
};

}
