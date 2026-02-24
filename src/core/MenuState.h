#pragma once

#include "core/GameState.h"
#include "core/UIRenderer.h"
#include <vector>
#include <string>
#include <memory>

namespace Core {

class MenuState : public GameState {
public:
    MenuState(i32 screenWidth, i32 screenHeight);

    void OnEnter() override;
    void OnExit()  override;

    void ProcessInput(Game& game) override;
    void Update(Game& game, float deltaTime) override;
    void Render(Game& game, float interpolationAlpha) override;

private:
    i32 m_screenWidth  = 0;
    i32 m_screenHeight = 0;

    std::unique_ptr<UIRenderer> m_uiRenderer;

    std::vector<std::string> m_items;
    i32                      m_selectedIndex = 0;
};

} // namespace Core
