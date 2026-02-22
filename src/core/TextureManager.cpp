#include "core/TextureManager.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace Core {

TextureManager::TextureManager(SDL_Renderer* renderer)
    : m_renderer(renderer) {
}

TextureManager::~TextureManager() {
    UnloadAll();
}

SDL_Texture* TextureManager::Load(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());
    if (!texture) {
        SDL_Log("TextureManager: failed to load texture '%s': %s", path.c_str(), SDL_GetError());
        return nullptr;
    }

    m_textures[path] = texture;
    SDL_Log("TextureManager: loaded '%s'", path.c_str());
    return texture;
}

void TextureManager::Unload(const std::string& path) {
    auto it = m_textures.find(path);
    if (it == m_textures.end()) return;

    SDL_DestroyTexture(it->second);
    m_textures.erase(it);
}

void TextureManager::UnloadAll() {
    for (auto& [path, texture] : m_textures) {
        SDL_DestroyTexture(texture);
    }
    m_textures.clear();
}

} // namespace Core