# NutFuzz Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Complete NutFuzz's safe local engine gaps, validate subprocess feedback on owned synthetic targets, remove all first-party Markdown em dashes, and document the external campaign boundary honestly.

**Architecture:** Extend the existing dependency-free fuzzer in layers: scheduling and progress, stability and trimming, structured mutation, constrained execution, then a versioned cooperative forkserver for local external coverage. Keep exec-per-run as the reference path and keep all real-world targeting outside this plan unless explicit authorization is documented.

**Tech Stack:** Rust 2021 standard library only, Unix process APIs through minimal FFI, Bash, optional local AFL++, Markdown

## Global Constraints

- Run from `C:/Users/Mohith S/Desktop/nutfuzz`.
- Keep `[dependencies]` empty.
- Use only repository-owned synthetic targets for fuzzing and benchmarks.
- Do not probe, fuzz, or contact an external system.
- Do not claim a third-party vulnerability, disclosure, or maintainer response.
- Keep Windows builds compiling; Linux-only runtime features must use `#[cfg(target_os = "linux")]`.
- Every first-party Markdown file must contain zero em dash characters.
- Use repository commit style and no AI attribution.

---

### Task 1: Add deterministic scheduler comparison and progress reporting

**Files:**
- Modify: `src/corpus.rs`
- Modify: `src/fuzzer.rs`
- Modify: `src/main.rs`
- Modify: `tests/coverage_wins.rs`

**Interfaces:**
- Consumes: `Corpus`, `Rng`, `Config`, and `Report`.
- Produces: `Schedule::{Weighted, RoundRobin}`, `Progress`, and `fuzz_with_progress`.

- [ ] **Step 1: Write failing scheduler tests**

Add tests asserting that `pick_round_robin()` visits indices
`0, 1, 2, 0`, while weighted selection remains deterministic for a fixed RNG
seed.

- [ ] **Step 2: Run the failing tests**

```bash
cargo test corpus::tests::round_robin_visits_every_entry_in_order
```

Expected: compile failure because `pick_round_robin` does not exist.

- [ ] **Step 3: Add the scheduler interface**

Add:

```rust
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Schedule {
    Weighted,
    RoundRobin,
}
```

Store a private round-robin cursor in `Corpus`, keep weighted logic in
`pick_weighted`, and dispatch from `pick(schedule, rng)`.

- [ ] **Step 4: Add progress without storing callbacks in Config**

Add:

```rust
#[derive(Clone, Debug)]
pub struct Progress {
    pub executions: u64,
    pub corpus_size: usize,
    pub favoured: usize,
    pub edges_found: usize,
    pub unique_bugs: usize,
    pub elapsed_secs: f64,
}

pub fn fuzz_with_progress<F>(
    target: TargetFn,
    seeds: &[Vec<u8>],
    config: &Config,
    on_progress: F,
) -> Report
where
    F: FnMut(&Progress);
```

Add `schedule: Schedule` and `progress_interval: u64` to `Config`. Keep
`fuzz()` as a wrapper with a no-op callback.

- [ ] **Step 5: Add CLI live stats and a fixed-seed comparison test**

Print progress to stderr only when the interval is nonzero. Add a deterministic
test comparing weighted and round-robin with the same target, seeds, seed, and
iteration budget. Assert reproducibility and report edges per execution without
requiring weighted scheduling to win every run.

- [ ] **Step 6: Run and commit**

```bash
cargo fmt --check
cargo test
git add src/corpus.rs src/fuzzer.rs src/main.rs tests/coverage_wins.rs
git commit -m "feat(schedule): add progress and comparison modes"
```

### Task 2: Detect unstable coverage and trim corpus entries

**Files:**
- Create: `src/trim.rs`
- Modify: `src/lib.rs`
- Modify: `src/fuzzer.rs`
- Modify: `tests/coverage_wins.rs`

**Interfaces:**
- Consumes: `TargetFn`, `Coverage::snapshot`, and guided corpus admission.
- Produces: `trim_coverage`, `Stability`, `unstable_inputs`, and `trimmed_bytes`.

- [ ] **Step 1: Write failing trim and instability tests**

Use a target backed by `AtomicU64` that alternates one edge between executions.
Assert it is reported unstable and not admitted as stable new coverage. Use a
separate target where only `b"KEY"` controls coverage and assert
`b"xxxxKEYyyyy"` trims to the smallest coverage-equivalent input.

- [ ] **Step 2: Define the interfaces**

In `src/trim.rs` add:

```rust
pub fn trim_coverage<F>(input: &[u8], preserves: F) -> Vec<u8>
where
    F: FnMut(&[u8]) -> bool;
```

In `src/fuzzer.rs` add:

```rust
#[derive(Clone, Debug, Default)]
pub struct Stability {
    pub checks: u64,
    pub unstable_inputs: u64,
}
```

Add `stability_runs: u8` and `trim_corpus: bool` to `Config`, plus
`stability` and `trimmed_bytes` to `Report`.

- [ ] **Step 3: Implement coverage equality and admission order**

For interesting candidates, rerun coverage `stability_runs` times and compare
sorted snapshots exactly. Skip trimming and corpus admission when snapshots
differ. For stable inputs, call `trim_coverage` with an oracle that requires
the exact same snapshot, then add the trimmed input.

- [ ] **Step 4: Run focused and full tests**

```bash
cargo test unstable
cargo test trim
cargo test
```

Expected: all tests pass and prior deterministic campaign iteration counts
remain intentionally updated or unchanged.

- [ ] **Step 5: Commit**

```bash
git add src/trim.rs src/lib.rs src/fuzzer.rs tests/coverage_wins.rs
git commit -m "feat(coverage): detect instability and trim corpus"
```

### Task 3: Add automatic tokens, grammar generation, and checksum fixups

**Files:**
- Create: `src/structure.rs`
- Modify: `src/lib.rs`
- Modify: `src/dictionary.rs`
- Modify: `src/mutate.rs`
- Modify: `src/fuzzer.rs`
- Modify: `src/target.rs`
- Modify: `tests/coverage_wins.rs`

**Interfaces:**
- Consumes: `Dictionary`, `havoc`, and corpus admission.
- Produces: `harvest_tokens`, `mutate_token`, `Grammar::Json`, and `FixupFn`.

- [ ] **Step 1: Write failing behavior tests**

Add tests proving that printable runs of length 4 through 32 are harvested
without duplicates, token replacement preserves surrounding tokens, the JSON
grammar always emits balanced objects, and a checksum fixup makes a
checksum-gated target reachable.

- [ ] **Step 2: Define focused interfaces**

```rust
pub type FixupFn = fn(&mut Vec<u8>);

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Grammar {
    Json,
}

pub fn harvest_tokens(input: &[u8]) -> Vec<Vec<u8>>;
pub fn generate(grammar: Grammar, rng: &mut Rng) -> Vec<u8>;
pub fn mutate_token(input: &mut Vec<u8>, dictionary: &Dictionary, rng: &mut Rng);
```

Add `auto_dictionary: bool`, `grammar: Option<Grammar>`, and
`fixup: Option<FixupFn>` to `Config`.

- [ ] **Step 3: Integrate at one mutation boundary**

Clone the configured dictionary into campaign-local mutable state. Harvest
tokens only from stable coverage-increasing inputs. Choose grammar generation
as one mutation strategy. Apply token mutation after havoc when tokens exist.
Apply `fixup` exactly once after every mutation and before target execution,
including trimming oracles.

- [ ] **Step 4: Add an owned checksum target**

Add a named synthetic target whose final byte is the wrapping sum of preceding
bytes and whose guarded branch requires a structured token. Provide its fixup
function and a test showing the gate is unreachable without fixup in the same
budget but reachable with it.

- [ ] **Step 5: Run and commit**

```bash
cargo fmt --check
cargo test dictionary
cargo test checksum
cargo test
git add src tests
git commit -m "feat(structure): add learned tokens grammar and fixups"
```

### Task 4: Enforce subprocess resource limits

**Files:**
- Create: `src/limits.rs`
- Modify: `src/lib.rs`
- Modify: `src/exec.rs`
- Modify: `tests/external_cli.rs`

**Interfaces:**
- Consumes: `ExternalTarget` and `CommandExt::pre_exec`.
- Produces: `ResourceLimits` and `ExternalTarget::with_limits`.

- [ ] **Step 1: Write a Linux-only failing test**

Create a helper invocation that attempts to write beyond a small file-size
limit. Assert the run is terminated or classified as a limit signal rather
than hanging or growing without bound.

- [ ] **Step 2: Define limits**

```rust
#[derive(Clone, Copy, Debug, Default)]
pub struct ResourceLimits {
    pub address_space_bytes: Option<u64>,
    pub cpu_seconds: Option<u64>,
    pub file_size_bytes: Option<u64>,
}
```

Add `limits: ResourceLimits` to `ExternalTarget` and a consuming
`with_limits(ResourceLimits) -> Self` builder.

- [ ] **Step 3: Apply Linux limits before exec**

Under `#[cfg(target_os = "linux")]`, use `CommandExt::pre_exec` and a minimal
`extern "C"` declaration for `setrlimit`. Set core size to zero and apply only
configured CPU, address-space, and file-size limits. Return an I/O harness error
if setup fails. On other platforms, reject nonempty limits explicitly.

- [ ] **Step 4: Run and commit**

```bash
cargo test exec
cargo test external
cargo test
git add src/limits.rs src/lib.rs src/exec.rs tests/external_cli.rs
git commit -m "feat(exec): constrain external target resources"
```

### Task 5: Add a cooperative local forkserver with coverage replies

**Files:**
- Create: `src/protocol.rs`
- Create: `src/forkserver.rs`
- Create: `src/bin/nutfuzz-forkserver-target.rs`
- Modify: `src/lib.rs`
- Modify: `src/exec.rs`
- Modify: `src/external.rs`
- Create: `tests/forkserver.rs`

**Interfaces:**
- Consumes: Owned synthetic target logic and subprocess execution result types.
- Produces: Versioned framed protocol, `Executor` abstraction, and reusable local forkserver client.

- [ ] **Step 1: Write protocol round-trip tests**

Define protocol version 1 with hello, run, and result frames. Test truncated,
oversized, wrong-version, clean, crash, hang, and coverage-bearing replies.

- [ ] **Step 2: Define the executor boundary**

```rust
pub trait Executor {
    fn run(&mut self, input: &[u8]) -> RunResult;
}
```

Implement it for `ExternalTarget` without changing current behavior. Change
external campaign internals to accept `&mut dyn Executor`.

- [ ] **Step 3: Define the protocol**

Use little-endian `u32` length-prefixed frames with:

```rust
pub const PROTOCOL_VERSION: u32 = 1;

pub struct ForkserverReply {
    pub result: RunResult,
    pub coverage: Vec<(usize, u8)>,
}
```

Reject frames larger than 64 KiB plus protocol overhead.

- [ ] **Step 4: Implement the owned Linux harness**

The harness initializes once, reads one framed input, forks, evaluates only the
repository's synthetic target in the child, writes status and coverage through
a dedicated pipe, waits in the parent, and repeats. Child execution must use
`_exit`; the parent must close unused descriptors. Non-Linux builds print an
unsupported message and exit 2.

- [ ] **Step 5: Implement and test the client**

`ForkserverTarget::spawn(path, timeout)` performs the version handshake and
implements `Executor`. Test repeated clean runs, deterministic coverage,
crash classification, timeout recovery, and clean shutdown using only
`CARGO_BIN_EXE_nutfuzz-forkserver-target`.

- [ ] **Step 6: Run and commit**

```bash
cargo fmt --check
cargo test --test forkserver
cargo test
git add src tests
git commit -m "feat(exec): add local coverage forkserver"
```

### Task 6: Add safe comparison and presentation artifacts

**Files:**
- Create: `bench/afl_target.c`
- Create: `scripts/benchmark-aflpp.sh`
- Create: `docs/assets/campaign.cast`
- Modify: `README.md`
- Modify: `docs/04-roadmap.md`
- Modify: `docs/milestones/milestone-2-scheduling.md`
- Modify: `docs/milestones/milestone-3-external-targets.md`
- Modify: `docs/milestones/milestone-4-structure.md`
- Modify: `docs/milestones/milestone-6-polish.md`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: Completed local engine and included synthetic target.
- Produces: Reproducible local comparison script, authentic demo recording, and accurate milestone state.

- [ ] **Step 1: Add an opt-in AFL++ comparison**

The script must require `afl-fuzz` and `afl-clang-fast` on PATH, compile only
`bench/afl_target.c`, use fixed seeds and a fixed time budget, write all outputs
under `build/aflpp`, and print `SKIP` with exit 0 when tools are absent. It must
never download, install, or target external code.

- [ ] **Step 2: Capture an authentic local demo**

Run a fixed-seed NutFuzz campaign against the included target and encode the
actual terminal output into asciinema v2 events in `docs/assets/campaign.cast`.
Do not hand-invent findings or timing values.

- [ ] **Step 3: Update milestone truth**

Check completed normal engine tasks and record measured scheduler, forkserver,
and structured-input results. Keep the real third-party campaign and private
disclosure items unchecked and explicitly outside the authorized local scope.

- [ ] **Step 4: Commit**

```bash
git add bench scripts docs README.md CHANGELOG.md
git commit -m "test(benchmark): compare owned synthetic campaigns"
```

### Task 7: Complete all Markdown and README presentation

**Files:**
- Modify: `README.md`
- Modify: `CHANGELOG.md`
- Modify: `docs/**/*.md`

**Interfaces:**
- Consumes: Fresh local measurements and accurate milestone state.
- Produces: Complete architecture, requirements, limitations, ethics, documentation links, and zero em dashes.

- [ ] **Step 1: Add missing README sections**

Add `## Architecture`, `## Requirements`, `## Limitations`, and
`## Documentation`. Explain in-process versus external feedback, supported
platforms, deterministic seeds, corpus behavior, the local-only forkserver,
resource controls, expected false negatives, and the explicit authorization
boundary.

- [ ] **Step 2: Rewrite all 11 current em dashes plus any introduced later**

Use contextual punctuation and preserve all technical meaning.

- [ ] **Step 3: Assert zero em dashes**

```powershell
$dash = [char]0x2014
$hits = Get-ChildItem -Recurse -File -Filter *.md |
  Where-Object { $_.FullName -notmatch '\\(\.git|target|build)\\' } |
  Select-String -SimpleMatch $dash
if ($hits) { $hits; exit 1 }
```

Expected: no output.

- [ ] **Step 4: Commit**

```bash
git add README.md CHANGELOG.md docs
git commit -m "docs(readme): complete defensive fuzzing guide"
```

### Task 8: Run final release verification and publication

**Files:**
- Verify: `Cargo.toml`
- Verify: `tests/*.rs`
- Verify: `.github/workflows/ci.yml`

**Interfaces:**
- Consumes: Complete safe local NutFuzz implementation.
- Produces: Formatting, lint, release, test, campaign, Git, and CI evidence.

- [ ] **Step 1: Run all local gates**

```bash
cargo fmt --check
cargo clippy --all-targets -- -D warnings
cargo build --release
cargo test --all-targets
```

Expected: all commands exit 0 and `[dependencies]` remains empty.

- [ ] **Step 2: Run deterministic owned campaigns twice**

```bash
./target/release/nutfuzz --target nested --seed 12648430 \
  --iterations 300000 | tee build/campaign-a.log
./target/release/nutfuzz --target nested --seed 12648430 \
  --iterations 300000 | tee build/campaign-b.log
sed -E '/execs\/s|elapsed/d' build/campaign-a.log > build/campaign-a.stable
sed -E '/execs\/s|elapsed/d' build/campaign-b.log > build/campaign-b.stable
diff -u build/campaign-a.stable build/campaign-b.stable
```

Expected: deterministic finding data after removing the two explicitly
time-dependent output lines.

- [ ] **Step 3: Run external safety tests**

```bash
cargo test --test external_cli
cargo test --test forkserver
bash scripts/benchmark-aflpp.sh
```

Expected: external and forkserver tests pass; AFL++ either produces owned-target
measurements or reports an honest tool-not-installed skip.

- [ ] **Step 4: Merge, push, and watch CI**

```bash
git switch main
git merge --no-ff milestone-safe-engine-completion
git push origin main
gh run watch --exit-status
git status --short --branch
```

Expected: clean synchronized `main` and successful CI.
