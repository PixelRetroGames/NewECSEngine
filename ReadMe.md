# Entity Component System Game Engine

## Entity Component System Engine
### Entities and Components
Entities represent unique identifiers.

Components are structs that represent attributes of entities. Entities can have multiple components, but only one of each type of component.

#### Component example

```cpp
struct PlayerMovement {
    Engine::Scancode keyUp, keyDown;
    PlayerMovement(Engine::Scancode keyUp, Engine::Scancode keyDown) : keyUp(keyUp), keyDown(keyDown) {}
};
```

#### Creating a new Entity and adding Components

```cpp
auto player = engine->CreateEntity();
auto texture = textureSharedManager->Load("samples/pong/assets/player.png");
auto scaling = Scaling(1, 15);

engine->AddComponent(player, Transform2D(window.GetPosition().w - texture->w * scaling.x - 10, window.GetPosition().h / 2));
engine->AddComponent(player, std::move(texture));
engine->AddComponent(player, scaling);
engine->AddComponent(player, Velocity2D(0, 0));
engine->AddComponent(player, VelocityMoved());
engine->AddComponent(player, Collider(0, 0, 2, texture->h));
engine->AddComponent(player, Pong::PlayerMovement(Engine::Scancode::SCANCODE_UP, Engine::Scancode::SCANCODE_DOWN));
```

```cpp
// Removing components from one entity
ctx->engine->DeleteComponents<NewTransform2D, Scaling, Transform2D>(entity);
```

### Group View
The Group View is the backbone of the whole engine, as it does the fetching of components. It gets a Group Iterator that is used to iterate over the components of the entities that respect a given filter.

The Group View can be used in the following ways:

#### Simple Group View

In this example the filter is `(ECS::Entity, Transform2D and NewTransform2D)`, so the engine will get a Group Iterator over the components of entities that have all of the components from the filter.
`[entity, transform2D, newTransform]` are references to Components, entity is a component that holds the id of the entity.

```cpp
for (auto [entity, transform2D, newTransform] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, NewTransform2D>()) {
    transform2D = newTransform;
    ctx->engine->DeleteComponents<NewTransform2D>(entity);
}
```

#### Group View with Optionals

In this example the filter means: `Transform2D` is mandatory and `transform2D` is a reference to the `Transform2D` component, `ECS::Optional<RenderRect, Scaling, SharedTexturePtr, UniqueTexturePtr>()` means that the rest of the components are optional. The optional components are given as pointers, null meaning that the current entity does not have that type of component.

```cpp
for (auto [transform2D, optRenderRect, optScaling, optSharedTexturePtr, optUniqueTexturePtr] :
     ctx->engine->GetGroupView<Transform2D>(ECS::Optional<RenderRect, Scaling, SharedTexturePtr, UniqueTexturePtr>())) {

    Engine::Texture *texture = nullptr;

    if (optSharedTexturePtr) {
        texture = optSharedTexturePtr->get();
    } else if (optUniqueTexturePtr) {
        texture = optUniqueTexturePtr->get();
    } else {
        continue;
    }

    RenderRect renderRect = (Engine::Rectangle) * texture;

    if (optRenderRect) {
        renderRect = *optRenderRect;
    }

    Scaling scaling;

    if (optScaling) {
        scaling = *optScaling;
    }

    ctx->window->GetRenderer()->DrawTexture(renderRect,
                                            texture,
                                            Engine::Rectangle(transform2D.x, transform2D.y, scaling.x * (float)renderRect.w, scaling.y * (float)renderRect.h));
}
```

#### Group View with Unused

Unused means that the filtering also takes into consideration the unused components as if they were mandatory but it does not iterate over the component containers.

```cpp
for (auto [transform2D, renderRect, scaling] : ctx->engine->GetGroupView<Transform2D, RenderRect, Scaling>(ECS::Unused<PlayerMovement>()))
```

#### Group View Mixed

Mandatory, Optional and Unused can be used together for more complex filtering.

```cpp
for (auto [transform2D, velocity2D, scaling, optCollider] :
        ctx->engine->GetGroupView<Transform2D, Velocity2D, Scaling>(ECS::Optional<Collider>(), ECS::Unused<VelocityMoved>())) {
```

### ComponentManager
Component Manager is a templated container that stores all Components of ComponentT type. It stores the components in a `std::vector` in order to keep data in contiguous memory and maximize cache hits. Component Manager keeps the vector sorted by entity id in order to have `O(log vector::size)` complexity per find. The complexity for adding and removing components is `O(vector::size log vector::size)` because the vector must be sorted afterwards.

#### ComponentManagerView
ComponentManagerView represents a subset of components specified through a sorted vector of entity ids. It instantiates a ComponentIterator that can be used to loop through components using a given strategy. ComponentIterator keeps track of the position in the container and only searches from that point onwards, because the components are sorted by entity id and the subset is sorted by entity id too.

For example, when a ComponentManagerView is created for a small subset, that means that the elements in the subset are sparse in the full set, so a binary search strategy will yield better results as the gap between one element and the next will be longer.

When the subset is large, the elements in the subset are dense in the full set, so a linear search approach may be faster because of caching and branch prediction. The gap between two elements is shorter so doing a binary search might be more inefficient than just iterating lineary.

For a subset of length `M` and a full set of length `N`, the complexity of using binary search for M elements is `O(M * log N)` and the complexity of the linear search is `O(N)`, because the subset is also sorted. When `M` is approaching `N`, `O(M * log N)` gets worse than `O(N)` complexity wise, but it also gets worse because of the trashing of the cache and difficulty in branch prediction.

#### ComponentGroupView
ComponentGroupView is a class that holds multiple ComponentManagerViews with the same entity subset. ComponentGroupIterator is a custom iterator that holds a tuple of ComponentIterators and keeps them in sync. For mandatory components the elements are returned as references and for optional components the elements are returned as pointers.

### Systems
Systems are callbacks that get called in the order of registration. Systems can be registered as anonymous functions or as objects of classes that implement SystemInterface.
Ideally, in an ECS engine, systems would be stateless, but in this implementation stateful systems are allowed too.

#### Lambda system example

```cpp
// Lambda registration
engine->Register([ctx]() {
      for (auto [entity, transform2D, velocity] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, Velocity2D>()) {
          NewTransform2D newTransform = transform2D;
          newTransform += (*ctx->dt) * velocity;
          ctx->engine->AddComponent(entity, newTransform);
      }
});
```

#### System interface example

```cpp
// Stateless system definition
StatelessSystem(TextureRendererSystem, GameContext);

// Expands to
class TextureRendererSystem : public ECS::SystemInterface {
private:
    GameContext *ctx;

public:
    TextureRendererSystem(GameContext *ctx):
        ctx(ctx) {}

    virtual void Update() override;
}

// Update function
void TextureRendererSystem::Update() {
    for (auto [transform2D, optRenderRect, optScaling, optSharedTexturePtr, optUniqueTexturePtr] :
         ctx->engine->GetGroupView<Transform2D>(ECS::Optional<RenderRect, Scaling, SharedTexturePtr, UniqueTexturePtr>())) {

        Engine::Texture *texture = nullptr;

        if (optSharedTexturePtr) {
            texture = optSharedTexturePtr->get();
        } else if (optUniqueTexturePtr) {
            texture = optUniqueTexturePtr->get();
        } else {
            continue;
        }

        RenderRect renderRect = (Engine::Rectangle) * texture;

        if (optRenderRect) {
            renderRect = *optRenderRect;
        }

        Scaling scaling;

        if (optScaling) {
            scaling = *optScaling;
        }

        ctx->window->GetRenderer()->DrawTexture(renderRect,
                                                texture,
                                                Engine::Rectangle(transform2D.x, transform2D.y, scaling.x * (float) renderRect.w, scaling.y * (float) renderRect.h));
    }
}

// Registration
engine->Register(new TextureRendererSystem(&gameContext));
```

## Game Engine
The game engine has 2 parts: the SDL2 wrappers and the Dynamic Loader. The SDL2 wrappers can be used to quickly get a game up and running, but the ECS engine is not dependant on it, so any graphics library can be used. The Dynamic Loader can be used to load dynamic libraries at runtime.

### Dynamic Loader
Dynamic Loader is used for loading and managing dynamic libraries. It can be used for running 'one time' scripts (for example, running commands by writing C++ code, compiling it to a dynamic library and load it in the game, all while the game is still running) or for loading new functionality such as Systems, Components, etc. For example, new Component Managers <ComponentType>, or new Systems can be generated and registered in the ECS engine.

The drawbacks of this approach is that once a script is loaded and code from that dynamic library is registered in the ECS engine, the dynamic library can not be easily unloaded. For example, if a Component Manager was generated in the script and registered in the ECS engine, once the dynamic library where that code resides is unloaded, the application will crash as the code is no longer loaded into memory.

```cpp
// Context class generation
EVAL(CREATE_CONTEXT(GameContext, (DynamicLoader::DynamicScriptManager<GameContext>, dynamicLoader), (ECS::Engine<ECS::DefaultEngineCore>, engine), (ResourceManagerT, resourceManager), (Engine::Window, window), (float, dt)))

// Objects construction
DynamicLoader::DynamicScriptManager<GameContext> dynamicScriptManager;
GameContext gameContext(&dynamicScriptManager, engine, &EngineWrapper::resourceManager, window, &dt);
dynamicScriptManager.SetContext(&gameContext);

// Loading a script and registering it as a system
auto playerMovementScript = dynamicScriptManager.Load("script1.dll", "CreatePlayerMovementScript");
engine->Register([&playerMovementScript, &gameContext]() {
    if (auto script = playerMovementScript.lock()) {
        script->Run();
    } else {
        LOG_INFO("debug", "Script unloaded");
    }
});

// Loading a script and running it
auto scriptWeak = dynamicScriptManager.Load("script2.dll", "CreateScript1");
if (auto script = scriptWeak.lock()) {
    script->Run();
}
```

## Game Sample
https://github.com/PixelRetroGames/NewECSEngine/blob/1e4bb9f9ad6b71a1088f4045cdf7f05be0368786/samples/pong/pong.cpp

## Samples
#### Pong
https://github.com/user-attachments/assets/ddfad047-ac82-49cd-954b-9464be5dc8d3

#### Physics Simulation
https://github.com/user-attachments/assets/3d53d1fb-ea2b-4e5f-b9ea-7626d13849c3
