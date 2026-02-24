#include "systems/CameraSystem.h"
#include "ecs/World.h"
#include "components/CameraComponent.h"
#include "components/PlayerComponent.h"
#include "components/TransformComponent.h"
#include <SDL3/SDL.h>

namespace Systems {

void CameraSystem::Update(ECS::World& world, float deltaTime) {
    (void)deltaTime;
    auto& cameras = world.GetStore<Components::CameraComponent>();
    auto& transforms = world.GetStore<Components::TransformComponent>();

    const auto playerEntities = world.View<Components::PlayerComponent, Components::TransformComponent>();
    static bool loggedPlayerCountIssue = false;
    if (playerEntities.size() != 1) {
        if (!loggedPlayerCountIssue) {
            SDL_Log("CameraSystem: expected exactly 1 player with transform, found %zu.", playerEntities.size());
            loggedPlayerCountIssue = true;
        }
        return;
    }
    loggedPlayerCountIssue = false;

    const auto cameraEntities = world.View<Components::CameraComponent>();
    static bool loggedCameraCountIssue = false;
    if (cameraEntities.size() != 1) {
        if (!loggedCameraCountIssue) {
            SDL_Log("CameraSystem: expected exactly 1 camera, found %zu.", cameraEntities.size());
            loggedCameraCountIssue = true;
        }
        return;
    }
    loggedCameraCountIssue = false;

    const ECS::Entity playerEntity = playerEntities.front();
    const ECS::Entity cameraEntity = cameraEntities.front();

    auto& playerTransform = transforms.Get(playerEntity);
    auto& camera = cameras.Get(cameraEntity);
    camera.x = playerTransform.x - (camera.viewportWidth / 2.0f);
    camera.y = playerTransform.y - (camera.viewportHeight / 2.0f);
}

} // namespace Systems
