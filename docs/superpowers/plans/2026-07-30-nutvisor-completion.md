# Nutvisor Completion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Publish a complete Nutvisor README, remove all first-party Markdown em dashes, and reverify every KVM guest path.

**Architecture:** Keep the VMM unchanged unless testing exposes a defect. Improve the documentation around host requirements and supported virtualization scope, then run both compile-only and real `/dev/kvm` checks.

**Tech Stack:** GNU C11, NASM, Linux KVM API, GNU Make, Bash, Markdown

## Global Constraints

- Run from `C:/Users/Mohith S/Desktop/nutvisor`.
- Preserve the dependency-free KVM implementation and current guest ABI.
- Do not claim runtime verification when `/dev/kvm` is unavailable.
- Every first-party Markdown file must contain zero em dash characters.
- Use repository commit style and no AI attribution.

---

### Task 1: Improve the README and documentation

**Files:**
- Modify: `README.md`
- Modify: `docs/06-vcpu-and-modes.md`
- Modify: `CHANGELOG.md`

**Interfaces:**
- Consumes: Existing VMM architecture, guest flow, KVM setup script, and demo.
- Produces: Explicit requirements, limitations, documentation navigation, and zero em dashes.

- [ ] **Step 1: Add README completeness sections**

Add `## Limitations` covering x86-64 Linux, KVM availability, one vCPU, no
device model beyond the documented ports/MMIO, no live migration, and no
security isolation claim. Add `## Documentation` linking the overview,
architecture, KVM API, guest memory, vCPU modes, devices, ELF loader, and
testing guide.

- [ ] **Step 2: Rewrite the single em dash**

Edit `docs/06-vcpu-and-modes.md` according to sentence meaning.

- [ ] **Step 3: Verify the Markdown condition**

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
git commit -m "docs(readme): complete vmm guide"
```

### Task 2: Reverify every guest mode

**Files:**
- Verify: `Makefile`
- Verify: `scripts/self-test.sh`

**Interfaces:**
- Consumes: Real-mode, long-mode, serial, MMIO, fault, and ELF guest images.
- Produces: Warning-free builds and runtime evidence for each guest path.

- [ ] **Step 1: Run the clean build**

```bash
make clean && make all
```

Expected: exit 0 with no warnings.

- [ ] **Step 2: Check KVM before runtime testing**

```bash
make check-kvm
```

Expected: exit 0. If it fails, record the exact `/dev/kvm` limitation and do
not report the runtime suite as passed.

- [ ] **Step 3: Run the complete suite**

```bash
make test
make run
```

Expected: every guest marker in `scripts/self-test.sh` passes and the ELF guest
exits cleanly.

- [ ] **Step 4: Merge, push, and watch CI**

```bash
git switch main
git merge --no-ff docs/readme-completion
git push origin main
gh run watch --exit-status
```

Expected: synchronized clean `main` and successful CI.
