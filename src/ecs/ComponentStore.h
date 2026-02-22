#pragma once
#include <unordered_map>
#include <optional>
#include "Entity.h"

namespace ECS {

template<typename T>
class ComponentStore {
public:
    void Add(Entity entity, T component) {
        m_components[entity] = component;
    }

    void Remove(Entity entity) {
        m_components.erase(entity);
    }

    bool Has(Entity entity) const {
        return m_components.count(entity) > 0;
    }

    T& Get(Entity entity) {
        return m_components.at(entity);
    }

    const T& Get(Entity entity) const {
        return m_components.at(entity);
    }

    std::optional<T*> TryGet(Entity entity) {
        auto it = m_components.find(entity);
        if (it == m_components.end()) return std::nullopt;
        return &it->second;
    }

    std::unordered_map<Entity, T>& GetAll() {
        return m_components;
    }

private:
    std::unordered_map<Entity, T> m_components;
};

} // namespace ECS