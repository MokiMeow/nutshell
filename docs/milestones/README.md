# Milestones

Each milestone is a self-contained step that leaves a **bootable** kernel
behind. Build them in order (see the [roadmap](../04-roadmap.md) for the
dependency graph). Do exactly one per pass; keep the build green.

| # | Milestone | State |
|---|-----------|-------|
| 0 | [Boot](milestone-0-boot.md) | ✅ done |
| 1 | [I/O — `kprintf`](milestone-1-io.md) | ✅ done |
| 2 | [GDT](milestone-2-gdt.md) | ✅ done |
| 3 | [Interrupts (IDT + PIC + PIT)](milestone-3-interrupts.md) | ⬜ |
| 4 | [Keyboard](milestone-4-keyboard.md) | ⬜ |
| 5 | [Memory (PMM + heap)](milestone-5-memory.md) | ⬜ |
| 6 | [The shell](milestone-6-shell.md) | ⬜ |
| 7 | [Polish](milestone-7-polish.md) | ⬜ |

## Every milestone spec has

- **Goal** — one sentence.
- **Concepts** — what you'll learn.
- **Tasks** — an ordered checklist.
- **Files** — what to add/change.
- **Definition of Done** — the objective bar; don't tick it without booting.
- **References** — the canonical sources.

## The Builder's loop (from AGENTS.md)

1. Pick the lowest-numbered unfinished milestone.
2. Implement its tasks, keeping `make iso` clean and `make run` booting.
3. Verify against the Definition of Done (actually boot it).
4. Update the concept doc, tick the DoD, update the roadmap table and CHANGELOG.
5. Commit (`feat(scope): …`), open a PR, keep CI green. Stop for review.
