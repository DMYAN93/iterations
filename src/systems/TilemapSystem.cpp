#include "systems/TilemapSystem.h"
#include "ecs/World.h"
#include "components/TilemapComponent.h"
#include "components/CameraComponent.h"

namespace Systems {

TilemapSystem::TilemapSystem(SDL_Renderer* renderer)
    : m_renderer(renderer) {
}

void TilemapSystem::Update(ECS::World& world, float deltaTime) {
    (void)deltaTime;
    auto& tilemaps = world.GetStore<Components::TilemapComponent>();
    auto& cameras  = world.GetStore<Components::CameraComponent>();

    f32 cameraX = 0.0f;
    f32 cameraY = 0.0f;
    const auto cameraEntities = world.View<Components::CameraComponent>();
    static bool loggedCameraCountIssue = false;
    if (cameraEntities.size() == 1) {
        const auto& camera = cameras.Get(cameraEntities.front());
        cameraX = camera.x;
        cameraY = camera.y;
        loggedCameraCountIssue = false;
    } else if (!loggedCameraCountIssue) {
        SDL_Log("TilemapSystem: expected exactly 1 camera, found %zu. Using world-origin fallback camera.", cameraEntities.size());
        loggedCameraCountIssue = true;
    }

    for (auto& [entity, tilemap] : tilemaps.GetAll()) {
        if (!tilemap.tileset)            continue;
        if (tilemap.tilesetColumns <= 0) continue;

        for (i32 row = 0; row < static_cast<i32>(tilemap.grid.size()); ++row) {
            for (i32 col = 0; col < static_cast<i32>(tilemap.grid[row].size()); ++col) {
                const Components::Tile& tile = tilemap.grid[row][col];
                if (tile.id < 0) continue;

                i32 srcCol = tile.id % tilemap.tilesetColumns;
                i32 srcRow = tile.id / tilemap.tilesetColumns;

                SDL_FRect src {
                    static_cast<float>(srcCol * tilemap.tileWidth),
                    static_cast<float>(srcRow * tilemap.tileHeight),
                    static_cast<float>(tilemap.tileWidth),
                    static_cast<float>(tilemap.tileHeight)
                };

                SDL_FRect dst {
                    static_cast<float>(col * tilemap.tileWidth)  - cameraX,
                    static_cast<float>(row * tilemap.tileHeight) - cameraY,
                    static_cast<float>(tilemap.tileWidth),
                    static_cast<float>(tilemap.tileHeight)
                };

                SDL_RenderTexture(m_renderer, tilemap.tileset, &src, &dst);
            }
        }
    }
}

} // namespace Systems
