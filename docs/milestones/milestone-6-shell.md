# Milestone 6: The shell ✅ (done)

**Goal:** the payoff: an interactive prompt running on the bare kernel, reading
lines from the keyboard, dispatching to built-in commands.

## Concepts

The read–eval–print loop, line reading over the keyboard ring buffer,
tokenising input into `argv`, a command dispatch table, tying every prior
subsystem together.

## Tasks

- [x] `src/shell.c` + `include/shell.h`: `shell_run()`: print `nutshell> `,
      read a line via `keyboard_getchar()` with echo + backspace, tokenise on
      spaces into `argv`, look up and call the command.
- [x] Implement built-ins:
  - [x] `help`: list commands + one-line help each.
  - [x] `echo`: print the arguments.
  - [x] `clear`: `vga_clear()`.
  - [x] `mem`: print `mem_stats()` (from M5).
  - [x] `uptime`: `timer_ticks()` → seconds (from M3).
  - [x] `reboot`: pulse the 8042 reset line (fallback: triple fault).
  - [x] `panic`: trigger a fault to demo the exception handlers.
- [x] Handle unknown commands and empty lines gracefully.
- [x] Call `shell_run()` at the end of `kernel_main` (after all `*_init`).

## Files

`src/shell.c`, `include/shell.h`, edit `src/kernel.c`.

## Definition of Done

- [x] Boots straight into `nutshell> `.
- [x] Each built-in works: `help`, `echo hello world`, `clear`, `mem`,
      `uptime`, `reboot`.
- [x] Unknown command prints a friendly error; the prompt never wedges.
- [x] `make iso` clean, `make run` boots into the shell, CI green.
- [x] **This is the project's Definition of Done.** Update the README status
      table and record the GIF in milestone 7.

## References

- [docs/08: The shell](../08-the-shell.md) (design + command-table pattern)

**Next:** [Milestone 7: Polish](milestone-7-polish.md).
