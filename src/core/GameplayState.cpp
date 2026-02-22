#include "core/GameplayState.h"
#include "core/Game.h"
#include "core/PausedState.h"
#include <SDL3/SDL.h>
#include "systems/RenderSystem.h"
#include "systems/MovementSystem.h"
#include "systems/CameraSystem.h"
#include "systems/TilemapSystem.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "components/PlayerComponent.h"
#include "components/CameraComponent.h"
#include "components/SpriteComponent.h"
#include "components/TilemapComponent.h"

namespace Core {

GameplayState::GameplayState(int width, int height)
    : m_width(width), m_height(height) {
}

void GameplayState::OnEnter() {
}

void GameplayState::OnExit() {
}

void GameplayState::InitEntities(Game& game) {
    m_textureManager = game.GetTextureManager();

    // Player entity
    ECS::Entity playerEntity = m_world.CreateEntity();
    m_world.AddComponent<Components::TransformComponent>(playerEntity, {100.0f, 100.0f});
    m_world.AddComponent<Components::PlayerComponent>(playerEntity, {});

    SDL_Texture* playerTex = m_textureManager->Load("assets/Idle.png");
    if (playerTex) {
        m_world.AddComponent<Components::SpriteComponent>(playerEntity, {playerTex, nullptr});
    } else {
        m_world.AddComponent<Components::RenderComponent>(playerEntity, {16, 16, 255, 100, 100});
    }

    SDL_Log("Player entity: %u", playerEntity);

    // Tilemap entity
    ECS::Entity tilemapEntity = m_world.CreateEntity();
    SDL_Texture* tilesetTex = m_textureManager->Load("assets/tileset.png");

    Components::TilemapComponent tilemap;
    tilemap.tileset        = tilesetTex;
    tilemap.tileWidth      = 16;
    tilemap.tileHeight     = 16;
    tilemap.tilesetColumns = 8;

    constexpr i32 mapRows = 20;
    constexpr i32 mapCols = 20;
    tilemap.grid.resize(mapRows, std::vector<Components::Tile>(mapCols));

    for (i32 row = 0; row < mapRows; ++row) {
        for (i32 col = 0; col < mapCols; ++col) {
            bool isBorder = (row == 0 || row == mapRows - 1 ||
                             col == 0 || col == mapCols - 1);
            tilemap.grid[row][col] = { isBorder ? 1 : 0, !isBorder };
        }
    }

    m_world.AddComponent<Components::TilemapComponent>(tilemapEntity, std::move(tilemap));
    SDL_Log("Tilemap entity: %u", tilemapEntity);

    // Camera entity
    ECS::Entity cameraEntity = m_world.CreateEntity();
    m_world.AddComponent<Components::CameraComponent>(cameraEntity, {
        0.0f, 0.0f,
        m_width, m_height
    });
    SDL_Log("Camera entity: %u", cameraEntity);
}

void GameplayState::InitSystems(Game& game) {
    SDL_Renderer* renderer = game.GetRenderer();

    m_world.AddUpdateSystem(std::make_unique<Systems::MovementSystem>(game.GetInput(), 200.0f));
    m_world.AddUpdateSystem(std::make_unique<Systems::CameraSystem>());
    m_world.AddRenderSystem(std::make_unique<Systems::TilemapSystem>(renderer));
    m_world.AddRenderSystem(std::make_unique<Systems::RenderSystem>(renderer));
}

void GameplayState::ProcessInput(Game& game) {
    if (game.GetInput().IsKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
        game.PushState(std::make_unique<PausedState>());
    }
}

void GameplayState::Update(Game& game, float deltaTime) {
    m_world.UpdateSystems(deltaTime);
}

void GameplayState::DrawScene(Game& game) {
    SDL_Renderer* renderer = game.GetRenderer();
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);
    m_world.RenderSystems(0.0f);
}

void GameplayState::Render(Game& game) {
    DrawScene(game);
    SDL_RenderPresent(game.GetRenderer());
}

} // namespace Core