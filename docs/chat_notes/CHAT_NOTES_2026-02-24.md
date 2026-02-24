# Iterations — Dev Diary 2026-02-24

Organisational session. No code written, but the tooling and documentation structure is now
significantly cleaner and the workflow for future sessions is better defined.

---

## What Was Done

### Documentation restructure
Identified that `CHAT_PREFERENCES.md` was bloated with project state and out of date relative
to the 2026-02-23 notes. Cleaned it to contain only interaction conventions. Introduced
`PROJECT_STATE.md` as a dedicated living document for current project state, replacing the
state sections that were being duplicated across every session's notes. Session notes are now
diary-only going forward — much shorter.

Clarified the filename convention: filename as a label in response text before the code block,
never as a comment inside the code itself. Previous confusion was because it had been done
both ways at different points.

### File approach formalised
Six-document structure agreed and documented in `CHAT_PREFERENCES.md`:
- `CHAT_PREFERENCES.md` — interaction conventions, rarely changes
- `PROJECT_STATE.md` — replaced each session
- `CHAT_NOTES_YYYY-MM-DD.md` — diary only, one per session, permanent
- `DESIGN_PILLARS.md` — game design and narrative
- `CHARACTER_MATRIX.md` — character system design
- `DESIGN_REVIEW.md` — living multi-role review

### CLAUDE.md created
Lean auto-load file for Claude Code in VS Code. Points to `PROJECT_STATE.md` and
`CHAT_PREFERENCES.md` for full context, with key conventions inlined so a session is useful
even if the model skips reading the full files.

### Two bug fixes identified and specified (not yet implemented)
From the Design Review, two issues identified as blocking the next steps:
- Delta time unclamped — will cause teleporting on debugger resume, and matters immediately
  for main menu navigation. Fix: clamp to 100ms in `Game::Run()`.
- `InputManager::BeginFrame` ordering — input is one frame late, will feel broken for menu
  key presses. Fix: call `ProcessInput()` before `m_input.BeginFrame()` in the game loop.

Both fixes are in `Game.cpp` only. Full corrected file was produced in this session but not
yet committed — apply via Claude Code locally or manually.

### Tooling investigation
Confirmed GitHub sync to claude.ai Projects is not pulling source files — known platform
regression, multiple reported instances. Markdown files sync correctly, source files do not.
Investigated the three Claude variants and agreed on a workflow split (see below).

---

## Tooling Workflow Agreed

**claude.ai (Projects chat)** — design discussions, architecture decisions, web search,
document generation, session-end file production. Primary tool for planning.

**Claude Code in VS Code** — implementation. Reads `CLAUDE.md` automatically, has full
local file access. Primary tool for writing code.

**Claude Code on the web** — not useful at current project stage. Designed for async
fire-and-forget tasks against a stable codebase with well-defined issues. Revisit later.

---

## Next Steps

- Apply the two `Game.cpp` fixes (delta clamp + input ordering) in Claude Code locally
- Main menu state — design discussion first, then implementation
- Frame cap / vsync
