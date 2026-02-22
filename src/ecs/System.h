#pragma once

namespace ECS {

class World;

class System {
public:
    virtual ~System() = default;
    virtual void Update(World& world, float deltaTime) = 0;
};

} // namespace ECS