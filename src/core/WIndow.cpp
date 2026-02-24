#include "core/Window.h"
#include <SDL3/SDL.h>

namespace Core {

Window::Window(const std::string& title, i32 width, i32 height, bool vsync, ScreenMode screenMode) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Window: failed to initialise SDL: %s", SDL_GetError());
        return;
    }

    SDL_WindowFlags flags = 0;
    if (screenMode == ScreenMode::Borderless) flags |= SDL_WINDOW_BORDERLESS;
    if (screenMode == ScreenMode::Fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    m_window = SDL_CreateWindow(title.c_str(), width, height, flags);
    if (!m_window) {
        SDL_Log("Window: failed to create window: %s", SDL_GetError());
        return;
    }

    if (screenMode == ScreenMode::Borderless) {
        SDL_MaximizeWindow(m_window);
    }

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer) {
        SDL_Log("Window: failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        return;
    }

    // Apply vsync from settings. We use 1 (regular vsync) for simplicity —
    // adaptive vsync (SDL value -1) can be revisited in the Options screen pass.
    int vsyncMode = vsync ? 1 : 0;
    if (!SDL_SetRenderVSync(m_renderer, vsyncMode)) {
        SDL_Log("Window: vsync mode %d requested but not supported: %s", vsyncMode, SDL_GetError());
    } else {
        SDL_Log("Window: vsync %s.", vsync ? "enabled" : "disabled");
    }

    const char* modeStr = (screenMode == ScreenMode::Fullscreen) ? "fullscreen"
                        : (screenMode == ScreenMode::Borderless)  ? "borderless"
                        :                                            "windowed";
    SDL_Log("Window: screen mode '%s'.", modeStr);
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