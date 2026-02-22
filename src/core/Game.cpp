#include "Game.h"
#include <SDL3/SDL.h>
#include "GameplayState.h"

namespace Core {

Game::Game(const std::string& title, i32 width, i32 height)
    : m_width(width), m_height(height) {
    m_window = std::make_unique<Window>(title, width, height);
    if (!m_window->IsValid()) {
        SDL_Log("Failed to initialize game window.");
        return;
    }

    m_textureManager = std::make_unique<TextureManager>(m_window->GetRenderer());

    auto gameplay = std::make_unique<GameplayState>(width, height);
    gameplay->InitEntities(*this);
    gameplay->InitSystems(*this);
    PushState(std::move(gameplay));

    m_running = true;
}

void Game::Run() {
    u32 lastTime = SDL_GetTicks();

    while (m_running && !m_states.empty()) {
        u32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        m_input.BeginFrame();

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

    if (!m_states.empty()) {
        m_states.back()->ProcessInput(*this);
    }
}

void Game::Update(float deltaTime) {
    if (!m_states.empty()) {
        m_states.back()->Update(*this, deltaTime);
    }
}

void Game::Render() {
    if (!m_states.empty()) {
        m_states.back()->Render(*this);
    }
}

void Game::PushState(std::unique_ptr<GameState> state) {
    if (!m_states.empty()) {
        m_states.back()->OnExit();
    }
    m_states.push_back(std::move(state));
    m_states.back()->OnEnter();
}

void Game::PopState() {
    if (m_states.empty()) return;
    m_states.back()->OnExit();
    m_states.pop_back();
    if (!m_states.empty()) {
        m_states.back()->OnEnter();
    }
}

InputManager& Game::GetInput() {
    return m_input;
}

TextureManager* Game::GetTextureManager() {
    return m_textureManager.get();
}

SDL_Renderer* Game::GetRenderer() {
    return m_window->GetRenderer();
}

GameState* Game::GetStateBelow() {
    if (m_states.size() < 2) return nullptr;
    return m_states[m_states.size() - 2].get();
}

i32 Game::GetWindowWidth() const  { return m_width; }
i32 Game::GetWindowHeight() const { return m_height; }

} // namespace Core