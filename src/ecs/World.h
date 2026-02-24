#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>
#include <tuple>
#include "Entity.h"
#include "ComponentStore.h"
#include "System.h"

namespace ECS {

class World {
public:
    Entity CreateEntity();
    void DestroyEntity(Entity entity);

    template<typename T>
    void AddComponent(Entity entity, T component) {
        GetOrCreateStore<T>().Add(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        GetOrCreateStore<T>().Remove(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity) {
        return GetOrCreateStore<T>().Has(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return GetOrCreateStore<T>().Get(entity);
    }

    template<typename T>
    ComponentStore<T>& GetStore() {
        return GetOrCreateStore<T>();
    }

    template<typename TPrimary, typename... TOthers>
    std::vector<Entity> View() {
        auto* primaryStore = FindStore<TPrimary>();
        if (!primaryStore) {
            return {};
        }

        std::tuple<ComponentStore<TOthers>*...> otherStores {FindStore<TOthers>()...};
        if constexpr (sizeof...(TOthers) > 0) {
            if (((std::get<ComponentStore<TOthers>*>(otherStores) == nullptr) || ...)) {
                return {};
            }
        }

        std::vector<Entity> entities;
        entities.reserve(primaryStore->GetAll().size());

        for (const auto& [entity, component] : primaryStore->GetAll()) {
            (void)component;
            if constexpr (sizeof...(TOthers) == 0) {
                entities.push_back(entity);
            } else if ((std::get<ComponentStore<TOthers>*>(otherStores)->Has(entity) && ...)) {
                entities.push_back(entity);
            }
        }

        return entities;
    }

    void AddUpdateSystem(std::unique_ptr<System> system);
    void AddRenderSystem(std::unique_ptr<System> system);
    void UpdateSystems(float deltaTime);
    void RenderSystems(float deltaTime);

private:
    template<typename T>
    ComponentStore<T>& GetOrCreateStore() {
        auto key = std::type_index(typeid(T));
        auto it = m_stores.find(key);
        if (it == m_stores.end()) {
            auto store = std::make_unique<ComponentStore<T>>();
            auto* raw  = store.get();
            m_stores.emplace(key, std::move(store));
            return *raw;
        }

        return *static_cast<ComponentStore<T>*>(it->second.get());
    }

    template<typename T>
    ComponentStore<T>* FindStore() {
        auto key = std::type_index(typeid(T));
        auto it = m_stores.find(key);
        if (it == m_stores.end()) {
            return nullptr;
        }

        return static_cast<ComponentStore<T>*>(it->second.get());
    }

    Entity m_nextEntity = 1;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStore>> m_stores;
    std::vector<std::unique_ptr<System>> m_updateSystems;
    std::vector<std::unique_ptr<System>> m_renderSystems;
};

} // namespace ECS
