# Entity Component System Game Engine

## Entity Component System Engine
### Entities and Components
Entities represent unique identifiers.

Components are structs that represent attributes of entities. Entities can have multiple components, but only one of each type of component.

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
`[entity, transform2D, newTransform]` are references to Components, entity is a component with the id of the entity.

```cpp
for (auto [entity, transform2D, newTransform] : ctx->engine->GetGroupView<ECS::Entity, Transform2D, NewTransform2D>()) {
    transform2D = newTransform;
    ctx->engine->DeleteComponents<NewTransform2D>(entity);
}
```

#### Group View with Optionals

In this example the filter means: `Transform2D` is mandatory and `transform2D` is a reference to the `Transform2D` component, and `ECS::Optional<RenderRect, Scaling, SharedTexturePtr, UniqueTexturePtr>()` means that the rest of the components are optional. The optional components are pointers, null means that the entity does not have that component type of component.

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

#### Group View mixed

Mandatory, Optional and Unused can be used together for more complex filtering.

```cpp
for (auto [transform2D, velocity2D, scaling, optCollider] :
        ctx->engine->GetGroupView<Transform2D, Velocity2D, Scaling>(ECS::Optional<Collider>(), ECS::Unused<VelocityMoved>())) {
```


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

## Game Sample
https://github.com/PixelRetroGames/NewECSEngine/blob/1e4bb9f9ad6b71a1088f4045cdf7f05be0368786/samples/pong/pong.cpp

## Samples
#### Pong
https://github.com/user-attachments/assets/ddfad047-ac82-49cd-954b-9464be5dc8d3

#### Physics Simulation
https://github.com/user-attachments/assets/3d53d1fb-ea2b-4e5f-b9ea-7626d13849c3
