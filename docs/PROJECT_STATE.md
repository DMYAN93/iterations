# Iterations — Project State
*Last updated: 2026-02-24*

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

## SDL3 / Library Notes

- SDL3_image header: `<SDL3_image/SDL_image.h>` (not `<SDL3/SDL_image.h>`)
- SDL3_ttf header: `<SDL3_ttf/SDL_ttf.h>`
- `SDL_GetKeyboardState` returns `const bool*` in SDL3 — use `reinterpret_cast<const u8*>`
- `IMG_Init` / `IMG_Quit` removed in SDL3_image — format detection is automatic
- `SDL_GetTextureSize` replaces SDL2's `SDL_QueryTexture`
- PNG support in sdl3-image requires explicit vcpkg feature: `"features": ["png", "jpeg"]`
- `sdl3-mixer` not yet available in vcpkg — revisit for audio, or use `miniaudio`
- `nlohmann-json`: use `json.value("key", default)` idiom for safe fallback reads
- `TTF_Init` returns bool in SDL3_ttf — check with `if (!TTF_Init())`
- `TTF_RenderText_Blended` in SDL3_ttf: second arg `const char*`, third `Uint32 wrapLength`, fourth `SDL_Color`

---

## Architecture Decisions

- Simple homemade ECS before considering Flecs
- ECS uses type erasure (`unique_ptr<void, void(*)(void*)>` + `type_index`) for component stores
- `GetOrCreateStore` uses `emplace` not `operator[]` (no default constructor on the void unique_ptr)
- Two system lists in `World`: `m_updateSystems` and `m_renderSystems`
- All files use `src/` as include root — no relative paths
- Each `.cpp` explicitly includes what it directly uses — no relying on transitive includes
- **Systems know only about their own components** — no cross-system dependencies
- `ECS::World` lives in `GameplayState`, not `Game` — `Game` is purely a state host
- State stack uses `std::vector<unique_ptr<GameState>>` — top is `back()`
- Member declaration order in `Game` is load-bearing: `m_settings` → `m_window` → `m_textureManager` → `m_debugRenderer` → `m_input`
- `TransformComponent` is the entity's logical center — all sprite and hitbox offsets are relative to it
- Tag components (e.g. `PlayerComponent`) use empty structs — presence in store IS the data
- `DebugRenderSystem` is the **only** system that knows about `DebugRenderer` — all debug drawing centralised there
- `DebugRenderer` is touched by exactly two files: `DebugRendererSystem.cpp` and `Game.cpp`
- `SettingsManager` is loaded before `Game` is constructed, passed in by value and moved
- `TilemapComponent` has no `TransformComponent` — implicitly rooted at world origin `(0,0)`
- `RenderComponent` retained as a valid primitive for debug drawing and fallback
- Clear/present contract: states are responsible for calling `SDL_RenderClear`, `Game::Render` calls `SDL_RenderPresent`
- **Data strategy:** authored data (character definitions, events, dialogue) → JSON; runtime simulation state → SQLite; dialogue trees → JSON node/edge schema with a `DialogueSystem` interpreter
- Game-specific components will live in `src/game/components/` when they appear, separate from engine-level `src/components/`
- Engine/game separation is a long-term goal — `core/`, `ecs/`, `systems/` should remain reusable

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

## What's Working

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
- `DebugRendererSystem` — sprite bounds, collision hitboxes, tile grid, walkability overlay
- Three-state `DebugMode`: `Off`, `PerFlag`, `On` — controlled from `settings.json` with no recompile
- Tile collision — AABB hitbox on player, axis-independent resolution, wall sliding works
- Test assets: `Idle.png` (2-frame duck spritesheet, 64×64 per frame), `tileset.png` (8×8 color grid), `JetBrainsMono-Regular.ttf`

---

## Pending Fixes (ready to apply, not yet committed)

Both fixes are in `src/core/Game.cpp`, in the `Run()` method:

**1. Delta time clamp** — add after computing `deltaTime`:
```cpp
constexpr float k_maxDelta = 0.1f;
deltaTime = (deltaTime < k_maxDelta) ? deltaTime : k_maxDelta;
```

**2. Input ordering** — swap so `ProcessInput()` runs before `m_input.BeginFrame()`:
```cpp
ProcessInput();
m_input.BeginFrame();
```

---

## Next Steps

- Apply the two `Game.cpp` fixes above
- **Main menu state** — design discussion first, then implementation
- **Frame cap / vsync** — running uncapped (~1200 FPS) is wasteful
- **UI / text rendering** — SDL3_ttf is wired for debug, needs a real UI layer for game text
- **Load tilemap from file** — Tiled editor JSON export
- **Camera bounds clamping** — player can walk camera outside the tilemap

---

## Known Tech Debt

- `SettingsManager.cpp` — rename `j` to `settingsFile` for readability (TODO comment in place)
- `DebugRenderer::DrawText` — creates/destroys a texture every call; fine for debug, not for game UI
- `World::DestroyEntity` is a no-op — never actually removes components from stores
- `SpriteComponent::debugDraw` field exists but is never read — should be removed
- `CollisionSystem` only handles one tilemap — grabs first and breaks
- `AnimationComponent` stores clip frame data by value per entity — should eventually be shared resources
- `RenderContext` missing — `TilemapSystem` and `RenderSystem` both independently query `CameraComponent` each frame
- Camera position stored in `CameraComponent` rather than `TransformComponent` — inconsistent with other entities
- `GameplayState::InitEntities` mixes entity composition, asset paths, and tilemap layout — all eventually candidates for data files
- `PausedState::Render` calls `GetStateBelow()->DrawScene()` with no null check
