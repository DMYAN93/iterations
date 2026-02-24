#include "World.h"

namespace ECS {

Entity World::CreateEntity() {
    return m_nextEntity++;
}

void World::DestroyEntity(Entity entity) {
    for (auto& [key, store] : m_stores) {
        (void)key;
        store->RemoveEntity(entity);
    }
}

void World::AddUpdateSystem(std::unique_ptr<System> system) {
    m_updateSystems.push_back(std::move(system));
}

void World::AddRenderSystem(std::unique_ptr<System> system) {
    m_renderSystems.push_back(std::move(system));
}

void World::UpdateSystems(float deltaTime) {
    for (auto& system : m_updateSystems) {
        system->Update(*this, deltaTime);
    }
}

void World::RenderSystems(float deltaTime) {
    for (auto& system : m_renderSystems) {
        system->Update(*this, deltaTime);
    }
}

} // namespace ECS
