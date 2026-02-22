#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

namespace Core {

// Loads and caches SDL textures by file path.
// Owns all textures — they are destroyed when TextureManager is destroyed.
// This is the foundation for the full resource manager we'll build later.
class TextureManager {
public:
    explicit TextureManager(SDL_Renderer* renderer);
    ~TextureManager();

    // Non-copyable — owns SDL resources.
    TextureManager(const TextureManager&)            = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // Returns a cached texture if already loaded, otherwise loads from disk.
    // Returns nullptr if the file cannot be loaded.
    SDL_Texture* Load(const std::string& path);

    void Unload(const std::string& path);
    void UnloadAll();

private:
    SDL_Renderer*                                m_renderer;
    std::unordered_map<std::string, SDL_Texture*> m_textures;
};

} // namespace Core