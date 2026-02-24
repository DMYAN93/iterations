#pragma once
#include "core/SettingsManager.h"
#include "core/Types.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <deque>
#include <unordered_map>

namespace Core {

// Utility for debug-only rendering — text, shapes, FPS counter.
// Only draws when settings permit via IsDebugEnabled.
// Not intended for game UI — use a proper UI system for that.
class DebugRenderer {
public:
    DebugRenderer(SDL_Renderer* renderer, const SettingsManager& settings);
    ~DebugRenderer();

    DebugRenderer(const DebugRenderer&)            = delete;
    DebugRenderer& operator=(const DebugRenderer&) = delete;

    bool IsValid() const;

    void DrawText(float x, float y, const std::string& text, SDL_Color color = {255, 255, 0, 255});
    void DrawFPS(float deltaTime);
    void DrawRect(float x, float y, float w, float h, SDL_Color color = {255, 0, 255, 255});
    void DrawFilledRect(float x, float y, float w, float h, SDL_Color color = {255, 0, 0, 80});

private:
    struct TextCacheKey {
        std::string text;
        u32 colorKey = 0;

        bool operator==(const TextCacheKey& other) const {
            return text == other.text && colorKey == other.colorKey;
        }
    };

    struct TextCacheKeyHash {
        std::size_t operator()(const TextCacheKey& key) const {
            return std::hash<std::string>{}(key.text) ^ (static_cast<std::size_t>(key.colorKey) << 1);
        }
    };

    struct CachedTextTexture {
        SDL_Texture* texture = nullptr;
        float width = 0.0f;
        float height = 0.0f;
    };

    SDL_Renderer*          m_renderer;
    const SettingsManager& m_settings;
    TTF_Font*              m_font  = nullptr;
    bool                   m_valid = false;

    static constexpr int   k_fpsSampleCount = 30;
    static constexpr std::size_t k_maxCachedTextEntries = 256;
    std::deque<float>      m_fpsSamples;
    std::unordered_map<TextCacheKey, CachedTextTexture, TextCacheKeyHash> m_textCache;
};

} // namespace Core
