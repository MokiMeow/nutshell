# NutDB Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Complete NutDB's operational README and reverify storage, SQL, MVCC, Raft, recovery, and cluster behavior.

**Architecture:** NutDB already has no Markdown em dashes and no unfinished milestone tasks. This plan adds missing setup and documentation navigation, then treats the full Rust test suite and cluster demo as release proof.

**Tech Stack:** Rust 2021 standard library, TCP, write-ahead log, B-tree, MVCC, Raft, Markdown

## Global Constraints

- Run from `C:/Users/Mohith S/Desktop/nutdb`.
- Preserve the zero-dependency `Cargo.toml`.
- Keep durability and consistency claims aligned with tests and documented limitations.
- Every first-party Markdown file must contain zero em dash characters.
- Use repository commit style and no AI attribution.

---

### Task 1: Finish README navigation and requirements

**Files:**
- Modify: `README.md`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: Existing architecture, durability result, limitations, and demo.
- Produces: Explicit tool requirements and complete documentation navigation.

- [ ] **Step 1: Add requirements**

Add `## Requirements` with Rust stable, Cargo, a loopback TCP-capable host, and
shell requirements for `scripts/cluster-demo.sh`. State that the library has no
third-party crate dependencies.

- [ ] **Step 2: Add documentation navigation**

Add `## Documentation` linking overview, architecture, durability, MVCC, SQL,
Raft, testing, glossary, decisions, and milestones.

- [ ] **Step 3: Verify the punctuation invariant**

```powershell
$dash = [char]0x2014
if (Get-ChildItem -Recurse -File -Filter *.md |
    Where-Object { $_.FullName -notmatch '\\(\.git|target)\\' } |
    Select-String -SimpleMatch $dash) { exit 1 }
```

Expected: exit 0.

- [ ] **Step 4: Commit**

```bash
git add README.md CHANGELOG.md
git commit -m "docs(readme): complete database guide"
```

### Task 2: Reverify database behavior

**Files:**
- Verify: `Cargo.toml`
- Verify: `tests/storage.rs`
- Verify: `tests/crash_recovery.rs`
- Verify: `tests/mvcc.rs`
- Verify: `tests/sql.rs`
- Verify: `tests/raft.rs`
- Verify: `tests/cluster.rs`
- Verify: `tests/randomized_simulation.rs`

**Interfaces:**
- Consumes: Complete storage and distributed database stack.
- Produces: Release build, full test, and real cluster-demo evidence.

- [ ] **Step 1: Run formatting and static diagnostics**

```bash
cargo fmt --check
cargo clippy --all-targets -- -D warnings
```

Expected: both commands exit 0.

- [ ] **Step 2: Build and test**

```bash
cargo build --release
cargo test --all-targets
```

Expected: warning-free release build and every test passes.

- [ ] **Step 3: Run the cluster demonstration**

```bash
bash scripts/cluster-demo.sh
```

Expected: the documented leader, replication, failover, and recovery markers
appear.

- [ ] **Step 4: Merge, push, and watch CI**

```bash
git switch main
git merge --no-ff docs/readme-completion
git push origin main
gh run watch --exit-status
```

Expected: clean synchronized `main` and successful CI.
