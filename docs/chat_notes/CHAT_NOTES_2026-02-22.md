# Iterations Project — Chat Notes 2026-02-22

## About the Developer
- CS degree, but ~10 years out of software development
- Needs modern C++ conventions and tooling explained, but not fundamental CS concepts
- Returning to programming through this project as a learning exercise
- Working on Windows, VS Code, MSVC compiler, SDL3, CMake, vcpkg

---

## How to Structure Responses

### Code Files
- Always show the **full file code first** (including comments), then explain line by line
- **One file per message** — never show multiple files in one response
- After showing a file, ask if ready to proceed to the next one
- Always include the **filename** at the top of the code block
- At the end of a milestone, include a **"How to test"** section — what to expect visually, what to interact with, what the SDL log should show

### Explanations
- Explain the **why**, not just the what
- Flag when something is a **modern C++ convention** vs a fundamental concept
- Use analogies where helpful
- Don't over-explain things a CS graduate would know (pointers, OOP, algorithms, complexity)
- **Do** explain: modern C++ features (smart pointers, auto, lambdas, structured bindings), tooling (CMake, vcpkg, IntelliSense vs compiler), SDL3 API, ECS patterns

### General
- Keep responses focused — one topic at a time
- When the developer asks a side question mid-file walkthrough, answer it then offer to continue
- Don't use excessive bullet points — prose is fine for explanations
- Be direct about trade-offs and architectural decisions rather than just presenting one option
- Before starting a new feature, always do a **design discussion first** — layout the plan, files to touch, key decisions, then ask to proceed
- At the end of each chat session, generate updated notes as `CHAT_NOTES_YYYY-MM-DD.md`

---

## Project Setup
- **OS:** Windows
- **Compiler:** MSVC (Visual Studio Build Tools 2022, amd64)
- **Build system:** CMake with Ninja
- **Package manager:** vcpkg in manifest mode (`vcpkg.json` in project root)
- **SDL3** installed via vcpkg into `vcpkg_installed/` in project root (not global)
- **CMake include path:** `target_include_directories(Iterations PRIVATE ${CMAKE_SOURCE_DIR}/src)`
- **IntelliSense config:** `.vscode/c_cpp_properties.json` with `${workspaceFolder}/src` in includePath
- **GLOB_RECURSE** used for sources — reconfigure CMake when adding new `.cpp` files
- `.vscode/` is committed to Git (intentional, solo project)
- `build/` and `vcpkg_installed/` are in `.gitignore`
- Assets live in `build/assets/` (relative to executable)

---

## Architecture Decisions
- Simple homemade ECS before considering Flecs
- ECS uses type erasure (`unique_ptr<void, void(*)(void*)>` + `type_index`) for component stores
- `GetOrCreateStore` uses `emplace` not `operator[]` (no default constructor on the void unique_ptr)
- Two system lists in World: `m_updateSystems` and `m_renderSystems`
- All files use `src/` as include root (e.g. `#include "core/Types.h"` not relative paths)
- Each `.cpp` explicitly includes what it directly uses — no relying on transitive includes
- `InputManager` is a value member on `Game`, not a pointer — no SDL resources, safe to construct early
- `TextureManager` is a `unique_ptr` on `Game` — needs renderer to construct, so heap allocated for ordering control
- Member declaration order in `Game` is load-bearing: `m_window` first (inits SDL), `m_textureManager` second (needs renderer), `m_input` third
- `ECS::World` lives in `GameplayState`, not `Game` — `Game` is purely a state host
- State stack uses `std::vector<unique_ptr<GameState>>` — top is `back()`
- `TilemapComponent` has no `TransformComponent` — implicitly rooted at world origin `(0,0)`
- Colored rect (`RenderComponent`) and sprite (`SpriteComponent`) coexist — rects draw first, sprites on top
- `RenderComponent` retained as a valid primitive for debug drawing and fallback
- Tag components (e.g. `PlayerComponent`) use empty structs — presence in store IS the data
- Engine/game separation is a long-term goal — `core/`, `ecs/`, `systems/` should remain reusable

---

## SDL3 Notes
- SDL3_image header: `<SDL3_image/SDL_image.h>` (not `<SDL3/SDL_image.h>`)
- SDL3_ttf header will be: `<SDL3_ttf/SDL_ttf.h>`
- `SDL_GetKeyboardState` returns `const bool*` in SDL3 (was `const Uint8*` in SDL2) — use `reinterpret_cast<const u8*>`
- `IMG_Init` / `IMG_Quit` removed in SDL3_image — format detection is automatic
- `SDL_GetTextureSize` replaces SDL2's `SDL_QueryTexture`
- PNG support in sdl3-image requires explicit vcpkg feature: `"features": ["png", "jpeg"]`
- `sdl3-mixer` not yet available in vcpkg (SDL3 is still new) — revisit for audio later, or use `miniaudio`

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
        "sdl3-ttf"
    ]
}
```

---

## Folder Structure
```
src/
├── main.cpp
├── components/
│   ├── CameraComponent.h
│   ├── PlayerComponent.h
│   ├── RenderComponent.h
│   ├── SpriteComponent.h
│   ├── TilemapComponent.h
│   └── TransformComponent.h
├── core/
│   ├── Game.h / Game.cpp
│   ├── GameState.h
│   ├── GameplayState.h / GameplayState.cpp
│   ├── InputManager.h / InputManager.cpp
│   ├── PausedState.h / PausedState.cpp
│   ├── TextureManager.h / TextureManager.cpp
│   ├── Types.h
│   └── Window.h / Window.cpp
├── ecs/
│   ├── ComponentStore.h
│   ├── Entity.h
│   ├── System.h
│   └── World.h / World.cpp
└── systems/
    ├── CameraSystem.h / CameraSystem.cpp
    ├── MovementSystem.h / MovementSystem.cpp
    ├── RenderSystem.h / RenderSystem.cpp
    └── TilemapSystem.h / TilemapSystem.cpp
```

---

## Current State
- Window, game loop, ECS all working
- `InputManager` tracks key held / just pressed / just released per frame
- WASD moves player entity, camera follows
- `TextureManager` loads and caches textures by path
- `SpriteComponent` renders textures; `RenderComponent` renders colored rects as fallback
- `TilemapSystem` renders a tile grid from a tileset texture using tile IDs
- State stack working: `GameplayState` → Escape → `PausedState` (dark overlay, world frozen) → Escape → resume
- Test assets in `build/assets/`: `Idle.png` (duck sprite), `tileset.png` (generated 8x8 color grid)

---

## TODO
- Sprite animation (spritesheet frames)
- UI layer / text rendering (SDL3_ttf)
- Main menu state
- Resource manager (formalize TextureManager into a broader asset system)
- Load tilemap from file (Tiled editor export)
- Collision / walkability using `Tile.walkable`
- Simulation layer (sci-fi framing)
- Dynasty / gameplay systems
- Engine / game layer separation

---

## Revisit
- **`TilemapComponent` has no `TransformComponent`** — currently rooted at world origin. Add if offset maps are needed.
- **Tile size is configurable per tilemap** (`tileWidth`/`tileHeight` fields) but no UI to change it — keep it out of any hardcoded constants.
- **`Game.h` include inconsistency** — `GameplayState.h` still uses `"../ecs/World.h"` relative path. Should be `"ecs/World.h"` for consistency with the rest of the project.
- **Render order** — currently controlled by system registration order. Will need a proper layer/depth system as complexity grows.
- **Camera bounds** — camera can currently move outside the tilemap. Add world bounds clamping when map size is more defined.
- **Audio** — `sdl3-mixer` not in vcpkg yet. Revisit, or adopt `miniaudio` (available in vcpkg, single-header).
- **Resource manager** — `TextureManager` is the seed. Expand into a full asset manager (fonts, tilemaps, audio) once sprites are stable. Decouple asset paths from code.
- **Engine / game separation** — `core/`, `ecs/`, `systems/` should eventually be a reusable engine layer. Game-specific content (dynasty systems, simulation) should live in a separate layer on top.
- **State stack `PushState` timing** — currently states are pushed synchronously mid-frame. If this causes issues later, consider a deferred command queue.

---

## Game Concept
**"Iterations"** — a dynasty simulator with RPG, strategy, and simulation layers.
- Sci-fi framing: researcher in near future interfaces with a simulation of a lost civilization
- Similar to Assassin's Creed framing device
- 2D, SDL3
- Simulation layer may eventually model economics similar to Victoria 3 with individual agents
- Setting: fictional world (simulation justifies full creative freedom)
