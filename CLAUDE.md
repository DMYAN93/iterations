# Iterations — Claude Code Context

Read `docs/PROJECT_STATE.md` for full architecture, folder structure, vcpkg dependencies, SDL3 gotchas,
working features, and known tech debt before starting any task.

Read `docs/CHAT_PREFERENCES.md` for interaction conventions.

---

## Developer Background
- CS degree, ~10 years out of active development. Returning through this project.
- Explain the **why** behind modern C++ conventions and SDL3/ECS patterns.
- Don't over-explain CS fundamentals (pointers, OOP, complexity).

---

## Code Conventions
- No single-letter variable names except loop iterators (`i`, `j`, etc.).
- All includes use `src/` as root: `#include "core/Types.h"` — no relative paths.
- Each `.cpp` explicitly includes what it directly uses — no relying on transitive includes.
- Systems know only about their own components — no cross-system dependencies.

---

## Response Conventions
- Show the **filename as a label** before each code block. Never as a comment inside the code.
- **One file per response.** After showing a file, ask if ready to proceed to the next.
- Show the **full file**, not a partial diff, unless discussing a specific isolated change.
- Explain the **why**, not just the what. Prose over bullet points.
- **Design discussion before every new feature** — plan files, decisions, trade-offs, then ask to proceed.
- At the end of a milestone, include a **"How to test"** section.
