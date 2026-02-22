#pragma once
#include "Window.h"
#include "Types.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "GameState.h"
#include <memory>
#include <vector>
#include <string>

namespace Core {

class Game {
public:
    Game(const std::string& title, i32 width, i32 height);
    ~Game() = default;

    void Run();

    // State stack management
    void PushState(std::unique_ptr<GameState> state);
    void PopState();

    // Accessors for states
    InputManager&   GetInput();
    TextureManager* GetTextureManager();
    SDL_Renderer*   GetRenderer();
    GameState*      GetStateBelow();
    i32             GetWindowWidth()  const;
    i32             GetWindowHeight() const;

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    std::unique_ptr<Window>                 m_window;
    std::unique_ptr<TextureManager>         m_textureManager;
    InputManager                            m_input;
    std::vector<std::unique_ptr<GameState>> m_states;
    i32                                     m_width;
    i32                                     m_height;
    bool                                    m_running = false;
};

} // namespace Core