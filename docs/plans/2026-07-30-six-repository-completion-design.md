# Six-Repository Completion and Documentation Design

Date: 2026-07-30

## Objective

Finish and verify the safe, first-party engineering work across Nutshell,
Nutvisor, NutJIT, NutDB, NutFuzz, and NutLLM. Improve each repository's README
without erasing its individual voice, remove em dashes from all first-party
Markdown, and publish only claims supported by reproducible evidence.

## Scope

The completion pass covers:

- First-party source code, tests, scripts, documentation, and CI configuration.
- Every first-party Markdown file in all six repositories.
- NutLLM's optional packed-kernel optimization, subject to correctness and
  non-regression measurements.
- NutFuzz's unfinished local engine features and safe benchmarks.
- Clean builds, complete test suites, project-specific runtime checks, Git
  history, remote branches, release state, and CI results.

The pass excludes:

- Files under `.git`, generated build directories, dependency caches, vendored
  code, and other third-party material.
- Unauthorised testing of external systems.
- Claims that a third-party vulnerability was found or disclosed unless that
  event actually occurs under explicit authorization and can be evidenced.

## Repository Completion Model

Each repository will be handled independently and kept green:

1. Re-read its `AGENTS.md` and milestone specifications.
2. Audit unchecked tasks against the implementation and current project scope.
3. Implement unfinished safe engineering work in small logical units.
4. Run the repository's documented build, test, and runtime verification.
5. Update milestone documents, roadmap, changelog, and README to match reality.
6. Self-review the complete diff for correctness, safety, simplicity, and
   unsupported claims.
7. Commit only verified changes, merge the working branch, push, and inspect
   remote CI.

A repository is not complete merely because a checklist is empty. Its required
verification must pass, the documentation must match the implementation, and
the working tree and remote branch must agree.

## NutLLM Optimization

The remaining optional packing work will be implemented behind the existing
kernel interfaces. Tests will compare packed and reference computations for
correctness across representative shapes and edge cases. Benchmarks will use
the repository's established methodology.

The optimized path will remain enabled only if:

- All correctness tests pass.
- Sanitizer or diagnostic builds remain clean where the repository supports
  them.
- Performance measurements show no material regression in the intended
  workload.

If packing is objectively worse, the experiment and measurements will be
documented honestly and the production path will retain the faster design.

## NutFuzz Completion and Safety Boundary

Normal local engine work includes determinism checks, corpus trimming,
scheduling measurements, live statistics, resource limits, fork-server
improvements, coverage plumbing for local targets, dictionary harvesting,
token-aware mutation, grammar support, checksum fixups, and reproducible
benchmarking.

All fuzzing and benchmark execution will be confined to:

- Targets stored in the repository and owned by the project.
- Purpose-built local test programs.
- Other targets only when explicit authorization is documented.

The real-world third-party campaign and disclosure checklist cannot be made
deterministically complete by code. A genuine defect cannot be promised, and a
private disclosure cannot be claimed without a real authorized finding.
Documentation will distinguish completed engine capability from that external
validation track.

If the Claude CLI and requested Opus model are available, they may be used as a
bounded reviewer for NutFuzz design or patches. No credentials, private data,
or external attack authority will be provided. Suggested work will be treated
as untrusted input and independently reviewed and verified before use.

## README Design

The READMEs will retain their current visual style and project-specific
narrative. They will share a quality standard rather than a rigid template.

Every README will clearly provide, where applicable:

- A concise project identity and current release state.
- The concrete result or measured outcome.
- What the project does and why its implementation is interesting.
- A high-level architecture or execution flow.
- Requirements and platform constraints.
- Exact quick-start, test, benchmark, and verification commands.
- Honest status, guarantees, limitations, and safety boundaries.
- Repository layout and links to deeper documentation.
- License information.

Measured numbers will remain only when their command, environment, and evidence
can be reproduced. Status tables will not mark excluded or unverified work as
complete.

## Markdown Style

Every first-party `*.md` file will be scanned for Unicode em dash characters.
Each occurrence will be edited according to its sentence:

- Use a colon before an explanation.
- Use parentheses for a true aside.
- Use a comma when the clauses remain easy to read.
- Split dense statements into separate sentences.
- Use an ordinary hyphen only for a genuine compound or range.

This is an editorial rewrite, not a blind character substitution. A final
repository-wide scan must report zero em dash characters.

## Verification

Verification will follow each repository's local instructions. At minimum it
will include:

- Clean release builds with warnings treated according to project policy.
- Full automated test suites.
- Nutshell headless QEMU boot and serial-marker assertions.
- Nutvisor runtime checks when `/dev/kvm` is available, with any host limitation
  reported rather than concealed.
- NutJIT output inspection and benchmark tests.
- NutDB persistence, recovery, and durability tests.
- NutFuzz deterministic local target runs and benchmark comparisons.
- NutLLM reference comparisons, portable builds, model runs, and performance
  measurements.
- Zero-em-dash scans and Markdown link checks.
- Clean working trees, synchronized remote branches, and successful remote CI.

## Commit and Publication Strategy

Work will use repository-local branches and small commits with each repository's
required commit style. Product changes, tests, documentation, and release
metadata may be separated when that keeps review clear, but every commit must
leave the repository buildable.

No AI attribution or co-author trailers will be added. Pushes will occur only
after local verification and self-review. The final report will identify exact
commands, results, commits, tags, CI runs, and any external limitation that
prevents a literal claim of completion.
