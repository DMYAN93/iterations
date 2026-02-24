# Iterations — Dev Diary 2026-02-23

Good session today. Picked up right where yesterday left off and pushed through a lot of ground —
animation, a proper settings system, debug infrastructure, and tile collision. The engine is starting
to feel like something real rather than a tech demo.

---

## Chat Preferences

- CS degree, ~10 years out of active development. Returning through this project.
- Explain the **why**, not just the what. Flag modern C++ conventions vs fundamentals.
- Don't over-explain CS basics (pointers, OOP, complexity). Do explain modern C++ features, SDL3 API, ECS patterns, tooling.
- **One file per message** — full file first, explanation after, then ask to proceed.
- **No filename comment at top of code blocks** — redundant with the code block header.
- **No single-letter variables** except loop iterators.
- **Design discussion before every feature** — plan files, decisions, trade-offs, then proceed.
- Prose over bullet points for explanations.
- Be direct about trade-offs rather than presenting one option as obvious.
- Generate a new `CHAT_NOTES_YYYY-MM-DD.md` diary file at end of each session.

---

## Project Setup & Tooling

- **OS:** Windows
- **Compiler:** MSVC (Visual Studio Build Tools 2022, amd64)
- **Build system:** CMake with Ninja
- **Package manager:** vcpkg in manifest mode (`vcpkg.json` in project root)
- **SDL3** installed via vcpkg into `vcpkg_installed/` in project root (not global)
- **CMake include path:** `target_include_directories(Iterations PRIVATE ${CMAKE_SOURCE_DIR}/src)`
- **IntelliSense config:** `.vscode/c_cpp_properties.json` with `${workspaceFolder}/src` in includePath
- **GLOB_RECURSE** used for sources — run `CMake: Configure` from VS Code command palette when adding new `.cpp` files, otherwise linker won't find them
- `.vscode/` committed to Git (intentional, solo project)
- `build/` and `vcpkg_installed/` in `.gitignore`
- Assets live in `build/assets/` (relative to executable)
- `settings.json` lives in `build/` alongside the executable

---

## vcpkg Dependencies

```json
{
    "name": "iterations",
    "version": "0.1.0",
    "dependencies": [
        "sdl3",
        {
            "name": "sdl3-image",
            "features": ["png", "jpeg"]
        },
        "sdl3-ttf",
        "nlohmann-json"
    ]
}
```

---

## Architecture Decisions

- Simple homemade ECS before considering Flecs
- ECS uses type erasure (`unique_ptr<void, void(*)(void*)>` + `type_index`) for component stores
- Two system lists in World: `m_updateSystems` and `m_renderSystems`
- All files use `src/` as include root — no relative paths except where noted
- Each `.cpp` explicitly includes what it directly uses — no relying on transitive includes
- **Systems know only about their own components** — no cross-system dependencies. Debug rendering is the canonical example of what not to do.
- `ECS::World` lives in `GameplayState`, not `Game` — `Game` is purely a state host
- State stack uses `std::vector<unique_ptr<GameState>>` — top is `back()`
- Member declaration order in `Game` is load-bearing: `m_settings` → `m_window` → `m_textureManager` → `m_debugRenderer` → `m_input`
- `TransformComponent` is the entity's logical center — all sprite and hitbox offsets are relative to it
- `SpriteComponent::offsetX/offsetY` shifts sprite rendering relative to transform center. Depth/layer ordering is a future concern driven by asset data.
- Tag components (e.g. `PlayerComponent`) use empty structs — presence in store IS the data
- `DebugRenderSystem` is the **only** system that knows about `DebugRenderer` — all debug drawing is centralised there
- `DebugRenderer` is touched by exactly two files: `DebugRendererSystem.cpp` and `Game.cpp`
- `SettingsManager` is loaded before `Game` is constructed, passed in by value and moved
- **Authored data** (character definitions, event definitions, dialogue) → JSON files
- **Runtime simulation state** (living character data, relationship graphs, history) → SQLite
- **Scripted logic / dialogue trees** → JSON with custom node/edge schema and a `DialogueSystem` interpreter. Ink is a future option if JSON volume becomes unmanageable. Defer actual implementation.
- **Scripting language** (Lua etc.) → deferred until pain is felt
- Game-specific components will live in `src/game/components/` when they appear, separate from engine-level `src/components/`
- No generic property-bag component — the character matrix system will handle game-specific dynamic data
- Engine/game separation is a long-term goal — `core/`, `ecs/`, `systems/` should remain reusable

---

## SDL3 / vcpkg Notes

- SDL3_image header: `<SDL3_image/SDL_image.h>` (not `<SDL3/SDL_image.h>`)
- SDL3_ttf header: `<SDL3_ttf/SDL_ttf.h>`
- `SDL_GetKeyboardState` returns `const bool*` in SDL3 (was `const Uint8*` in SDL2) — use `reinterpret_cast<const u8*>`
- `IMG_Init` / `IMG_Quit` removed in SDL3_image — format detection is automatic
- `SDL_GetTextureSize` replaces SDL2's `SDL_QueryTexture`
- PNG support in sdl3-image requires explicit vcpkg feature: `"features": ["png", "jpeg"]`
- `sdl3-mixer` not yet available in vcpkg — revisit for audio, or use `miniaudio`
- `nlohmann-json` available in vcpkg — use `json.value("key", default)` idiom for safe fallback reads
- TTF_Init returns bool in SDL3_ttf, not int — check with `if (!TTF_Init())`
- `TTF_RenderText_Blended` signature changed in SDL3_ttf — second arg is `const char*`, third is `Uint32 wrapLength`, fourth is `SDL_Color`

---

## Folder Structure

```
src/
├── main.cpp
├── components/
│   ├── AnimationComponent.h
│   ├── CameraComponent.h
│   ├── CollisionComponent.h
│   ├── PlayerComponent.h
│   ├── RenderComponent.h
│   ├── SpriteComponent.h
│   ├── TilemapComponent.h
│   └── TransformComponent.h
├── core/
│   ├── DebugRenderer.h / DebugRenderer.cpp
│   ├── Game.h / Game.cpp
│   ├── GameState.h
│   ├── GameplayState.h / GameplayState.cpp
│   ├── InputManager.h / InputManager.cpp
│   ├── PausedState.h / PausedState.cpp
│   ├── SettingsManager.h / SettingsManager.cpp
│   ├── TextureManager.h / TextureManager.cpp
│   ├── Types.h
│   └── Window.h / Window.cpp
├── ecs/
│   ├── ComponentStore.h
│   ├── Entity.h
│   ├── System.h
│   └── World.h / World.cpp
└── systems/
    ├── AnimationSystem.h / AnimationSystem.cpp
    ├── CameraSystem.h / CameraSystem.cpp
    ├── CollisionSystem.h / CollisionSystem.cpp
    ├── DebugRendererSystem.h / DebugRendererSystem.cpp
    ├── MovementSystem.h / MovementSystem.cpp
    ├── RenderSystem.h / RenderSystem.cpp
    └── TilemapSystem.h / TilemapSystem.cpp
```

---

## Cumulative State (what's working)

- Window, game loop, ECS all solid
- WASD movement with delta time, camera follows player
- `InputManager` tracks held / just pressed / just released per frame
- `TextureManager` loads and caches textures by path
- Sprite rendering with per-component offset from transform center
- Sprite animation — named clips, explicit frame rects, looping/non-looping
- `RenderComponent` retained as colored rect primitive / fallback
- Tilemap rendering from tile ID grid + tileset texture
- State stack: `GameplayState` → Escape → `PausedState` (dark overlay, world frozen) → Escape → resume
- `SettingsManager` — loads/saves JSON, categorized structs, sensible defaults on missing file or keys
- `DebugRenderer` — FPS counter (rolling 30-frame average), `DrawRect`, `DrawFilledRect`, `DrawText`
- `DebugRendererSystem` — centralised debug drawing, sprite bounds, collision hitboxes, tile grid, walkability overlay
- Three-state `DebugMode`: `Off`, `PerFlag`, `On` — controlled from `settings.json` with no recompile
- Tile collision — AABB hitbox on player, axis-independent resolution so wall sliding works, previous position stored in `TransformComponent`
- Test assets: `Idle.png` (2-frame duck spritesheet, 64×64 per frame), `tileset.png` (generated 8×8 color grid), `JetBrainsMono-Regular.ttf`

---

## Done This Session

Started the day planning to continue from yesterday's tilemap/camera state. Ended up building a lot more than expected.

Sprite animation came first — named clips, explicit SDL_FRect arrays per clip, `AnimationSystem` driving `SpriteComponent::srcRect` each tick. `srcRect` changed from a raw pointer to `std::optional<SDL_FRect>` to eliminate dangling pointer risk. Confirmed the duck animates with real spritesheet dimensions (128×64, 2 frames at 64×64).

Then spent time on the right foundations before collision: `SettingsManager` with full JSON load/save, categorized structs, three-state debug mode. `DebugRenderer` with font loading, fallback system fonts, FPS counter. Confirmed player speed changes in `settings.json` take effect without recompile.

Had a good architecture conversation about ECS purity — systems should only know about their components. Caught ourselves adding `DebugRenderer` to `RenderSystem`, `TilemapSystem`, `CollisionSystem` and corrected it by pulling all debug drawing into `DebugRendererSystem`. Now the boundary is clean.

Collision landed well — AABB against tilemap walkability, axis-independent resolution, previous position in `TransformComponent` for clean snap-back. Debug hitbox visible via `showCollision` flag. Transform is now center-anchored, sprite offsets relative to that.

Also had useful side discussions on dialogue system design (JSON node/edge graph), data storage strategy (JSON for authored data, SQLite for runtime state), and folder separation for future game-specific components.

---

## Next Steps

- **Main menu state** — most natural next step, also tests UI text rendering properly
- **Frame cap / vsync** — running at ~1200 FPS is wasteful, easy fix
- **UI / text rendering** — SDL3_ttf is wired for debug, needs a real UI layer for game text
- **Load tilemap from file** — Tiled editor JSON export
- **Camera bounds clamping** — player can currently walk camera outside the tilemap

---

## Code to Revise Later

- `SettingsManager.cpp` — rename `j` to `settingsFile` for readability (TODO comment already in place)
- `DebugRenderer::DrawText` — creates and destroys a texture every call, fine for debug but not for game UI. Note this when building the real UI system.
- `Game.h` include inconsistency — some files still use relative paths instead of `src/`-rooted includes
- `CollisionSystem` currently only supports one tilemap — grab first and break. Will need revisiting for multi-map or layered maps.

---

## Known Issues / Tech Debt

- **Render order** — controlled by system registration order. No proper layer/depth system. Will cause pain as visual complexity grows.
- **`RenderContext` missing** — `TilemapSystem` and `RenderSystem` both independently query `CameraComponent` each frame to get offset. Should be computed once per frame into a shared `RenderContext` struct, written by `CameraSystem`, read by render systems. Eliminates redundant iteration and gives a clean place to add zoom/viewport later.
- **Camera bounds** — camera can move outside the tilemap. Needs world bounds clamping once map size is stable.
- **Audio** — `sdl3-mixer` not in vcpkg. Revisit or adopt `miniaudio`.
- **Asset paths in code** — texture paths, font paths, settings path all hardcoded. Needs a proper asset pipeline once the game layer starts growing.
- **State stack `PushState` timing** — states pushed synchronously mid-frame. Defer to a command queue if this causes issues.
- **DebugRenderer font fallbacks** — currently Windows-only paths. Low priority TODO for Linux/Mac if portability ever matters.
- **Sprite layer/depth ordering** — no system yet. Will be driven by asset data when the asset pipeline is built.
