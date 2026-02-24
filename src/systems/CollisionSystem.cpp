#include "systems/CollisionSystem.h"
#include "ecs/World.h"
#include "components/TransformComponent.h"
#include "components/CollisionComponent.h"
#include "components/TilemapComponent.h"

namespace Systems {

bool CollisionSystem::OverlapsNonWalkable(
    float x, float y, float width, float height,
    const Components::TilemapComponent& tilemap) const {

    i32 left   = static_cast<i32>(x               / tilemap.tileWidth);
    i32 right  = static_cast<i32>((x + width - 1) / tilemap.tileWidth);
    i32 top    = static_cast<i32>(y               / tilemap.tileHeight);
    i32 bottom = static_cast<i32>((y + height - 1) / tilemap.tileHeight);

    i32 mapRows = static_cast<i32>(tilemap.grid.size());
    i32 mapCols = mapRows > 0 ? static_cast<i32>(tilemap.grid[0].size()) : 0;

    for (i32 row = top; row <= bottom; ++row) {
        for (i32 col = left; col <= right; ++col) {
            if (row < 0 || row >= mapRows || col < 0 || col >= mapCols) return true;
            int index = row * mapCols + col;
            if (tilemap.blockedTiles.count(index)) return true;
        }
    }

    return false;
}

void CollisionSystem::Update(ECS::World& world, float deltaTime) {
    auto& transforms = world.GetStore<Components::TransformComponent>();
    auto& collisions = world.GetStore<Components::CollisionComponent>();
    auto& tilemaps   = world.GetStore<Components::TilemapComponent>();

    Components::TilemapComponent* tilemap = nullptr;
    for (auto& [entity, tm] : tilemaps.GetAll()) {
        tilemap = &tm;
        break;
    }
    if (!tilemap) return;

    for (auto& [entity, collision] : collisions.GetAll()) {
        if (!transforms.Has(entity)) continue;

        auto& transform = transforms.Get(entity);

        float hitboxX = transform.x - (collision.width  / 2.0f) + collision.offsetX;
        float hitboxY = transform.y - (collision.height / 2.0f) + collision.offsetY;

        if (OverlapsNonWalkable(hitboxX, hitboxY, collision.width, collision.height, *tilemap)) {
            float prevHitboxY = transform.prevY - (collision.height / 2.0f) + collision.offsetY;
            if (!OverlapsNonWalkable(hitboxX, prevHitboxY, collision.width, collision.height, *tilemap)) {
                transform.y = transform.prevY;
            } else {
                float prevHitboxX = transform.prevX - (collision.width / 2.0f) + collision.offsetX;
                if (!OverlapsNonWalkable(prevHitboxX, hitboxY, collision.width, collision.height, *tilemap)) {
                    transform.x = transform.prevX;
                } else {
                    transform.x = transform.prevX;
                    transform.y = transform.prevY;
                }
            }
        }
    }
}

} // namespace Systems