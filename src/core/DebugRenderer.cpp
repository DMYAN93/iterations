#include "core/DebugRenderer.h"
#include <SDL3/SDL.h>
#include <numeric>
#include <utility>

namespace Core {

// TODO (low priority): expand fallback font list for Linux/Mac support
static const char* k_fallbackFonts[] = {
    "C:/Windows/Fonts/consola.ttf",
    "C:/Windows/Fonts/arial.ttf",
    "C:/Windows/Fonts/cour.ttf"
};

DebugRenderer::DebugRenderer(SDL_Renderer* renderer, const SettingsManager& settings)
    : m_renderer(renderer), m_settings(settings) {

    if (!TTF_Init()) {
        SDL_Log("DebugRenderer: TTF_Init failed: %s", SDL_GetError());
        return;
    }

    const int fontSize = 14;

    m_font = TTF_OpenFont(settings.debug.debugFontPath.c_str(), fontSize);

    if (!m_font) {
        SDL_Log("DebugRenderer: could not load font '%s', trying fallbacks.",
            settings.debug.debugFontPath.c_str());

        for (const char* fallback : k_fallbackFonts) {
            m_font = TTF_OpenFont(fallback, fontSize);
            if (m_font) {
                SDL_Log("DebugRenderer: using fallback font '%s'.", fallback);
                break;
            }
        }
    }

    if (!m_font) {
        SDL_Log("DebugRenderer: all fonts failed, debug text unavailable.");
        return;
    }

    m_valid = true;
}

DebugRenderer::~DebugRenderer() {
    for (auto& [cacheKey, cachedText] : m_textCache) {
        (void)cacheKey;
        if (cachedText.texture) {
            SDL_DestroyTexture(cachedText.texture);
        }
    }
    m_textCache.clear();

    if (m_font) TTF_CloseFont(m_font);
    TTF_Quit();
}

bool DebugRenderer::IsValid() const {
    return m_valid;
}

void DebugRenderer::DrawText(float x, float y, const std::string& text, SDL_Color color) {
    if (!m_valid) return;

    const u32 colorKey = (static_cast<u32>(color.r) << 24)
                       | (static_cast<u32>(color.g) << 16)
                       | (static_cast<u32>(color.b) << 8)
                       | static_cast<u32>(color.a);
    TextCacheKey key {text, colorKey};

    auto cacheEntry = m_textCache.find(key);
    if (cacheEntry == m_textCache.end()) {
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), 0, color);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_DestroySurface(surface);
        if (!texture) return;

        float width = 0.0f;
        float height = 0.0f;
        SDL_GetTextureSize(texture, &width, &height);

        if (m_textCache.size() >= k_maxCachedTextEntries) {
            for (auto& [cachedKey, cachedText] : m_textCache) {
                (void)cachedKey;
                if (cachedText.texture) {
                    SDL_DestroyTexture(cachedText.texture);
                }
            }
            m_textCache.clear();
        }

        auto [inserted, wasInserted] = m_textCache.emplace(
            std::move(key),
            CachedTextTexture {texture, width, height}
        );
        if (!wasInserted) {
            SDL_DestroyTexture(texture);
            return;
        }

        cacheEntry = inserted;
    }

    const CachedTextTexture& cachedText = cacheEntry->second;
    SDL_FRect dst { x, y, cachedText.width, cachedText.height };
    SDL_RenderTexture(m_renderer, cachedText.texture, nullptr, &dst);
}

void DebugRenderer::DrawFPS(float deltaTime) {
    if (!m_valid) return;
    if (!m_settings.IsDebugEnabled(m_settings.debug.showFPS)) return;

    m_fpsSamples.push_front(deltaTime);
    if (static_cast<int>(m_fpsSamples.size()) > k_fpsSampleCount) {
        m_fpsSamples.pop_back();
    }

    float avgDelta = std::accumulate(m_fpsSamples.begin(), m_fpsSamples.end(), 0.0f)
                     / static_cast<float>(m_fpsSamples.size());

    int fps = avgDelta > 0.0f ? static_cast<int>(1.0f / avgDelta) : 0;
    DrawText(8.0f, 8.0f, "FPS: " + std::to_string(fps), {255, 255, 0, 255});
}

void DebugRenderer::DrawRect(float x, float y, float w, float h, SDL_Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect { x, y, w, h };
    SDL_RenderRect(m_renderer, &rect);
}

void DebugRenderer::DrawFilledRect(float x, float y, float w, float h, SDL_Color color) {
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_FRect rect { x, y, w, h };
    SDL_RenderFillRect(m_renderer, &rect);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
}

} // namespace Core
