#pragma once

namespace Core {

class Game;

class GameState {
public:
    virtual ~GameState() = default;

    virtual void OnEnter()  {}
    virtual void OnExit()   {}

    virtual void ProcessInput(Game& game) = 0;
    virtual void Update(Game& game, float deltaTime) = 0;
    virtual void Render(Game& game, float interpolationAlpha) = 0;

    // Optional — override to draw without clear/present.
    // Used by overlay states like PausedState to composite on top.
    virtual void DrawScene(Game& game, float interpolationAlpha) { Render(game, interpolationAlpha); }
};

} // namespace Core
