# Contributing to Nutshell

Thanks for looking under the hood. This is primarily a learning/portfolio
project, but clean contributions are welcome.

## Before you start

- Read [AGENTS.md](AGENTS.md) — it is the operating manual for this repo and
  applies to humans too.
- Skim [docs/00-overview.md](docs/00-overview.md) and the
  [roadmap](docs/04-roadmap.md).

## Workflow

1. Pick the lowest-numbered unfinished milestone in
   [docs/milestones/](docs/milestones/), or an open issue.
2. Branch from `main`: `git checkout -b milestone-3-interrupts`.
3. Implement it. Keep the build green at every commit
   (`make clean && make iso` must pass; `make run` must boot).
4. Update the relevant doc and tick the Definition of Done.
5. Open a PR into `main`. CI must be green.

## Commit style

`type(scope): outcome` in the imperative, lower case. Types: `feat`, `fix`,
`docs`, `refactor`, `build`, `chore`, `test`. No AI/co-author trailers.

Example: `feat(pic): remap the 8259 PIC to vectors 32-47`.

## Code style

See §4 of [AGENTS.md](AGENTS.md). Short version: C11, 4-space indent,
`snake_case`, freestanding only (no libc), clean under `-Wall -Wextra`,
comment *why* not *what*.

## Reporting issues

Say what you did, what you expected, and what happened. For boot failures,
include the QEMU serial log (`make run` prints it) and your host/toolchain
versions.
