#include "core/GameplayState.h"
#include "core/Game.h"
#include "core/PausedState.h"
#include "core/TilemapLoader.h"
#include <SDL3/SDL.h>
#include "systems/DebugRendererSystem.h"
#include "systems/RenderSystem.h"
#include "systems/MovementSystem.h"
#include "systems/CollisionSystem.h"
#include "systems/CameraSystem.h"
#include "systems/TilemapSystem.h"
#include "systems/AnimationSystem.h"
#include "components/CollisionComponent.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "components/PlayerComponent.h"
#include "components/CameraComponent.h"
#include "components/SpriteComponent.h"
#include "components/TilemapComponent.h"
#include "components/AnimationComponent.h"

namespace Core {

GameplayState::GameplayState(int width, int height)
    : m_width(width), m_height(height) {
}

void GameplayState::OnEnter() {
    SDL_Log("GameplayState: entered.");
}

void GameplayState::OnExit() {
    SDL_Log("GameplayState: exited.");
}

void GameplayState::InitEntities(Game& game) {
    m_textureManager = game.GetTextureManager();

    // Player entity
    ECS::Entity playerEntity = m_world.CreateEntity();
    m_world.AddComponent<Components::TransformComponent>(playerEntity, {100.0f, 100.0f});
    m_world.AddComponent<Components::PlayerComponent>(playerEntity, {});
    m_world.AddComponent<Components::CollisionComponent>(playerEntity, {
        0.0f,  // offsetX — centered on transform
        0.0f,  // offsetY — centered on transform
        12.0f, // width — tighter than the 64px sprite
        12.0f  // height — adjust once you see the debug rect in game
    });

    SDL_Texture* playerTex = m_textureManager->Load("assets/Idle.png");
    if (playerTex) {
        m_world.AddComponent<Components::SpriteComponent>(playerEntity, {playerTex, {}, true});

        const float frameW     = 64.0f;
        const float frameH     = 64.0f;
        const int   frameCount = 2;

        Components::AnimationClip idleClip;
        idleClip.frameDuration = 0.15f;
        idleClip.looping       = true;

        for (int i = 0; i < frameCount; ++i) {
            idleClip.frames.push_back({ i * frameW, 0.0f, frameW, frameH });
        }

        Components::AnimationComponent anim;
        anim.clips["idle"] = std::move(idleClip);
        anim.currentClip   = "idle";
        anim.currentFrame  = 0;
        anim.elapsedTime   = 0.0f;

        m_world.AddComponent<Components::AnimationComponent>(playerEntity, std::move(anim));
    } else {
        m_world.AddComponent<Components::RenderComponent>(playerEntity, {16, 16, 255, 100, 100});
    }

    SDL_Log("GameplayState: player entity %u created.", playerEntity);

    // Tilemap entity — loaded from file
    ECS::Entity tilemapEntity = m_world.CreateEntity();
    Components::TilemapComponent tilemap;

    if (!TilemapLoader::Load("assets/map01.json", m_textureManager, tilemap)) {
        SDL_Log("GameplayState: failed to load map, tilemap will be empty.");
    }

    m_world.AddComponent<Components::TilemapComponent>(tilemapEntity, std::move(tilemap));
    SDL_Log("GameplayState: tilemap entity %u created.", tilemapEntity);

    // Camera entity
    ECS::Entity cameraEntity = m_world.CreateEntity();
    m_world.AddComponent<Components::CameraComponent>(cameraEntity, {
        0.0f, 0.0f,
        m_width, m_height
    });
    SDL_Log("GameplayState: camera entity %u created.", cameraEntity);
}

void GameplayState::InitSystems(Game& game) {
    SDL_Renderer*        renderer      = game.GetRenderer();
    Core::DebugRenderer& debugRenderer = game.GetDebugRenderer();

    m_world.AddUpdateSystem(std::make_unique<Systems::MovementSystem>(
        game.GetInput(),
        game.GetSettings().gameplay.playerSpeed
    ));
    m_world.AddUpdateSystem(std::make_unique<Systems::CollisionSystem>());
    m_world.AddUpdateSystem(std::make_unique<Systems::CameraSystem>());
    m_world.AddUpdateSystem(std::make_unique<Systems::AnimationSystem>());
    m_world.AddRenderSystem(std::make_unique<Systems::TilemapSystem>(renderer));
    m_world.AddRenderSystem(std::make_unique<Systems::RenderSystem>(renderer));
    m_world.AddRenderSystem(std::make_unique<Systems::DebugRenderSystem>(
        renderer,
        debugRenderer,
        game.GetSettings()
    ));
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
}

} // namespace Core