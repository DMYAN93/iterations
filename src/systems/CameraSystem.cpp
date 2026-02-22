#include "systems/CameraSystem.h"
#include "ecs/World.h"
#include "components/CameraComponent.h"
#include "components/PlayerComponent.h"
#include "components/TransformComponent.h"

namespace Systems {

void CameraSystem::Update(ECS::World& world, float deltaTime) {
    auto& cameras    = world.GetStore<Components::CameraComponent>();
    auto& players    = world.GetStore<Components::PlayerComponent>();
    auto& transforms = world.GetStore<Components::TransformComponent>();

    ECS::Entity playerEntity = ECS::NULL_ENTITY;
    for (auto& [entity, _] : players.GetAll()) {
        playerEntity = entity;
        break;
    }

    if (playerEntity == ECS::NULL_ENTITY) return;
    if (!transforms.Has(playerEntity))   return;

    auto& playerTransform = transforms.Get(playerEntity);

    for (auto& [entity, camera] : cameras.GetAll()) {
        camera.x = playerTransform.x - (camera.viewportWidth  / 2.0f);
        camera.y = playerTransform.y - (camera.viewportHeight / 2.0f);
    }
}

} // namespace Systems