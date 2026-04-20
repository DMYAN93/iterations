# Iterations — Technical Setup & Project State
*Living document — updated regularly as the project evolves.*

## 1. Project Setup & Tooling
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

## 2. Build System Instructions (Windows local baseline)
Dependencies are defined by `vcpkg.json` (manifest). CMake uses `find_package(<dep> CONFIG REQUIRED)`, so configure must happen where `*Config.cmake` files are discoverable via vcpkg. Do **not** commit build outputs.

**Locations (Windows):**
- CMake: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`
- Ninja: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe`
- MSVC toolchain root: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\MSVC\`

**Standard Build Commands (from repo root):**
- Build folder: `build/`
- Configure: `cmake -S . -B build -G Ninja`
- Build: `cmake --build build --config Debug`

**MSVC Environment Initialization (Common Failure Mode):**
If CMake/Ninja finds `cl.exe` but compilation fails with missing standard headers (`stdarg.h`, `<string>`), MSVC include/lib variables are missing.
*Preferred Fix:* Open **x64 Native Tools Command Prompt for VS 2022** and run the absolute paths for CMake above.
*From Normal Shell:* `cmd /c "\"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat\" && \"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe\" -S . -B build -G Ninja && \"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe\" --build build --config Debug\"`

## 3. vcpkg Dependencies
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

## 4. SDL3 / Library Notes
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

## 5. Architecture Decisions
- Simple homemade ECS.
- ECS uses type erasure (`unique_ptr<void, void(*)(void*)>` + `type_index`) for component stores.
- `GetOrCreateStore` uses `emplace` not `operator[]`.
- Two system lists in `World`: `m_updateSystems` and `m_renderSystems`.
- `ECS::World` lives in `GameplayState`, not `Game` — `Game` is purely a state host.
- State stack uses `std::vector<unique_ptr<GameState>>` — top is `back()`.
- Member declaration order in `Game` is load-bearing: `m_settings` → `m_window` → `m_textureManager` → `m_debugRenderer` → `m_input`.
- `TransformComponent` is the entity's logical center — all sprite and hitbox offsets are relative to it.
- Tag components (e.g., `PlayerComponent`) use empty structs — presence in store IS the data.
- `DebugRenderSystem` is the **only** system that knows about `DebugRenderer`.
- `SettingsManager` is loaded before `Game` is constructed, passed in by value and moved.
- `TilemapComponent` has no `TransformComponent` — implicitly rooted at world origin `(0,0)`.
- `RenderComponent` retained as a valid primitive for debug drawing and fallback.
- **Clear/present contract:** states are responsible for calling `SDL_RenderClear`, `Game::Render` calls `SDL_RenderPresent`.
- **Data strategy:** authored data (character definitions, events, dialogue) → JSON; runtime simulation state → SQLite; dialogue trees → JSON node/edge schema with a `DialogueSystem` interpreter.
- Game-specific components will live in `src/game/components/`, separate from engine-level `src/components/`.
- Engine/game separation is a long-term goal — `core/`, `ecs/`, `systems/` should remain reusable.

## 6. Folder Structure
```text
.
|-- .claude/
|-- .vscode/
|-- docs/
|   `-- chat_notes/ (Session diaries)
|-- src/
|   |-- components/  (AnimationComponent, CameraComponent, TransformComponent, etc.)
|   |-- core/        (Game, GameState, InputManager, TextureManager, Window, etc.)
|   |-- ecs/         (Entity, World, System, ComponentStore)
|   |-- systems/     (AnimationSystem, CollisionSystem, MovementSystem, RenderSystem, etc.)
|   `-- main.cpp
|-- AGENTS.md        (AI Agent Instructions)
|-- DESIGN.md        (Game Design Document)
|-- README.md        (Technical Setup & Project State)
|-- CMakeLists.txt
`-- vcpkg.json
```
*(Excluded: build/, vcpkg_installed/, .vs/, CMakeFiles/, binaries)*

## 7. Current State (What's Working)
- Window, game loop, ECS all solid.
- Fixed-step simulation loop with variable rendering.
- WASD movement with delta time, camera follows player.
- Deferred state transitions (push/pop queued and applied at safe boundary).
- ECS Query/View abstraction (iterates entities matching component sets).
- `InputManager` tracks held / just pressed / just released per frame.
- `TextureManager` loads and caches textures by path.
- Sprite rendering with per-component offset from transform center.
- Sprite animation — named clips, explicit frame rects, looping/non-looping.
- Tilemap rendering from tile ID grid + tileset texture.
- State stack: `GameplayState` → Escape → `PausedState` (dark overlay, world frozen) → Escape → resume.
- `SettingsManager` — loads/saves JSON, categorized structs, sensible defaults on missing file or keys.
- `DebugRenderer` — FPS counter (rolling 30-frame average), DrawRect, DrawFilledRect, DrawText (with texture caching).
- `DebugRendererSystem` — sprite bounds, collision hitboxes, tile grid (camera-culled), walkability overlay.
- Three-state `DebugMode`: `Off`, `PerFlag`, `On`.
- Tile collision — AABB hitbox on player, axis-independent resolution, wall sliding works.
- Entity destruction removes components from all registered stores correctly.

## 8. Next Steps
- **Main menu state** — design discussion first, then implementation.
- **UI / text rendering** — SDL3_ttf is wired for debug, needs a real UI layer for game text.
- **Load tilemap from file** — Tiled editor JSON export.
- **Camera bounds clamping** — player can walk camera outside the tilemap.

## 9. Known Tech Debt & Technical Review
*Consolidated from ongoing design reviews.*

**High Priority (ECS Target Design & Architecture)**
1. **ECS Reads Mutate State:** `HasComponent`/`GetComponent` route through `GetOrCreateStore`, mutating state. Need read-only `FindStore`.
2. **ECS Views Allocate:** `World::View` allocates a vector every call. Move to non-allocating iterable views.
3. **Cache-Unfriendly ECS:** Move hot components (`Transform`, `Velocity`, `Sprite`) from hash maps to sparse-set/dense arrays.
4. **Game is a God Object:** `Game` acts as a service locator. Need a narrow `GameServices` interface or explicit DI.

**Medium Priority**
5. **Entity Lifecycles:** Entities lack generation/version tracking. Move to 64-bit `(index, generation)` handle.
6. **Command Buffer for ECS Mutations:** Add a command buffer for structural mutations to flush at deterministic sync points.
7. **Movement/Input Coupling:** `MovementSystem` directly reads SDL scancodes. Should read a `MovementIntentComponent` written by an `InputSystem`.
8. **Render Camera Coupling:** Render systems manually query the camera singleton. Pass a pre-computed `RenderContext` instead.

**Low Priority**
9. **SettingsManager Write Access:** Settings passed without `const` references, enabling unintended mid-frame writes.
10. **Animation Clip Duplication:** `AnimationComponent` copies frame rects per entity. Move to shared asset registry reference.
11. **Debug Font Fallbacks:** Hardcoded to Windows `C:/Windows/Fonts/`.
12. **Testability:** Systems are hard-coupled to SDL/Game, making pure logic unit tests difficult.