#pragma once
#include "core/GameState.h"

namespace Core {

class PausedState : public GameState {
public:
    void OnEnter() override;
    void OnExit()  override;

    void ProcessInput(Game& game) override;
    void Update(Game& game, float deltaTime) override;
    void Render(Game& game, float interpolationAlpha) override;
};

} // namespace Core
