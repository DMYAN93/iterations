#include "core/PausedState.h"
#include "core/Game.h"
#include <SDL3/SDL.h>

namespace Core {

void PausedState::OnEnter() {
    SDL_Log("Game paused.");
}

void PausedState::OnExit() {
    SDL_Log("Game resumed.");
}

void PausedState::ProcessInput(Game& game) {
    if (game.GetInput().IsKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
        game.RequestPopState();
    }
}

void PausedState::Update(Game& game, float deltaTime) {
    // Intentionally empty — world is frozen while paused.
}

void PausedState::Render(Game& game, float interpolationAlpha) {
    GameState* below = game.GetStateBelow();
    if (below) {
        below->DrawScene(game, interpolationAlpha);
    }

    SDL_Renderer* renderer = game.GetRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_FRect overlay {
        0, 0,
        static_cast<float>(game.GetWindowWidth()),
        static_cast<float>(game.GetWindowHeight())
    };
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

} // namespace Core
