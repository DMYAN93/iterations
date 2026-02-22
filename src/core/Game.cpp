#include "Game.h"
#include <SDL3/SDL.h>
#include "systems/RenderSystem.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"

namespace Core {

Game::Game(const std::string& title, i32 width, i32 height) {
    m_window = std::make_unique<Window>(title, width, height);
    if (!m_window->IsValid()) {
        SDL_Log("Failed to initialize game window.");
        return;
    }

    // Create a test entity
    ECS::Entity e = m_world.CreateEntity();
    m_world.AddComponent<Components::TransformComponent>(e, {100.0f, 100.0f});
    m_world.AddComponent<Components::RenderComponent>(e, {50, 50, 255, 100, 100});

    // Register systems
    m_world.AddRenderSystem(std::make_unique<Systems::RenderSystem>(m_window->GetRenderer()));

    m_running = true;
}

void Game::Run() {
    u32 lastTime = SDL_GetTicks();

    while (m_running) {
        u32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_running = false;
        }
    }
}

void Game::Update(float deltaTime) {
    m_world.UpdateSystems(deltaTime);
}

void Game::Render() {
    SDL_Renderer* renderer = m_window->GetRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    m_world.RenderSystems(0.0f);
    SDL_RenderPresent(renderer);
}

} // namespace Core