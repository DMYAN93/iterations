#include "core/Game.h"
#include "core/MenuState.h"
#include <SDL3/SDL.h>

namespace Core {

Game::Game(SettingsManager settings)
    : m_settings(std::move(settings)) {
    m_window = std::make_unique<Window>(
        m_settings.window.title,
        m_settings.window.width,
        m_settings.window.height,
        m_settings.graphics.vsync,
        m_settings.graphics.screenMode
    );

    if (!m_window->IsValid()) {
        SDL_Log("Failed to initialize game window.");
        return;
    }

    m_textureManager = std::make_unique<TextureManager>(m_window->GetRenderer());
    m_debugRenderer  = std::make_unique<DebugRenderer>(m_window->GetRenderer(), m_settings);

    auto menu = std::make_unique<MenuState>(
        m_settings.window.width,
        m_settings.window.height
    );
    PushState(std::move(menu));

    m_running = true;
}

void Game::Run() {
    u32 lastTime = SDL_GetTicks();

    while (m_running && !m_states.empty()) {
        u32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Clamp delta time to 100ms. Without this, pausing in the debugger or an OS
        // interruption produces an enormous delta on the next frame, causing everything
        // to teleport. Also prevents SDL_GetTicks u32 overflow (~49 days) from spiking.
        constexpr float k_maxDelta = 0.1f;
        deltaTime = (deltaTime < k_maxDelta) ? deltaTime : k_maxDelta;

        ProcessInput();
        m_input.BeginFrame();
        Update(deltaTime);
        Render(deltaTime);
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

void Game::Render(float deltaTime) {
    if (!m_states.empty()) {
        m_states.back()->Render(*this);
    }
    m_debugRenderer->DrawFPS(deltaTime);
    SDL_RenderPresent(m_window->GetRenderer());
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

void Game::Quit() {
    m_running = false;
}

InputManager&          Game::GetInput()          { return m_input; }
TextureManager*        Game::GetTextureManager() { return m_textureManager.get(); }
SDL_Renderer*          Game::GetRenderer()        { return m_window->GetRenderer(); }
DebugRenderer&         Game::GetDebugRenderer()   { return *m_debugRenderer; }
const SettingsManager& Game::GetSettings() const  { return m_settings; }
i32 Game::GetWindowWidth()  const                 { return m_settings.window.width; }
i32 Game::GetWindowHeight() const                 { return m_settings.window.height; }

GameState* Game::GetStateBelow() {
    if (m_states.size() < 2) return nullptr;
    return m_states[m_states.size() - 2].get();
}

} // namespace Core