# 09 — Testing & debugging

Kernel bugs don't give you a stack trace by default — the machine just resets.
These are the techniques that make Nutshell debuggable.

## Serial logging is your best friend

The COM1 driver (milestone 0) prints to a port QEMU forwards to your terminal
(`make run` uses `-serial stdio`). Unlike the VGA screen, serial output is easy
to capture, scroll, and grep. **Log liberally to serial**, especially in
interrupt handlers and during bring-up. CI relies on it too
(`.github/workflows/ci.yml` greps the serial log for the boot banner).

## GDB source-level debugging

QEMU has a built-in GDB stub. Start the kernel halted:

```bash
make debug        # qemu ... -s -S  (stub on :1234, CPU frozen)
```

In another terminal:

```bash
gdb build/kernel.bin
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
(gdb) layout src
```

You can set breakpoints, step C code, and inspect memory and registers.

## The triple-fault trap

If the CPU faults, and while handling that fault it faults again, and again,
it **triple-faults** and resets — which looks like a silent reboot loop. Before
the IDT exists (pre-M3) any CPU exception does this. After M3, install exception
handlers early so a fault *prints* instead of resetting. When you see QEMU
rebooting in a loop, suspect an unhandled exception.

Tip: run QEMU with `-no-reboot -no-shutdown` and `-d int,cpu_reset` to make it
halt on a triple fault and dump the CPU state instead of rebooting:

```bash
qemu-system-x86_64 -cdrom build/nutshell.iso -no-reboot -no-shutdown \
    -d int,cpu_reset -serial stdio
```

## Automated smoke test (CI)

CI boots the ISO headless and checks the serial log for a known marker:

```bash
timeout 20 qemu-system-x86_64 -cdrom build/nutshell.iso \
    -display none -serial file:serial.log -no-reboot || true
grep -q "Nutshell" serial.log
```

As milestones land, extend the marker set: each subsystem's `*_init` can print
a line like `[ok] idt`, and CI can assert they all appear. A QEMU **isa-debug-exit**
device (port `0xF4`) lets a kernel test build exit QEMU with a chosen code — the
basis for pass/fail kernel tests if you want them in milestone 7.

Milestone 7 implements that path. `make test` builds a separate
`build/nutshell-test.iso`, repeats the PMM and heap tests, dispatches a scripted
shell command, and writes guest status zero to `0xF4`. QEMU encodes that as
host status one; `tests/run-selftest.sh` validates and translates it to a
normal successful exit code for CI.

## Common failure modes

| Symptom | Likely cause |
|---------|--------------|
| QEMU reboots in a loop | unhandled CPU exception (triple fault) |
| Boots but blank screen, serial OK | VGA cursor/scroll bug; check `vga.c` |
| Keys do nothing (post-M4) | forgot the PIC EOI, or IRQ1 not unmasked |
| Timer never ticks | PIT not initialised, or IRQ0 masked |
| `grub-mkrescue` fails | missing `xorriso` / `mtools` / `grub-pc-bin` |
