#include "core/MenuState.h"
#include "core/Game.h"
#include "core/GameplayState.h"
#include "core/InputManager.h"
#include <SDL3/SDL.h>

namespace Core {

MenuState::MenuState(i32 screenWidth, i32 screenHeight)
    : m_screenWidth(screenWidth)
    , m_screenHeight(screenHeight) {
    m_items = { "Start", "Quit" };
}

void MenuState::OnEnter() {
    SDL_Log("MenuState: entered.");
}

void MenuState::OnExit() {
    SDL_Log("MenuState: exited.");
    m_uiRenderer.reset();
}

void MenuState::ProcessInput(Game& game) {
    InputManager& input = game.GetInput();

    if (input.IsKeyJustPressed(SDL_SCANCODE_UP)) {
        m_selectedIndex = (m_selectedIndex - 1 + static_cast<i32>(m_items.size()))
                          % static_cast<i32>(m_items.size());
    }

    if (input.IsKeyJustPressed(SDL_SCANCODE_DOWN)) {
        m_selectedIndex = (m_selectedIndex + 1) % static_cast<i32>(m_items.size());
    }

    if (input.IsKeyJustPressed(SDL_SCANCODE_RETURN)) {
        SDL_Log("MenuState: selected '%s'.", m_items[m_selectedIndex].c_str());
        if (m_items[m_selectedIndex] == "Start") {
            auto gameplay = std::make_unique<GameplayState>(m_screenWidth, m_screenHeight);
            gameplay->InitEntities(game);
            gameplay->InitSystems(game);
            game.RequestPushState(std::move(gameplay));
        } else if (m_items[m_selectedIndex] == "Quit") {
            game.Quit();
        }
    }
}

void MenuState::Update(Game& game, float deltaTime) {
}

void MenuState::Render(Game& game, float interpolationAlpha) {
    (void)interpolationAlpha;

    SDL_Renderer* renderer = game.GetRenderer();
    SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255);
    SDL_RenderClear(renderer);

    if (!m_uiRenderer) {
        SDL_Log("MenuState: initialising UIRenderer.");
        m_uiRenderer = std::make_unique<UIRenderer>(
            renderer,
            "assets/JetBrainsMono-Regular.ttf",
            32,
            m_screenWidth
        );
    }

    if (!m_uiRenderer->IsValid()) return;

    const SDL_Color white    = { 255, 255, 255, 255 };
    const SDL_Color selected = { 255, 220,  50, 255 };

    i32 startY = m_screenHeight / 2 - 40;
    const i32 k_lineHeight = 60;

    for (i32 i = 0; i < static_cast<i32>(m_items.size()); ++i) {
        SDL_Color color = (i == m_selectedIndex) ? selected : white;
        m_uiRenderer->DrawTextCentered(m_items[i], startY + i * k_lineHeight, color);
    }
}

} // namespace Core
