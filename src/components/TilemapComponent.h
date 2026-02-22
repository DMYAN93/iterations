#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "core/Types.h"

namespace Components {

struct Tile {
    i32  id       = 0;
    bool walkable = true;
};

struct TilemapComponent {
    std::vector<std::vector<Tile>> grid;
    SDL_Texture*                   tileset = nullptr;
    i32                            tileWidth  = 16;
    i32                            tileHeight = 16;
    i32                            tilesetColumns = 0;
};

} // namespace Components