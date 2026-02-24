#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "Types.h"

namespace Core {

// Tracks per-frame keyboard state.
// Call BeginFrame() once per frame after SDL events are pumped for that frame,
// then query with IsKeyDown / IsKeyJustPressed / IsKeyJustReleased anywhere.
class InputManager {
public:
    InputManager();

    // Must be called once per frame after the event pump (SDL_PollEvent loop).
    void BeginFrame();

    // True every frame the key is physically held down.
    bool IsKeyDown(SDL_Scancode key) const;

    // True only on the first frame the key is pressed.
    bool IsKeyJustPressed(SDL_Scancode key) const;

    // True only on the frame the key is released.
    bool IsKeyJustReleased(SDL_Scancode key) const;

private:
    std::vector<u8> m_current;
    std::vector<u8> m_previous;
    int             m_keyCount = 0;
};

} // namespace Core
