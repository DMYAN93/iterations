#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <unordered_set>
#include "core/Types.h"

namespace Components {

struct Tile {
    i32 id = 0;
};

struct TilemapComponent {
    std::vector<std::vector<Tile>> grid;
    std::unordered_set<int>        blockedTiles;  // index = row * cols + col
    SDL_Texture*                   tileset        = nullptr;
    i32                            tileWidth      = 16;
    i32                            tileHeight     = 16;
    i32                            tilesetColumns = 0;
};

} // namespace Components