#include "texture_renderer_system.h"

namespace SpaceShooter {
void TextureRendererSystem::Update() {
    for (const auto &[transform2D, optRenderRect, optScaling, optSharedTexturePtr, optUniqueTexturePtr] : engine->GetGroupView<Transform2D>(ECS::Optional<RenderRect, Scaling, SharedTexturePtr, UniqueTexturePtr>())) {
        Engine::Texture *texture = nullptr;
        if (optSharedTexturePtr) {
            texture = optSharedTexturePtr->get();
        } else if (optUniqueTexturePtr) {
            texture = optUniqueTexturePtr->get();
        } else {
            continue;
        }

        RenderRect renderRect = (Engine::Rectangle)*texture;
        if (optRenderRect) {
            renderRect = *optRenderRect;
        }

        Scaling scaling;
        if (optScaling) {
            scaling = *optScaling;
        }

        window->GetRenderer()->DrawTexture(renderRect,
                                           texture,
                                           Engine::Rectangle(transform2D.x, transform2D.y, scaling.x * (float)renderRect.w, scaling.y * (float)renderRect.h));
    }
}
};
