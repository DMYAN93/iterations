#pragma once
#include "core/Window.h"
#include "core/Types.h"
#include "core/InputManager.h"
#include "core/TextureManager.h"
#include "core/GameState.h"
#include "core/SettingsManager.h"
#include "core/DebugRenderer.h"
#include <memory>
#include <vector>
#include <string>

namespace Core {

class Game {
public:
    explicit Game(SettingsManager settings);
    ~Game() = default;

    void Run();

    void RequestPushState(std::unique_ptr<GameState> state);
    void RequestPopState();
    void RequestReplaceState(std::unique_ptr<GameState> state);
    void Quit();

    InputManager&          GetInput();
    TextureManager*        GetTextureManager();
    SDL_Renderer*          GetRenderer();
    GameState*             GetStateBelow();
    const SettingsManager& GetSettings() const;
    DebugRenderer&         GetDebugRenderer();
    i32                    GetWindowWidth()  const;
    i32                    GetWindowHeight() const;

private:
    enum class StateCommandType {
        Push,
        Pop,
        Replace
    };

    struct StateCommand {
        StateCommandType              type;
        std::unique_ptr<GameState> pendingState;
    };

    void PumpEvents();
    void ProcessInput();
    void Update(float deltaTime);
    void Render(float interpolationAlpha, float frameDeltaTime);
    void ApplyPendingStateCommands();

    // Declaration order is load-bearing:
    // m_settings first — no dependencies.
    // m_window second — reads settings, inits SDL and renderer.
    // m_textureManager third — needs renderer.
    // m_debugRenderer fourth — needs renderer and settings.
    // m_input last — safe to construct any time.
    SettingsManager                         m_settings;
    std::unique_ptr<Window>                 m_window;
    std::unique_ptr<TextureManager>         m_textureManager;
    std::unique_ptr<DebugRenderer>          m_debugRenderer;
    InputManager                            m_input;
    std::vector<std::unique_ptr<GameState>> m_states;
    std::vector<StateCommand>               m_pendingStateCommands;
    bool                                    m_running = false;
};

} // namespace Core
