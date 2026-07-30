# Nutshell Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [x]`) syntax for tracking.

**Goal:** Verify Nutshell from a clean build through a headless shell boot, remove all first-party Markdown em dashes, and make the README a complete and evidence-backed project entry point.

**Architecture:** Product code is already complete. This pass edits only first-party documentation, preserves the current release proof, and reruns the kernel's full QEMU verification before publication.

**Tech Stack:** C11, NASM, GNU Make, GRUB, QEMU, Markdown, GitHub Actions

## Global Constraints

- Run repository commands from `C:/Users/Mohith S/Desktop/nutshell` through Ubuntu WSL when they require the Linux toolchain.
- Keep the kernel freestanding and dependency-free.
- Do not modify product code during this documentation pass unless verification exposes a real defect.
- Every first-party `*.md` file must contain zero Unicode em dash characters.
- Every commit must build and boot successfully and use `type(scope): outcome`.
- Add no AI attribution or co-author trailers.

---

### Task 1: Complete the README and Markdown style pass

**Files:**
- Modify: `README.md`
- Modify: `docs/03-boot-process.md`
- Modify: `docs/10-glossary.md`
- Modify: `docs/decisions/0003-vga-text-mode.md`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: Existing boot GIF, panic screenshot, milestone status, and verification commands.
- Produces: A complete README and a repository-wide zero-em-dash Markdown result.

- [x] **Step 1: Record the punctuation baseline**

Run:

```powershell
$dash = [char]0x2014
Get-ChildItem -Recurse -File -Filter *.md |
  Where-Object { $_.FullName -notmatch '\\(\.git|build)\\' } |
  Select-String -SimpleMatch $dash
```

Expected: exactly three first-party matches, all outside `README.md`.

- [x] **Step 2: Add the missing operational sections**

Add `## Requirements` after Quick Start with the supported WSL2/Linux host,
required x86-64 virtualization, toolchain names, and headless QEMU requirement.
Add `## Limitations` before Repository Layout covering BIOS/GRUB boot, VGA text
mode, PS/2 input, identity mapping, single core, and no userspace or filesystem.
Keep the current status, architecture links, screenshots, and exact shell
commands.

- [x] **Step 3: Rewrite every em dash in context**

Use sentence splits, commas, colons, or parentheses in the four files listed
above. Do not replace an em dash blindly with a hyphen.

- [x] **Step 4: Verify local documentation links and punctuation**

Run:

```powershell
$dash = [char]0x2014
$hits = Get-ChildItem -Recurse -File -Filter *.md |
  Where-Object { $_.FullName -notmatch '\\(\.git|build)\\' } |
  Select-String -SimpleMatch $dash
if ($hits) { $hits; exit 1 }
rg -n '\]\((docs/|assets/)[^)]+\)' README.md
```

Expected: no punctuation hits; every printed local target exists.

- [x] **Step 5: Commit the editorial pass**

```bash
git add README.md CHANGELOG.md docs
git commit -m "docs(readme): complete project guide"
```

### Task 2: Prove the release still builds and boots

**Files:**
- Verify: `Makefile`
- Verify: `tests/run-selftest.sh`
- Verify: `.github/workflows/ci.yml`

**Interfaces:**
- Consumes: Existing kernel and self-test ISO targets.
- Produces: Clean build output, serial boot evidence, and a green self-test.

- [x] **Step 1: Build from a clean tree**

Run in Ubuntu WSL:

```bash
cd /mnt/c/Users/MOHITH~1/Desktop/nutshell
make clean && make iso
```

Expected: exit 0 with no compiler or linker warnings.

- [x] **Step 2: Boot headlessly and assert the shell markers**

```bash
rm -f serial.log
timeout 60 qemu-system-x86_64 -cdrom build/nutshell.iso \
  -display none -serial file:serial.log -no-reboot >/dev/null 2>&1 || true
cat serial.log
for marker in '[ok] gdt' '[ok] pmm-selftest' '[ok] heap-selftest' \
  '[ok] idt' '[ok] keyboard' '[ok] shell' 'nutshell> '; do
  grep -Fq "$marker" serial.log
done
```

Expected: all marker assertions succeed.

- [x] **Step 3: Run the in-kernel self-test image**

```bash
make test
```

Expected: exit 0 with all self-test markers and no timeout classification.

- [x] **Step 4: Publish and inspect CI**

```bash
git switch main
git merge --no-ff docs/six-repo-completion
git push origin main
gh run watch --exit-status
git status --short --branch
```

Expected: CI succeeds and `main` is clean and synchronized with `origin/main`.
