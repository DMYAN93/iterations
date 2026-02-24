# Iterations — Agent Instructions (AGENTS.md)

This file defines how an agent should work in this repo (planning, editing, building, and reporting).
Chat formatting preferences live in `docs/CHAT_PREFERENCES.md`.

## Read-first (mandatory)
Before doing anything else:
1) Read `docs/PROJECT_STATE.md` (architecture, folder structure, dependencies, known issues).
2) Read `docs/CHAT_PREFERENCES.md` (how the human wants output presented in chat).

Hard stop:
- If you cannot access these files, say so clearly and stop. Do not guess.

## Context about the developer
- CS degree, ~10 years away from active development; returning through this project.
- Explain the **why** behind modern C++ conventions and SDL3/ECS patterns.
- Don’t over-explain CS fundamentals (pointers, OOP, complexity).

## Repo code conventions
- No single-letter variable names except loop iterators (`i`, `j`, …).
- Includes are rooted at `src/`: `#include "core/Types.h"` (no `../` relative paths).
- Each `.cpp` includes what it directly uses (no reliance on transitive includes).
- Systems know only about their own components (no cross-system dependencies).

## Agent workflow rules (planning, edits, verification)
- Prefer the smallest correct change; avoid drive-by refactors.
- For new features: propose a short plan (files touched + key decisions) before editing.
- Do not claim commands/tests were run unless you include the relevant output/log excerpt.
- If a build/test cannot be run (missing tools, permissions, network), state exactly:
  - what you attempted
  - the blocking error
  - the next command you would run in a working environment

## Dependency/build contract (important)
- Dependencies are defined by `vcpkg.json` (manifest).
- CMake uses `find_package(<dep> CONFIG REQUIRED)`; therefore the build must be configured in an environment
  where those *Config.cmake* files are discoverable (typically via vcpkg toolchain on Windows).
- Do **not** commit build outputs or installed package trees (`build/`, `vcpkg_installed/`, `.vs/`, etc.).

## Build system (Windows local baseline)
This repo is primarily built on Windows with Ninja + MSVC.

### Locations (Windows)
- CMake: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`
- Ninja: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe`
- MSVC toolchain root: `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\MSVC\`

### Standard build commands
- Build folder: `build/` (repo root)

Configure:
- `cmake -S . -B build -G Ninja`

Build:
- `cmake --build build --config Debug`

Policy:
- On **my Windows machine**, do not “hunt” for tools in PATH; use the full paths above when needed.
- On other environments (Linux/CI/Codex), use system tools if available and explicitly document the commands used.

### MSVC environment initialization (common failure mode)
If CMake/Ninja finds `cl.exe` but compilation fails with missing standard headers like `stdarg.h`, `<string>`, etc.,
then MSVC include/lib environment variables are not initialized.

Use one of these fixes:

**Preferred:** open **x64 Native Tools Command Prompt for VS 2022** (or **Developer PowerShell**), then run:
- `& 'C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' -S . -B build -G Ninja`
- `& 'C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --build build --config Debug`

**From a normal shell:** initialize MSVC first, then build:
- `cmd /c "\"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat\" && \"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe\" -S . -B build -G Ninja && \"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe\" --build build --config Debug\"`

### Reconfigure rule (GLOB_RECURSE)
This repo uses `GLOB_RECURSE` for sources, so if you add new `.cpp` files you must run configure again:
- `cmake -S . -B build -G Ninja`