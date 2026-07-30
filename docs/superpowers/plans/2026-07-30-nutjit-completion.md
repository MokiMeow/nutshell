# NutJIT Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Complete NutJIT's project guide, remove all first-party Markdown em dashes, and reverify language semantics, machine encodings, and the benchmark.

**Architecture:** Preserve the completed lexer, parser, interpreter, and x86-64 backend. Add clearer architecture and limitation guidance, then use differential tests as the correctness authority.

**Tech Stack:** C++17, x86-64 machine code, GNU Make, Bash, Markdown

## Global Constraints

- Run from `C:/Users/Mohith S/Desktop/nutjit`.
- Keep the JIT dependency-free and retain the interpreter as the oracle.
- Do not alter benchmark claims without a fresh measurement.
- Every first-party Markdown file must contain zero em dash characters.
- Use repository commit style and no AI attribution.

---

### Task 1: Complete the README and style cleanup

**Files:**
- Modify: `README.md`
- Modify: `docs/04-roadmap.md`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: Existing language examples, measured benchmark, and architecture docs.
- Produces: README architecture, limitations, documentation links, and zero em dashes.

- [ ] **Step 1: Add architecture and limitation sections**

Add an execution pipeline showing `source -> lexer -> parser -> AST ->
interpreter or x86-64 codegen -> executable memory`. Add limitations covering
x86-64 only, integer language semantics, no garbage collector, no native object
format, and no optimization claims beyond those measured. Add documentation
links to parser, codegen, JIT memory, calling convention, optimization, and
testing.

- [ ] **Step 2: Rewrite the three roadmap em dashes**

Edit lines 20 through 22 of `docs/04-roadmap.md` with context-appropriate
punctuation.

- [ ] **Step 3: Verify Markdown**

```powershell
$dash = [char]0x2014
if (Get-ChildItem -Recurse -File -Filter *.md |
    Where-Object { $_.FullName -notmatch '\\(\.git|build)\\' } |
    Select-String -SimpleMatch $dash) { exit 1 }
```

Expected: exit 0.

- [ ] **Step 4: Commit**

```bash
git add README.md CHANGELOG.md docs
git commit -m "docs(readme): complete jit guide"
```

### Task 2: Reverify generated code

**Files:**
- Verify: `tests/run-tests.sh`
- Verify: `tests/check-encodings.sh`
- Verify: `Makefile`

**Interfaces:**
- Consumes: JIT and interpreter implementations.
- Produces: Differential, encoding, sample, and benchmark evidence.

- [ ] **Step 1: Clean build and tests**

```bash
make clean && make all
make test
```

Expected: warning-free build, all semantic comparisons pass, and all encoding
checks pass.

- [ ] **Step 2: Run examples and benchmark**

```bash
make run
make bench | tee build/benchmark.txt
```

Expected: sample programs return their documented values and the benchmark
completes with finite positive timings.

- [ ] **Step 3: Merge, push, and watch CI**

```bash
git switch main
git merge --no-ff docs/readme-completion
git push origin main
gh run watch --exit-status
```

Expected: clean synchronized `main` and successful CI.
