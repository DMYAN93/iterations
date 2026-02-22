#include "Window.h"
#include <SDL3/SDL.h>

namespace Core {

Window::Window(const std::string& title, i32 width, i32 height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return;
    }

    m_window = SDL_CreateWindow(title.c_str(), width, height, 0);
    if (!m_window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return;
    }

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return;
    }
}

Window::~Window() {
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window)   SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Window::IsValid() const {
    return m_window != nullptr && m_renderer != nullptr;
}

SDL_Renderer* Window::GetRenderer() const { return m_renderer; }
SDL_Window*   Window::GetHandle()   const { return m_window; }

} // namespace Core