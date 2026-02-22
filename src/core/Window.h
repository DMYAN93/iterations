#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "Types.h"

namespace Core {

class Window {
public:
    Window(const std::string& title, i32 width, i32 height);
    ~Window();

    // Non-copyable - there should only ever be one window
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool IsValid() const;
    SDL_Renderer* GetRenderer() const;
    SDL_Window* GetHandle() const;

private:
    SDL_Window*   m_window   = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

} // namespace Core