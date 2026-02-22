#pragma once
#include "core/GameState.h"
#include "ecs/World.h"
#include "core/TextureManager.h"

namespace Core {

class GameplayState : public GameState {
public:
    GameplayState(int width, int height);

    void OnEnter() override;
    void OnExit()  override;

    void ProcessInput(Game& game) override;
    void Update(Game& game, float deltaTime) override;
    void Render(Game& game) override;
    void DrawScene(Game& game) override;

    void InitEntities(Game& game);
    void InitSystems(Game& game);

private:
    ECS::World           m_world;
    TextureManager*      m_textureManager = nullptr;
    int                  m_width;
    int                  m_height;
};

} // namespace Core