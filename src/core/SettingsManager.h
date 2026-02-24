#pragma once
#include <string>

namespace Core {

// -- Debug ----------------------------------------------------------------

enum class DebugMode {
    Off,     // no debug drawing, no overhead
    PerFlag, // respect individual flags below
    On       // all debug drawing enabled, individual flags ignored
};

struct DebugSettings {
    DebugMode mode            = DebugMode::Off;
    bool      spriteBounds    = false;
    bool      showTileGrid    = false;
    bool      showWalkability = false;
    bool      showCollision   = false;
    bool      showFPS         = false;
    std::string debugFontPath   = "assets/JetBrainsMono-Regular.ttf";
};

// -- Window ---------------------------------------------------------------

struct WindowSettings {
    int         width  = 800;
    int         height = 600;
    std::string title  = "Iterations";
};

// -- Gameplay -------------------------------------------------------------

struct GameplaySettings {
    float playerSpeed = 200.0f;
};

// -- Audio ----------------------------------------------------------------

struct AudioSettings {
    float masterVolume = 1.0f;
    float musicVolume  = 1.0f;
    float sfxVolume    = 1.0f;
};

// -- Graphics -------------------------------------------------------------

enum class ScreenMode {
    Windowed,
    Borderless,
    Fullscreen
};

struct GraphicsSettings {
    bool       vsync      = true;
    ScreenMode screenMode = ScreenMode::Windowed;
};

// -- Keybindings ----------------------------------------------------------
// Stub — will be expanded into a full InputMap when rebinding is implemented.

struct KeyBindings {
    int moveUp    = 26; // SDL_SCANCODE_W
    int moveDown  = 22; // SDL_SCANCODE_S
    int moveLeft  = 4;  // SDL_SCANCODE_A
    int moveRight = 7;  // SDL_SCANCODE_D
    int pause     = 41; // SDL_SCANCODE_ESCAPE
};

// -- SettingsManager ------------------------------------------------------

// Owns all settings categories. Load() populates from JSON, Save() writes
// back. Missing keys fall back to struct defaults, so first run works with
// no file present.
// Categories are public structs — treat as read-only outside SettingsManager
// except through explicit Save() roundtrips. Not enforced, but intended.
class SettingsManager {
public:
    WindowSettings   window;
    GameplaySettings gameplay;
    AudioSettings    audio;
    GraphicsSettings graphics;
    DebugSettings    debug;
    KeyBindings      keys;

    // Returns true if file loaded successfully. Falls back to defaults on failure.
    bool Load(const std::string& path);

    // Returns true if file written successfully.
    bool Save(const std::string& path) const;

    // Convenience — true if a given debug flag should be active,
    // respecting DebugMode::On and DebugMode::Off as overrides.
    bool IsDebugEnabled(bool flag) const;
};

} // namespace Core