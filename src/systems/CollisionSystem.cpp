#include "systems/CollisionSystem.h"
#include "ecs/World.h"
#include "components/TransformComponent.h"
#include "components/CollisionComponent.h"
#include "components/TilemapComponent.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace Systems {

namespace {

inline i32 WorldToTileFloor(float coordinate, i32 tileSize) {
    return static_cast<i32>(std::floor(coordinate / static_cast<float>(tileSize)));
}

} // namespace

bool CollisionSystem::OverlapsNonWalkable(
    float x, float y, float width, float height,
    const Components::TilemapComponent& tilemap) const {

    i32 left   = WorldToTileFloor(x, tilemap.tileWidth);
    i32 right  = WorldToTileFloor(x + width - 1.0f, tilemap.tileWidth);
    i32 top    = WorldToTileFloor(y, tilemap.tileHeight);
    i32 bottom = WorldToTileFloor(y + height - 1.0f, tilemap.tileHeight);

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
    (void)deltaTime;
    auto& collisions = world.GetStore<Components::CollisionComponent>();
    auto& tilemaps   = world.GetStore<Components::TilemapComponent>();

    Components::TilemapComponent* tilemap = nullptr;
    for (auto& [entity, tm] : tilemaps.GetAll()) {
        (void)entity;
        tilemap = &tm;
        break;
    }
    if (!tilemap) return;
    if (tilemap->grid.empty() || tilemap->grid[0].empty()) return;

    const i32 mapCols = static_cast<i32>(tilemap->grid[0].size());
    static bool loggedRaggedRowsIssue = false;
    for (const auto& row : tilemap->grid) {
        if (static_cast<i32>(row.size()) != mapCols) {
            if (!loggedRaggedRowsIssue) {
                SDL_Log("CollisionSystem: tilemap has ragged rows; collision skipped.");
                loggedRaggedRowsIssue = true;
            }
            return;
        }
    }
    loggedRaggedRowsIssue = false;

    for (ECS::Entity entity : world.View<Components::CollisionComponent, Components::TransformComponent>()) {
        auto& collision = collisions.Get(entity);
        auto& transform = world.GetComponent<Components::TransformComponent>(entity);

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
