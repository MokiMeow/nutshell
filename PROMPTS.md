# Prompts for the building agent (Codex)

Copy the **Kickoff prompt** below into Codex to build Nutshell to completion.
Best launched from **inside the Ubuntu (WSL) shell** at the repo root, so all
commands are native Linux with no Windows-quoting issues:

```bash
cd /mnt/c/Users/MOHITH~1/Desktop/nutshell   # 8.3 short path avoids the space in "Mohith S"
codex                                        # then paste the Kickoff prompt
```

---

## Kickoff prompt (build the whole kernel)

```
You are the Builder for the Nutshell kernel — a from-scratch x86-64 OS in C and
assembly that boots into its own shell. Your job is to take it from "boots to a
banner" (milestone 0, done) to "boots into a working shell" (milestone 6) and
then polish it (milestone 7).

STEP 0 — READ FIRST, in this order, and treat them as binding:
- AGENTS.md            (the operating manual; it overrides everything else)
- docs/04-roadmap.md   (the milestone order and dependencies)
- docs/milestones/     (one spec per milestone, each with a Definition of Done)

ENVIRONMENT (already set up — do not reinstall anything):
- Ubuntu on WSL2, default user root, toolchain installed: nasm, gcc, binutils,
  make, xorriso, grub-pc-bin, grub-common, mtools, qemu-system-x86_64.
- Repo lives at /mnt/c/Users/MOHITH~1/Desktop/nutshell (use this short path).
- There is NO display. Verify every build by booting headless and reading the
  serial log — never assume it works.

BUILD + VERIFY (run these from the repo root after every change):
    make clean && make iso            # must exit 0 with ZERO warnings
    rm -f serial.log
    timeout 25 qemu-system-x86_64 -cdrom build/nutshell.iso \
        -display none -serial file:serial.log -no-reboot >/dev/null 2>&1
    cat serial.log                    # must contain the expected markers
Have each subsystem's init print a serial marker (e.g. [ok] idt) as its
milestone spec says, and grep serial.log for them to prove the milestone works.

THE LOOP — repeat until milestone 6 is done, then do milestone 7:
1. Pick the lowest-numbered unfinished milestone in docs/milestones/.
2. Implement exactly its Tasks. New .c -> src/, new .h -> include/
   (the Makefile auto-discovers them; do not edit the Makefile for new files).
3. Build clean and boot-verify per above. If it does not boot, DEBUG IT — read
   docs/09-testing-and-debugging.md; a reboot loop means an unhandled CPU
   exception. Do not move on until the Definition of Done is met and proven.
4. Update the milestone's doc (tick the Definition of Done), the README status
   table, and CHANGELOG.md.
5. Commit: `type(scope): outcome`, imperative, lower case, NO AI/co-author
   trailers. Example: feat(idt): install 64-bit interrupt descriptor table.
6. Go to the next milestone.

HARD RULES (from AGENTS.md — violating any is a failure):
- Freestanding only: no libc, no dependencies. Write memset/memcpy/strlen
  yourself (src/string.c) if needed.
- The build must NEVER break and must always boot. Land big milestones as
  several small commits that each stay green.
- Warnings are errors: keep it clean under -Wall -Wextra and the linker.
- Do not enable SSE/MMX/FPU (the Makefile disables them on purpose).
- Do not add a bootloader/kernel's code from elsewhere; write it here.

DEFINITION OF DONE (whole project): Nutshell boots to an interactive
`nutshell> ` prompt where `help`, `echo`, `clear`, `mem`, `uptime`, and
`reboot` all work, running on the bare kernel. CHANGELOG updated, README status
all green, tag v1.0.0.

REPORTING: after each milestone, show me the diff summary, the exact
build+boot commands you ran, and the serial.log output proving it works. If you
get stuck on a milestone after a genuine debugging effort, STOP and report the
failure with the serial log — do not fake success or skip ahead.

Begin with milestone 1 now.
```

---

## Continuation prompt (if a session ends mid-way)

```
Continue building Nutshell. Read AGENTS.md and docs/milestones/. Pick the
lowest-numbered unfinished milestone and follow the same loop: implement, build
clean, boot-verify headless via serial.log, update docs+CHANGELOG, commit
(type(scope): outcome, no AI trailers), then the next. Keep the build green and
booting at every commit. Stop and report if you get genuinely stuck.
```

---

## If Codex runs from Windows PowerShell instead of inside WSL

Wrap each shell command so it runs in Ubuntu, and use the short path:

```
wsl -d Ubuntu -- bash -lc 'cd /mnt/c/Users/MOHITH~1/Desktop/nutshell && make iso'
```

Avoid embedded double-quotes in the bash string (PowerShell 5.1 drops them);
route output to a file and read it if needed. Running Codex *inside* the Ubuntu
shell avoids all of this.
