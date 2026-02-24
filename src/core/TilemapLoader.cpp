#include "core/TilemapLoader.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <SDL3/SDL.h>

using json = nlohmann::json;

namespace Core {

bool TilemapLoader::Load(
    const std::string&            path,
    TextureManager*               textureManager,
    Components::TilemapComponent& out) {

    std::ifstream file(path);
    if (!file.is_open()) {
        SDL_Log("TilemapLoader: could not open '%s'.", path.c_str());
        return false;
    }

    json mapData;
    try {
        file >> mapData;
    } catch (const json::parse_error& err) {
        SDL_Log("TilemapLoader: parse error in '%s': %s", path.c_str(), err.what());
        return false;
    }

    int mapCols = mapData.value("width",      0);
    int mapRows = mapData.value("height",     0);
    int tileW   = mapData.value("tilewidth",  16);
    int tileH   = mapData.value("tileheight", 16);

    if (mapCols <= 0 || mapRows <= 0) {
        SDL_Log("TilemapLoader: invalid map dimensions in '%s'.", path.c_str());
        return false;
    }

    out.tileWidth  = tileW;
    out.tileHeight = tileH;

    // Resize grid — all tile IDs default to 0.
    out.grid.assign(mapRows, std::vector<Components::Tile>(mapCols));

    // Read tileset path and tilesetColumns from the first tileset entry.
    if (mapData.contains("tilesets") && !mapData["tilesets"].empty()) {
        auto& tilesetEntry = mapData["tilesets"][0];
        std::string tilesetPath = tilesetEntry.value("image", "");
        out.tilesetColumns      = tilesetEntry.value("columns", 0);

        if (!tilesetPath.empty()) {
            out.tileset = textureManager->Load(tilesetPath);
            if (!out.tileset) {
                SDL_Log("TilemapLoader: failed to load tileset texture '%s'.", tilesetPath.c_str());
            }
        }
    }

    // Parse layers.
    if (!mapData.contains("layers")) {
        SDL_Log("TilemapLoader: no layers found in '%s'.", path.c_str());
        return false;
    }

    for (auto& layer : mapData["layers"]) {
        std::string name = layer.value("name", "");
        std::string type = layer.value("type", "");

        if (type != "tilelayer") continue;
        if (!layer.contains("data"))  continue;

        auto& tileData = layer["data"];

        if (name == "ground") {
            for (int index = 0; index < static_cast<int>(tileData.size()); ++index) {
                int gid = tileData[index].get<int>();
                int row = index / mapCols;
                int col = index % mapCols;
                if (row < mapRows && col < mapCols) {
                    // Tiled GIDs are 1-based — subtract 1 to get 0-based tile ID.
                    // GID 0 means empty tile.
                    out.grid[row][col].id = (gid > 0) ? (gid - 1) : 0;
                }
            }
            SDL_Log("TilemapLoader: loaded ground layer (%dx%d).", mapCols, mapRows);
        }
        else if (name == "collision") {
            for (int index = 0; index < static_cast<int>(tileData.size()); ++index) {
                int gid = tileData[index].get<int>();
                if (gid != 0) {
                    out.blockedTiles.insert(index);
                }
            }
            SDL_Log("TilemapLoader: loaded collision layer (%zu blocked tiles).",
                out.blockedTiles.size());
        }
    }

    SDL_Log("TilemapLoader: loaded '%s' (%dx%d tiles).", path.c_str(), mapCols, mapRows);
    return true;
}

} // namespace Core