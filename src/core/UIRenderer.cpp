#include "core/UIRenderer.h"
#include <SDL3/SDL.h>

namespace Core {

UIRenderer::UIRenderer(SDL_Renderer* renderer, const std::string& fontPath, i32 fontSize, i32 screenWidth)
    : m_renderer(renderer)
    , m_screenWidth(screenWidth) {
    m_font = TTF_OpenFont(fontPath.c_str(), static_cast<float>(fontSize));
    if (!m_font) {
        SDL_Log("UIRenderer: failed to load font '%s': %s", fontPath.c_str(), SDL_GetError());
    } else {
        SDL_Log("UIRenderer: loaded font '%s' at size %d.", fontPath.c_str(), fontSize);
    }
}

UIRenderer::~UIRenderer() {
    if (m_font) {
        TTF_CloseFont(m_font);
    }
}

bool UIRenderer::IsValid() const {
    return m_font != nullptr;
}

void UIRenderer::DrawText(const std::string& text, i32 x, i32 y, SDL_Color color) {
    if (!m_font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) return;

    float width  = 0.0f;
    float height = 0.0f;
    SDL_GetTextureSize(texture, &width, &height);

    SDL_FRect dst = {
        static_cast<float>(x),
        static_cast<float>(y),
        width,
        height
    };

    SDL_RenderTexture(m_renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
}

void UIRenderer::DrawTextCentered(const std::string& text, i32 y, SDL_Color color) {
    if (!m_font) return;

    int width  = 0;
    int height = 0;
    TTF_GetStringSize(m_font, text.c_str(), 0, &width, &height);

    i32 x = (m_screenWidth - width) / 2;
    DrawText(text, x, y, color);
}

} // namespace Core