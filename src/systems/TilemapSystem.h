#pragma once
#include "ecs/System.h"
#include <SDL3/SDL.h>

namespace Systems {

class TilemapSystem : public ECS::System {
public:
    TilemapSystem(SDL_Renderer* renderer);
    void Update(ECS::World& world, float deltaTime) override;

private:
    SDL_Renderer* m_renderer;
};

} // namespace Systems