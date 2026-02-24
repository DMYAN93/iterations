#pragma once

#include "core/Types.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

namespace Core {

// Minimal UI text renderer for game-facing UI — not for debug output.
// Assumes TTF_Init has already been called (DebugRenderer handles this).
// Owns its font. DrawTextCentered takes a y position and centers horizontally
// within the given screen width.
class UIRenderer {
public:
    UIRenderer(SDL_Renderer* renderer, const std::string& fontPath, i32 fontSize, i32 screenWidth);
    ~UIRenderer();

    UIRenderer(const UIRenderer&)            = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;

    bool IsValid() const;

    void DrawText(const std::string& text, i32 x, i32 y, SDL_Color color);
    void DrawTextCentered(const std::string& text, i32 y, SDL_Color color);

private:
    SDL_Renderer* m_renderer  = nullptr;
    TTF_Font*     m_font      = nullptr;
    i32           m_screenWidth = 0;
};

} // namespace Core