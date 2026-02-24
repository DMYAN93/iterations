#include "systems/AnimationSystem.h"
#include "ecs/World.h"
#include "components/AnimationComponent.h"
#include "components/SpriteComponent.h"

namespace Systems {

void AnimationSystem::Update(ECS::World& world, float deltaTime) {
    auto& animations = world.GetStore<Components::AnimationComponent>();
    auto& sprites    = world.GetStore<Components::SpriteComponent>();

    for (ECS::Entity entity : world.View<Components::AnimationComponent, Components::SpriteComponent>()) {
        auto& anim = animations.Get(entity);
        if (anim.currentClip.empty())              continue;

        auto it = anim.clips.find(anim.currentClip);
        if (it == anim.clips.end())                continue;

        Components::AnimationClip& clip = it->second;
        if (clip.frames.empty())                   continue;

        anim.elapsedTime += deltaTime;

        if (anim.elapsedTime >= clip.frameDuration) {
            anim.elapsedTime -= clip.frameDuration;
            anim.currentFrame++;

            if (anim.currentFrame >= static_cast<int>(clip.frames.size())) {
                anim.currentFrame = clip.looping ? 0 : static_cast<int>(clip.frames.size()) - 1;
            }
        }

        sprites.Get(entity).srcRect = clip.frames[anim.currentFrame];
    }
}

} // namespace Systems
