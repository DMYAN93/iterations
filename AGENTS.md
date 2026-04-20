# Iterations — AI Agent Instructions

This file defines how an AI agent should behave, code, and format its responses in this repository.

## Read-First (Mandatory)
Before doing anything else:
1) Read `README.md` for technical setup, architecture, and project state.
2) Read `DESIGN.md` for the game's core concept and design goals.

Hard stop: If you cannot access these files, say so clearly and stop. Do not guess.

## Developer Context & Persona
- The developer has a CS degree, but is ~10 years away from active development and returning through this project.
- Explain the **why**, not just the what. Prose over bullet points for explanations.
- Explain modern C++ conventions (smart pointers, `auto`, lambdas, structured bindings, `std::optional`, move semantics) and SDL3/ECS patterns.
- Don’t over-explain CS fundamentals (pointers, OOP, complexity).
- Be direct about trade-offs — present options honestly rather than steering toward one.

## Response Formatting Preferences
- **Show the full file first**, then explain. Never show partial files unless discussing a specific isolated change.
- **One file per message.** After showing a file, ask if ready to proceed to the next.
- Show the **filename as a label in the response text** before the code block, so it's clear where the file lives. Never put it as a comment inside the code itself.
- One topic at a time. Keep responses focused.
- When a side question comes up mid-walkthrough, answer it, then offer to continue.

## Code Conventions
- No single-letter variable names except loop iterators (`i`, `j`, etc.).
- Includes are rooted at `src/`: `#include "core/Types.h"` (no `../` relative paths).
- Each `.cpp` explicitly includes what it directly uses (no reliance on transitive includes).
- Systems know only about their own components (no cross-system dependencies).

## Agent Workflow Rules (Planning, Edits, Verification)
- Prefer the smallest correct change; avoid drive-by refactors.
- **Design discussion before every new feature** — lay out the plan, files to touch, key decisions and trade-offs, then ask to proceed.
- Do not claim commands/tests were run unless you include the relevant output/log excerpt.
- If a build/test cannot be run (missing tools, permissions, network), state exactly:
  - what you attempted
  - the blocking error
  - the next command you would run in a working environment
- At the end of a feature milestone, include a **"How to test"** section: what to expect visually, what to interact with, what the SDL log should show.

## Git Safety Rules (HARD STOP — Enforceable)
Goal: AI agents must not modify `main` (locally or on the remote) unless explicitly instructed to do so.

### Absolute Prohibitions (Unless explicitly requested)
Agents MUST NOT:
- Push to `origin/main` (including `git push origin main`, `git push`, or any push while checked out on `main`).
- Merge anything into `main` (GitHub “Merge” button, `git merge` into `main`, “rebase and merge”, squash merge, etc.).
- Rebase or rewrite `main` history (`git rebase` while on `main`, `git reset --hard` affecting `main`, `git push --force` of any branch, especially `main`).
- Change repo settings (branch protection, default branch, Actions settings, permissions).
- Delete remote branches (`git push origin --delete ...`) unless explicitly requested.

### Required Pre-Flight Checks
Before running any git command other than pure read-only commands, the agent MUST:
1) Print the current branch and status:
   - `git branch --show-current`
   - `git status --porcelain -b`
2) If the current branch is `main`, HARD STOP and ask for instructions.

### Allowed Read-Only Git Commands (Always allowed)
- `git status`, `git diff`, `git log`, `git show`, `git branch`, `git remote -v`, `git fetch` (These do not change `main`.)

### Allowed Write Actions (Only on non-main branches)
Agents MAY:
- Create branches (e.g., `codex/*`, `claude/*`, `fix-*`) and commit there.
- Push non-main branches (e.g., `git push -u origin fix-*`), provided:
  - they are NOT currently on `main`, and
  - they show the pre-flight check output first.
- Open PRs targeting `main` for review, but MUST NOT merge them.

### "Merge Permission" Protocol (Explicit approval requirement)
If the agent believes changes should be merged to `main`, it MUST:
1) Stop and summarize exactly what would be merged (branch name + commit list).
2) Ask for approval with a single yes/no question.
3) Proceed ONLY if the human replies with the exact phrase: **"APPROVED: MERGE TO MAIN"**.

If that exact phrase is not present, the agent must treat it as NOT approved.

### Safety Default
When unsure, default to: create a new branch, commit there, push that branch, and open a PR for review.

## Session Structure
- **Start of session:** Load `README.md` (Project State) + most recent `docs/chat_notes/CHAT_NOTES_YYYY-MM-DD.md`.
- **End of session:**
  - Generate a new `docs/chat_notes/CHAT_NOTES_YYYY-MM-DD.md` (diary only: what was built, key decisions made, notable discussions). No project state/architecture summaries — those live in `README.md`.
  - Propose updates to `README.md` (full project state, architecture decisions, new dependencies, tech debt additions/removals) and `DESIGN.md` (if design evolved).