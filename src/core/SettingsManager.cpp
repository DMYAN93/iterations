#include "core/SettingsManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <SDL3/SDL.h>

using json = nlohmann::json;

namespace Core {

    // TODO: rename j to settingsFile for readability
bool SettingsManager::Load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        SDL_Log("SettingsManager: no file at '%s', using defaults.", path.c_str());
        return false;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        SDL_Log("SettingsManager: parse error in '%s': %s", path.c_str(), e.what());
        return false;
    }

    // Window
    if (j.contains("window")) {
        auto& w    = j["window"];
        window.width  = w.value("width",  window.width);
        window.height = w.value("height", window.height);
        window.title  = w.value("title",  window.title);
    }

    // Gameplay
    if (j.contains("gameplay")) {
        auto& g       = j["gameplay"];
        gameplay.playerSpeed = g.value("playerSpeed", gameplay.playerSpeed);
    }

    // Simulation
    if (j.contains("simulation")) {
        auto& simulationJson = j["simulation"];
        simulation.tickRateHz = simulationJson.value("tickRateHz", simulation.tickRateHz);
        simulation.maxSubstepsPerFrame = simulationJson.value("maxSubstepsPerFrame", simulation.maxSubstepsPerFrame);
        simulation.maxFrameDelta = simulationJson.value("maxFrameDelta", simulation.maxFrameDelta);
    }

    // Audio
    if (j.contains("audio")) {
        auto& a          = j["audio"];
        audio.masterVolume = a.value("masterVolume", audio.masterVolume);
        audio.musicVolume  = a.value("musicVolume",  audio.musicVolume);
        audio.sfxVolume    = a.value("sfxVolume",    audio.sfxVolume);
    }

    // Graphics
    if (j.contains("graphics")) {
        auto& g        = j["graphics"];
        graphics.vsync = g.value("vsync", graphics.vsync);

        std::string mode = g.value("screenMode", "windowed");
        if      (mode == "borderless") graphics.screenMode = ScreenMode::Borderless;
        else if (mode == "fullscreen") graphics.screenMode = ScreenMode::Fullscreen;
        else                           graphics.screenMode = ScreenMode::Windowed;
    }

    // Debug
    if (j.contains("debug")) {
        auto& d = j["debug"];

        std::string mode = d.value("mode", "off");
        if      (mode == "on")      debug.mode = DebugMode::On;
        else if (mode == "perFlag") debug.mode = DebugMode::PerFlag;
        else                        debug.mode = DebugMode::Off;

        debug.spriteBounds    = d.value("spriteBounds",    debug.spriteBounds);
        debug.showTileGrid    = d.value("showTileGrid",    debug.showTileGrid);
        debug.showWalkability = d.value("showWalkability", debug.showWalkability);
        debug.showCollision   = d.value("showCollision",   debug.showCollision);
        debug.showFPS         = d.value("showFPS",         debug.showFPS);
        debug.debugFontPath   = d.value("debugFontPath",   debug.debugFontPath);
    }

    // Keybindings
    if (j.contains("keys")) {
        auto& k      = j["keys"];
        keys.moveUp    = k.value("moveUp",    keys.moveUp);
        keys.moveDown  = k.value("moveDown",  keys.moveDown);
        keys.moveLeft  = k.value("moveLeft",  keys.moveLeft);
        keys.moveRight = k.value("moveRight", keys.moveRight);
        keys.pause     = k.value("pause",     keys.pause);
    }

    SDL_Log("SettingsManager: loaded '%s'.", path.c_str());
    return true;
}

// TODO: rename j to settingsFile for readability
bool SettingsManager::Save(const std::string& path) const {
    json j;

    j["window"]   = {
        {"width",  window.width},
        {"height", window.height},
        {"title",  window.title}
    };

    j["gameplay"] = {
        {"playerSpeed", gameplay.playerSpeed}
    };

    j["simulation"] = {
        {"tickRateHz",          simulation.tickRateHz},
        {"maxSubstepsPerFrame", simulation.maxSubstepsPerFrame},
        {"maxFrameDelta",       simulation.maxFrameDelta}
    };

    j["audio"] = {
        {"masterVolume", audio.masterVolume},
        {"musicVolume",  audio.musicVolume},
        {"sfxVolume",    audio.sfxVolume}
    };

    std::string screenModeStr;
    switch (graphics.screenMode) {
        case ScreenMode::Borderless: screenModeStr = "borderless"; break;
        case ScreenMode::Fullscreen: screenModeStr = "fullscreen"; break;
        default:                     screenModeStr = "windowed";   break;
    }

    j["graphics"] = {
        {"vsync",      graphics.vsync},
        {"screenMode", screenModeStr}
    };

    std::string modeStr;
    switch (debug.mode) {
        case DebugMode::On:      modeStr = "on";      break;
        case DebugMode::PerFlag: modeStr = "perFlag"; break;
        default:                 modeStr = "off";     break;
    }

    j["debug"] = {
        {"mode",            modeStr},
        {"spriteBounds",    debug.spriteBounds},
        {"showTileGrid",    debug.showTileGrid},
        {"showWalkability", debug.showWalkability},
        {"showCollision",   debug.showCollision},
        {"showFPS",         debug.showFPS},
        {"debugFontPath",   debug.debugFontPath}
    };

    j["keys"] = {
        {"moveUp",    keys.moveUp},
        {"moveDown",  keys.moveDown},
        {"moveLeft",  keys.moveLeft},
        {"moveRight", keys.moveRight},
        {"pause",     keys.pause}
    };

    std::ofstream file(path);
    if (!file.is_open()) {
        SDL_Log("SettingsManager: could not write to '%s'.", path.c_str());
        return false;
    }

    file << j.dump(4);
    SDL_Log("SettingsManager: saved '%s'.", path.c_str());
    return true;
}

bool SettingsManager::IsDebugEnabled(bool flag) const {
    switch (debug.mode) {
        case DebugMode::On:      return true;
        case DebugMode::Off:     return false;
        case DebugMode::PerFlag: return flag;
    }
    return false;
}

} // namespace Core
