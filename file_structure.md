# File Structure

This structure is based on files currently synced with Git (`git ls-files`).

Excluded from this view because they are not synced (git-ignored):
- `build/`
- `vcpkg_installed/`
- `.vs/`
- `CMakeFiles/`
- generated binaries and objects (`*.exe`, `*.dll`, `*.obj`, `*.pdb`)

```text
.
|-- .claude/
|   `-- settings.local.json
|-- .vscode/
|   |-- c_cpp_properties.json
|   `-- settings.json
|-- docs/
|   |-- chat_notes/
|   |   |-- CHAT_NOTES_2026-02-22.md
|   |   |-- CHAT_NOTES_2026-02-23.md
|   |   `-- CHAT_NOTES_2026-02-24.md
|   |-- CHARACTER_MATRIX.md
|   |-- CHAT_PREFERENCES.md
|   |-- DESIGN_PILLARS.md
|   |-- DESIGN_REVIEW_CLAUDE.md
|   |-- DESIGN_REVIEW_CODEX.md
|   `-- PROJECT_STATE.md
|-- src/
|   |-- components/
|   |   |-- AnimationComponent.h
|   |   |-- CameraComponent.h
|   |   |-- CollisionComponent.h
|   |   |-- PlayerComponent.h
|   |   |-- RenderComponent.h
|   |   |-- SpriteComponent.h
|   |   |-- TilemapComponent.h
|   |   `-- TransformComponent.h
|   |-- core/
|   |   |-- DebugRenderer.cpp
|   |   |-- DebugRenderer.h
|   |   |-- Game.cpp
|   |   |-- Game.h
|   |   |-- GameState.h
|   |   |-- GameplayState.cpp
|   |   |-- GameplayState.h
|   |   |-- InputManager.cpp
|   |   |-- InputManager.h
|   |   |-- MenuState.cpp
|   |   |-- MenuState.h
|   |   |-- PausedState.cpp
|   |   |-- PausedState.h
|   |   |-- SettingsManager.cpp
|   |   |-- SettingsManager.h
|   |   |-- TextureManager.cpp
|   |   |-- TextureManager.h
|   |   |-- TilemapLoader.cpp
|   |   |-- TilemapLoader.h
|   |   |-- Types.h
|   |   |-- UIRenderer.cpp
|   |   |-- UIRenderer.h
|   |   |-- WIndow.cpp
|   |   `-- Window.h
|   |-- ecs/
|   |   |-- ComponentStore.h
|   |   |-- Entity.h
|   |   |-- System.h
|   |   |-- World.cpp
|   |   `-- World.h
|   |-- systems/
|   |   |-- AnimationSystem.cpp
|   |   |-- AnimationSystem.h
|   |   |-- CameraSystem.cpp
|   |   |-- CameraSystem.h
|   |   |-- CollisionSystem.cpp
|   |   |-- CollisionSystem.h
|   |   |-- DebugRendererSystem.cpp
|   |   |-- DebugRendererSystem.h
|   |   |-- MovementSystem.cpp
|   |   |-- MovementSystem.h
|   |   |-- RenderSystem.cpp
|   |   |-- RenderSystem.h
|   |   |-- TilemapSystem.cpp
|   |   `-- TilemapSystem.h
|   `-- main.cpp
|-- .gitignore
|-- AGENTS.md
|-- CLAUDE.md
|-- CMakeLists.txt
|-- README.md
|-- file_structure.md
`-- vcpkg.json
```
