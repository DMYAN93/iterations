#pragma once
#include "Window.h"
#include "Types.h"
#include "../ecs/World.h"
#include <memory>

namespace Core {

class Game {
public:
    Game(const std::string& title, i32 width, i32 height);
    ~Game() = default;

    void Run();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    std::unique_ptr<Window> m_window;
    ECS::World m_world;
    bool m_running = false;
};

} // namespace Core