# Iterations — Design Review

A living document. Updated at the end of each session.
Items are marked with the date found and moved to Resolved when fixed.

---

## 1. Architecture & Long-Term Structure (Tech Lead)

**`Game` is a service locator and will become a god object** *(found 2026-02-23)*
`Game` provides renderer, input, texture manager, settings, and debug renderer to anyone holding
a `Game&`. As the engine grows — audio, save system, scene manager, asset registry — the natural
gravity is to keep adding to `Game` because it's accessible everywhere. Studios typically solve
this with a dedicated service container or by passing only what each system actually needs.
The current pattern will resist both approaches because everything already depends on `Game&`.

**State machine is ad-hoc** *(found 2026-02-23)*
The state stack works, but nothing prevents a state from calling `game.PopState()` inside
`Update`, which would invalidate the state currently executing. A deferred command queue —
execute state changes at the end of the frame — is the standard solution. Already noted as a
revisit item, worth elevating before the state count grows.

**No separation between engine tick and simulation tick** *(found 2026-02-23)*
Everything runs at render framerate. For a dynasty simulator this will eventually be wrong —
the simulation should run at a fixed timestep independent of rendering, and potentially faster
than real time. A fixed-timestep simulation loop with interpolated rendering is the correct
architecture for this kind of game and retrofitting it later is painful.

**Asset lifetime has no formal system** *(found 2026-02-23)*
Textures are cached in `TextureManager` and never freed except on shutdown. There's no concept
of asset scopes — loading assets for a state and freeing them when that state exits. Fine now,
but when the game has multiple states with different assets, everything will stay loaded forever.

**ECS has no query system** *(found 2026-02-23)*
Every system calls `GetStore<T>()` and iterates the full store. There's no way to express "give
me all entities that have both `TransformComponent` and `CollisionComponent`." As component counts
grow, systems will accumulate defensive `Has()` checks and iteration becomes increasingly manual.
A formal query/view system is the standard ECS solution.

**`GameplayState::InitEntities` mixes concerns** *(found 2026-02-23)*
Entity composition, asset paths, component values, and tilemap layout are all defined in one
C++ function. The game's starting state is baked into code rather than data. Everything in that
function is eventually a candidate for being read from a file. Fine while the game is a tech
demo, will need addressing when the game layer grows.

---

## 2. ECS Design (Engine Programmer)

**Camera position stored in `CameraComponent`, not `TransformComponent`** *(found 2026-02-23)*
Every other positional entity uses `TransformComponent`. The camera stores `x/y` directly in
`CameraComponent`, making it inconsistent. Camera lerp, shake, or any position-affecting system
would need to special-case it. Fix: move camera position into `TransformComponent` and keep
`CameraComponent` for viewport metadata only, or remove the camera entity entirely and store
it as a plain struct on `GameplayState`.

**Implicit singleton assumptions baked into system logic** *(found 2026-02-23)*
`CameraSystem`, `MovementSystem`, and `CollisionSystem` all iterate their stores and break after
the first matching entity. This silently assumes exactly one player and one camera will ever exist.
If a second `PlayerComponent` or `CameraComponent` entity is created, behavior becomes arbitrary.
These constraints should be explicitly documented or enforced.

**`MovementSystem` conflates input handling and movement** *(found 2026-02-23)*
It queries `PlayerComponent` to find the controlled entity then applies input directly. When AI
movement arrives this will need splitting — input should write movement intent into a component,
and `MovementSystem` should read that regardless of whether the entity is player or AI controlled.

**Render systems hold `SDL_Renderer*` directly** *(found 2026-02-23)*
`TilemapSystem` and `RenderSystem` depend on a rendering context that has nothing to do with
ECS component logic. Unavoidable for now, but means these systems can never be reused outside
an SDL context. Will need an abstraction layer when engine/game separation happens.

**`AnimationComponent` stores clip data by value per entity** *(found 2026-02-23)*
Every animated entity gets its own full copy of frame rect arrays. With many characters sharing
the same animations this becomes wasteful. Clips should eventually be shared resources referenced
by name from an asset registry.

**`RenderContext` missing** *(found 2026-02-23)*
`TilemapSystem` and `RenderSystem` both independently query `CameraComponent` each frame to get
the camera offset. Should be computed once per frame into a shared `RenderContext` struct, written
by `CameraSystem` and read by render systems. Eliminates redundant iteration and gives a clean
place to add zoom/viewport later.

---

## 3. Code Quality (Code Reviewer)

**`World::DestroyEntity` is a no-op** *(found 2026-02-23)*
The function loops over component stores but never calls `Remove` on any of them. Dead entities
will leak component data and continue appearing in system iterations. Fix requires rethinking
the type-erased store interface to support a generic `Remove` call. Not urgent until anything
destroys entities, but will cause confusing bugs when it does.

**`SpriteComponent::debugDraw` is dead data** *(found 2026-02-23)*
The field exists but is never read. Debug drawing is handled entirely by `DebugRendererSystem`
via settings flags. Should be removed to avoid confusion.

**`GameplayState` holds `TextureManager*` as a raw non-owning pointer** *(found 2026-02-23)*
Lifetime is safe in practice — `Game` outlives `GameplayState` — but a raw pointer with no
ownership semantics is a refactor hazard. Should be a reference or at minimum have a comment
clarifying ownership.

**`DebugRenderer` calls `TTF_Quit()` in its destructor** *(found 2026-02-23)*
Safe as long as exactly one `DebugRenderer` exists, which is currently true. But it's a fragile
assumption with no enforcement. Needs a comment at minimum.

**`SpriteComponent` ownership contract is implicit** *(found 2026-02-23)*
`TextureManager` owns textures, `SpriteComponent` borrows them via raw pointer. Nothing enforces
this — a manually created `SDL_Texture*` stored in `SpriteComponent` would never be freed.
The ownership contract should be documented explicitly in the header.

**`SettingsManager` read-only intent is unenforced** *(found 2026-02-23)*
Categories are public structs with a comment saying treat as read-only outside the manager.
Nothing prevents a system from writing directly to `settings.gameplay.playerSpeed` mid-frame
and affecting other systems. Wherever settings are passed they should be passed as `const`
references to enforce this at the compiler level.

**Clear and present are split across two layers with no formal contract** *(found 2026-02-23)*
`Game::Render` calls `SDL_RenderPresent` but individual states call `SDL_RenderClear`. If a
state forgets to clear, the previous frame bleeds through. Should be documented as a firm
contract on `GameState` — states are responsible for clearing, `Game` is responsible for presenting.

**`DebugRenderer::DrawText` creates and destroys an `SDL_Texture` every call** *(found 2026-02-23)*
Fine for infrequent debug text, but a concrete performance problem for anything called frequently.
Must not be used as the basis for any game UI system without caching.

---

## 4. Logic Correctness

**Delta time is unclamped** *(found 2026-02-23)*
If the game freezes — debugger breakpoint, OS interruption, loading a file — the next frame's
delta time will be enormous and everything will teleport. Standard fix is clamping delta time
to a maximum, typically around 100ms.

**`SDL_GetTicks` overflow** *(found 2026-02-23)*
Delta time is computed from two `u32` values from `SDL_GetTicks`. After ~49 days of continuous
runtime the counter wraps to zero, producing a massive delta time spike. Clamping delta time
solves this too, or switch to `SDL_GetTicksNS`.

**`PausedState` has a potential null dereference** *(found 2026-02-23)*
`PausedState::Render` calls `game.GetStateBelow()->DrawScene(game)` with no null check. Safe
right now because `PausedState` is always pushed on top of `GameplayState`, but nothing enforces
that. If `PausedState` were ever the only state on the stack it would crash.

**`CollisionSystem` only supports one tilemap** *(found 2026-02-23)*
Grabs the first tilemap entity and breaks. Will need revisiting for multi-map or layered maps.

**`InputManager::BeginFrame` reads keyboard state before `SDL_PollEvent`** *(found 2026-02-23)*
The game loop runs `BeginFrame` then `ProcessInput`. `BeginFrame` snapshots the keyboard state
before `SDL_PollEvent` has been called, so every frame `InputManager` is reading the state from
the previous frame's event pump. Input is consistently one frame late — `IsKeyJustPressed` fires
one frame after the key is physically pressed. Fix: call `SDL_PollEvent` before `BeginFrame`, or
move `BeginFrame` to after the event loop.

**Collision resolution fails silently if entity spawns inside a non-walkable tile** *(found 2026-02-23)*
If `TransformComponent` is initialised inside a wall, both `prevX/prevY` and `x/y` overlap.
Snap-back restores `prevX/prevY` which is also overlapping — entity gets permanently stuck with
no escape and no error. Should at minimum log a warning if the resolved position is still overlapping.

**Integer truncation in `OverlapsNonWalkable` for negative coordinates** *(found 2026-02-23)*
`static_cast<i32>(-5 / 16)` truncates toward zero, giving `0` not `-1`. An entity at a slightly
negative world position might not be correctly identified as out-of-bounds. The border tiles mask
this currently, but on an open-edged map it would be a real bug. Floor division should be used
instead of truncation for negative coordinates.

---

## 5. Performance & Data-Oriented Review

**`unordered_map<Entity, T>` per component store is cache-unfriendly** *(found 2026-02-23)*
Components are stored in hash maps so iterating all entities with a given component results in
scattered memory access — the classic ECS performance mistake. A proper ECS stores components
in contiguous arrays so iteration is cache-friendly. Fine at current entity counts, but at
hundreds of simulated characters with matrices, memory layout will matter. Flecs or a custom
dense array store is the long-term fix.

**`AnimationSystem` does a string map lookup every tick per entity** *(found 2026-02-23)*
`anim.clips.find(anim.currentClip)` runs on every animated entity every frame. String lookups
in hash maps are slow relative to integer indexing. At scale this should be an index into a
flat array, not a string key into an `unordered_map`.

**`DebugRendererSystem` fetches component stores every frame regardless of debug mode** *(found 2026-02-23)*
Store fetches happen before the `IsDebugEnabled` check. When debug mode is `Off` this is
entirely wasted work. The enabled check should gate the store fetch entirely.

**`PausedState` re-renders the full game world every frame** *(found 2026-02-23)*
When paused, `DrawScene` is called on the state below — the full tilemap and entity render runs
every frame just to draw a static dark overlay on top. A screenshot-to-texture approach would
render once on pause entry and composite that each frame instead.

---

## 6. Game Feel / Player Experience Review

*(no findings yet — needs more gameplay to review)*

---

## 7. Portability Review

**Debug font fallbacks are Windows-only** *(found 2026-02-23)*
The fallback font list in `DebugRenderer` hardcodes `C:/Windows/Fonts/` paths. Low priority
given Windows-only target, but worth expanding if portability ever matters.

---

## 8. Testability Review

**Almost nothing can be tested without an SDL window** *(found 2026-02-23)*
Everything depends on `Game` or SDL resources. Systems, components, and logic are not structured
in a way that allows unit testing without spinning up the full engine. Not urgent, but worth
keeping in mind when structuring new systems — pure logic should be separable from SDL context.

---

## Resolved

*(nothing yet)*
