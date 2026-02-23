#pragma once
#include "core/SettingsManager.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <deque>

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
    SDL_Renderer*          m_renderer;
    const SettingsManager& m_settings;
    TTF_Font*              m_font  = nullptr;
    bool                   m_valid = false;

    static constexpr int   k_fpsSampleCount = 30;
    std::deque<float>      m_fpsSamples;
};

} // namespace Core