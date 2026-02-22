#include "InputManager.h"
#include <SDL3/SDL.h>

namespace Core {

InputManager::InputManager() {
    // SDL_GetKeyboardState returns a pointer to SDL's internal array.
    // We snapshot it so we can diff against the previous frame.
    const u8* state = reinterpret_cast<const u8*>(SDL_GetKeyboardState(&m_keyCount));
    m_current.assign(state, state + m_keyCount);
    m_previous = m_current;
}

void InputManager::BeginFrame() {
    // Pump SDL's internal state (SDL_PollEvent must still be called separately
    // to dispatch window/quit events, but this keeps the key array up to date).
    m_previous = m_current;
    const u8* state = reinterpret_cast<const u8*>(SDL_GetKeyboardState(nullptr));
    m_current.assign(state, state + m_keyCount);
}

bool InputManager::IsKeyDown(SDL_Scancode key) const {
    if (key < 0 || key >= m_keyCount) return false;
    return m_current[key] != 0;
}

bool InputManager::IsKeyJustPressed(SDL_Scancode key) const {
    if (key < 0 || key >= m_keyCount) return false;
    return m_current[key] != 0 && m_previous[key] == 0;
}

bool InputManager::IsKeyJustReleased(SDL_Scancode key) const {
    if (key < 0 || key >= m_keyCount) return false;
    return m_current[key] == 0 && m_previous[key] != 0;
}

} // namespace Core