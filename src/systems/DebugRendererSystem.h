#pragma once
#include "ecs/System.h"
#include "core/DebugRenderer.h"
#include "core/SettingsManager.h"
#include <SDL3/SDL.h>

namespace Systems {

// Handles all debug visualisation — hitboxes, sprite bounds, tile overlays.
// The only system that knows about DebugRenderer. Registered last in the
// render system list so debug draws appear on top of everything else.
class DebugRenderSystem : public ECS::System {
public:
    DebugRenderSystem(SDL_Renderer* renderer, Core::DebugRenderer& debugRenderer, const Core::SettingsManager& settings);
    void Update(ECS::World& world, float deltaTime) override;

private:
    SDL_Renderer*                m_renderer;
    Core::DebugRenderer&         m_debugRenderer;
    const Core::SettingsManager& m_settings;
};

} // namespace Systems