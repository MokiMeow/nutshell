# NutLLM Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement and measure the remaining packed matrix kernel, preserve the fastest correct production path, remove all first-party Markdown em dashes, and republish verified NutLLM documentation.

**Architecture:** Add packing as a separate matmul kernel so the existing scalar, blocked, and SIMD kernels remain permanent references. Correctness tests run packed output across tail-heavy sizes before benchmarks decide whether packing belongs in the preferred path.

**Tech Stack:** C++17, AVX2/FMA with scalar fallback, GNU Make, Bash, Markdown

## Global Constraints

- Run from `C:/Users/Mohith S/Desktop/nutllm`.
- Keep the runtime dependency-free and C++17.
- Build with `-Wall -Wextra -Werror`.
- Keep reference kernels permanently.
- Retain packing in the production path only if measured performance does not materially regress.
- Every first-party Markdown file must contain zero em dash characters.
- Use repository commit style and no AI attribution.

---

### Task 1: Add a failing packed-kernel correctness gate

**Files:**
- Modify: `include/matmul.hpp`
- Modify: `src/main.cpp`

**Interfaces:**
- Consumes: `Matrix`, `matmul_naive`, and `max_diff`.
- Produces: `void matmul_packed(const Matrix &, const Matrix &, Matrix &)` as a required link symbol and packed-difference test output.

- [ ] **Step 1: Declare the packed kernel**

Add to `include/matmul.hpp`:

```cpp
void matmul_packed(const Matrix &a, const Matrix &b, Matrix &c);
```

- [ ] **Step 2: Add packed output to every correctness size**

In `run_correctness()`, call `matmul_packed(a, b, got)`, calculate
`d_packed`, include it in `pass`, and print it alongside blocked and SIMD
differences for sizes `1, 7, 8, 9, 64, 96`.

- [ ] **Step 3: Verify the test fails before implementation**

```bash
make clean
make all
```

Expected: link failure naming undefined `matmul_packed`.

### Task 2: Implement isolated A and B panel packing

**Files:**
- Create: `src/packing.cpp`
- Modify: `include/matmul.hpp`

**Interfaces:**
- Consumes: Row-major `Matrix` storage.
- Produces: Shape-checked packed GEMM with exact interface `void matmul_packed(const Matrix &, const Matrix &, Matrix &)`.

- [ ] **Step 1: Implement shape validation and scratch ownership**

Use fixed tiles:

```cpp
constexpr size_t kPackM = 64;
constexpr size_t kPackN = 128;
constexpr size_t kPackK = 128;
```

Allocate reusable `thread_local std::vector<float>` buffers sized for one A
panel and one B panel. Resize only when capacity is insufficient.

- [ ] **Step 2: Pack both panels**

Pack A as contiguous `(i, k)` rows and B as contiguous `(k, j)` rows for every
clipped tile. Zero C once before the tile loops.

- [ ] **Step 3: Add the portable packed micro-kernel**

Accumulate each packed tile with `i, k, j` loops. Under `__AVX2__` and
`__FMA__`, process eight columns with `_mm256_loadu_ps`,
`_mm256_fmadd_ps`, and `_mm256_storeu_ps`; use the scalar tail for remaining
columns. The non-AVX2 build uses the same scalar loop.

- [ ] **Step 4: Run the correctness gate**

```bash
make clean && make test
```

Expected: all packed differences remain below `1e-3`, including sizes 7, 9,
and 96.

- [ ] **Step 5: Commit**

```bash
git add include/matmul.hpp src/main.cpp src/packing.cpp
git commit -m "perf(matmul): add packed panel kernel"
```

### Task 3: Measure packing and choose the production result honestly

**Files:**
- Modify: `src/main.cpp`
- Modify: `docs/05-kernels.md`
- Modify: `docs/09-testing-and-benchmarking.md`
- Modify: `docs/milestones/milestone-6-polish.md`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: `matmul_packed` and the existing `best_seconds` benchmark helper.
- Produces: Packed timing row and a documented keep-or-reject decision.

- [ ] **Step 1: Add packed timing**

Time `matmul_packed` with `fast_runs` and print a `packed` row with milliseconds,
GFLOP/s, and speedup.

- [ ] **Step 2: Run controlled measurements**

```bash
make clean && make all
./build/nutllm 512 | tee build/packing-512.log
./build/nutllm 1024 | tee build/packing-1024.log
```

Expected: both runs pass correctness before printing timings.

- [ ] **Step 3: Apply the decision rule**

If packed median or best-of-N results are not materially worse at both sizes,
document it as the preferred large-GEMM path. If it regresses, keep the
experimental kernel available but retain the existing SIMD kernel as
production. Record actual values either way.

- [ ] **Step 4: Mark the optional experiment complete**

Check the packing item in `docs/milestones/milestone-6-polish.md` and describe
the measured outcome, including a negative result if that is what the machine
shows.

- [ ] **Step 5: Commit**

```bash
git add src/main.cpp docs CHANGELOG.md
git commit -m "docs(perf): record packed kernel result"
```

### Task 4: Complete the README and remove all Markdown em dashes

**Files:**
- Modify: `README.md`
- Modify: `CHANGELOG.md`
- Modify: `docs/**/*.md`

**Interfaces:**
- Consumes: Existing TinyLlama and llama.cpp evidence plus the new packing result.
- Produces: Complete requirements, architecture, limitations, documentation links, and zero em dashes.

- [ ] **Step 1: Add missing README sections**

Add `## Architecture`, `## Requirements`, `## Limitations`, and
`## Documentation`. Cover tensor kernels, transformer blocks, loaders,
quantization, KV cache, tokenizer, model CLI, AVX2 portability, model files
outside CI, supported model formats, and the exact benchmark scripts.

- [ ] **Step 2: Rewrite all 83 first-party Markdown em dashes**

Edit each occurrence according to its sentence. Do not change technical meaning
or measured values.

- [ ] **Step 3: Verify zero occurrences**

```powershell
$dash = [char]0x2014
$hits = Get-ChildItem -Recurse -File -Filter *.md |
  Where-Object { $_.FullName -notmatch '\\(\.git|build)\\' } |
  Select-String -SimpleMatch $dash
if ($hits) { $hits; exit 1 }
```

Expected: no output.

- [ ] **Step 4: Commit**

```bash
git add README.md CHANGELOG.md docs
git commit -m "docs(readme): complete inference guide"
```

### Task 5: Run final portable and model verification

**Files:**
- Verify: `Makefile`
- Verify: `scripts/verify-tinyllama.sh`
- Verify: `.github/workflows/ci.yml`

**Interfaces:**
- Consumes: Complete kernel and model runtime.
- Produces: Native, portable, fixture, model, and CI proof.

- [ ] **Step 1: Native clean gate**

```bash
make clean && make test
```

Expected: warning-free build and every optimized path agrees with references.

- [ ] **Step 2: Portable build gate**

```bash
make clean
make ARCH="-mavx2 -mfma" test
```

Expected: warning-free portable AVX2/FMA build and passing tests.

- [ ] **Step 3: Run the checked-in tiny model fixtures**

```bash
./build/nutllm --model tests/fixtures/tiny-llama2.bin \
  --tokenizer tests/fixtures/tiny-llama2.tokenizer \
  --prompt "Once upon a time" --max-tokens 4
```

Expected: exit 0 and generated output beginning with the prompt.

- [ ] **Step 4: Merge, push, and watch CI**

```bash
git switch main
git merge --no-ff perf/packed-kernel
git push origin main
gh run watch --exit-status
```

Expected: clean synchronized `main` and successful CI.
