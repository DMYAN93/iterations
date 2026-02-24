#include "core/Game.h"
#include "core/MenuState.h"
#include <SDL3/SDL.h>
#include <algorithm>

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
    u64 lastTicksNs = SDL_GetTicksNS();
    float accumulator = 0.0f;

    const int tickRateHz = std::max(1, m_settings.simulation.tickRateHz);
    const int maxSubstepsPerFrame = std::max(1, m_settings.simulation.maxSubstepsPerFrame);
    const float fixedDeltaTime = 1.0f / static_cast<float>(tickRateHz);
    const float maxFrameDelta = std::max(fixedDeltaTime, m_settings.simulation.maxFrameDelta);

    while (m_running && !m_states.empty()) {
        u64 currentTicksNs = SDL_GetTicksNS();
        float frameDeltaTime = static_cast<float>((currentTicksNs - lastTicksNs) / 1'000'000'000.0);
        lastTicksNs = currentTicksNs;
        frameDeltaTime = std::min(frameDeltaTime, maxFrameDelta);
        accumulator += frameDeltaTime;

        PumpEvents();
        m_input.BeginFrame();
        ProcessInput();

        int substepsProcessed = 0;
        while (accumulator >= fixedDeltaTime && substepsProcessed < maxSubstepsPerFrame) {
            Update(fixedDeltaTime);
            accumulator -= fixedDeltaTime;
            ++substepsProcessed;
        }

        if (substepsProcessed == maxSubstepsPerFrame && accumulator >= fixedDeltaTime) {
            accumulator = 0.0f;
        }

        const float interpolationAlpha = accumulator / fixedDeltaTime;
        Render(interpolationAlpha, frameDeltaTime);
    }
}

void Game::PumpEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_running = false;
        }
    }
}

void Game::ProcessInput() {
    if (!m_states.empty()) {
        m_states.back()->ProcessInput(*this);
    }
}

void Game::Update(float deltaTime) {
    if (!m_states.empty()) {
        m_states.back()->Update(*this, deltaTime);
    }
}

void Game::Render(float interpolationAlpha, float frameDeltaTime) {
    if (!m_states.empty()) {
        m_states.back()->Render(*this, interpolationAlpha);
    }
    m_debugRenderer->DrawFPS(frameDeltaTime);
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
