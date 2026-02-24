#pragma once

#include "components/TilemapComponent.h"
#include "core/TextureManager.h"
#include <string>

namespace Core {

// Loads a Tiled JSON map file into a TilemapComponent.
// Expects two layers named "ground" and "collision".
// Ground layer populates the tile ID grid.
// Collision layer populates blockedTiles — any non-zero tile index marks a blocked position.
// Returns false and logs on any failure — component will be partially or fully empty.
class TilemapLoader {
public:
    static bool Load(
        const std::string&            path,
        TextureManager*               textureManager,
        Components::TilemapComponent& out
    );
};

} // namespace Core