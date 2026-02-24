# Iterations — Chat Preferences

## About the Developer
- CS degree, ~10 years out of active development. Returning to programming through this project.
- Needs modern C++ conventions and tooling explained, but not fundamental CS concepts.
- Working on Windows, VS Code, MSVC (Visual Studio Build Tools 2022, amd64), CMake with Ninja, vcpkg.

---

## Response Format

### Code Files
- Show the **full file first**, then explain. Never show partial files unless discussing a specific isolated change.
- **One file per message.** After showing a file, ask if ready to proceed to the next.
- Show the **filename as a label in the response text** before the code block, so it's clear where the file lives. Never put it as a comment inside the code itself.
- At the end of a feature milestone, include a **"How to test"** section: what to expect visually, what to interact with, what the SDL log should show.

### Explanations
- Explain the **why**, not just the what.
- Flag when something is a **modern C++ convention** vs a fundamental concept.
- Don't over-explain CS fundamentals (pointers, OOP, algorithms, complexity).
- Do explain: modern C++ features (smart pointers, `auto`, lambdas, structured bindings, `std::optional`, move semantics), tooling (CMake, vcpkg, IntelliSense vs compiler), SDL3 API, ECS patterns.
- Use analogies where helpful.
- Prose over bullet points for explanations.

### General
- One topic at a time. Keep responses focused.
- When a side question comes up mid-walkthrough, answer it, then offer to continue.
- Be direct about trade-offs — present options honestly rather than steering toward one.

---

## Code Conventions
- No single-letter variable names except loop iterators (`i`, `j`, etc.).
- All includes use `src/` as root (e.g. `#include "core/Types.h"`), no relative paths.
- Each `.cpp` explicitly includes what it directly uses — no relying on transitive includes.

---

## Session Structure
- **Design discussion before every new feature** — lay out the plan, files to touch, key decisions and trade-offs, then ask to proceed.

### File Approach
The project uses six documents with distinct roles:

| File | Role | Updated |
|------|------|---------|
| `CHAT_PREFERENCES.md` | Interaction conventions | Rarely, when conventions change |
| `PROJECT_STATE.md` | Authoritative current project state | Replaced each session |
| `CHAT_NOTES_YYYY-MM-DD.md` | Session diary — what was built, decided, discussed | One per session, never edited after |
| `DESIGN_PILLARS.md` | Game design and narrative direction | As design evolves |
| `CHARACTER_MATRIX.md` | Character system design | As design evolves |
| `DESIGN_REVIEW.md` | Living multi-role code/design review | Updated each session |

### Start of session
Load: `CHAT_PREFERENCES.md` + `PROJECT_STATE.md` + most recent `CHAT_NOTES_YYYY-MM-DD.md`

### End of session
Generate:
- A new `CHAT_NOTES_YYYY-MM-DD.md` — diary only: what was built, key decisions made, notable discussions. No project state, no architecture summaries — those live in `PROJECT_STATE.md`.
- A replacement `PROJECT_STATE.md` — full current project state: architecture decisions, folder structure, vcpkg deps, SDL3 gotchas, working features, tech debt summary, next steps.
- Updated `DESIGN_REVIEW.md` — if any new issues were found or old ones resolved.
