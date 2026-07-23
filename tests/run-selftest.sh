#!/usr/bin/env bash
set -uo pipefail

iso=${1:-build/nutshell-test.iso}
log=build/selftest-serial.log
# Generous default so a slow cold / emulated runner (no KVM -> TCG) is not
# mistaken for a failure. Override with SELFTEST_TIMEOUT if needed.
timeout_seconds=${SELFTEST_TIMEOUT:-60}

rm -f "$log"
timeout "$timeout_seconds" qemu-system-x86_64 \
    -cdrom "$iso" \
    -display none \
    -serial "file:$log" \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
    -no-reboot >/dev/null 2>&1
status=$?
set -e

echo "----- selftest serial -----"
cat "$log"

# `timeout` returns 124 when it has to kill QEMU. Report that as an
# infrastructure timeout (exit 2), NOT a guest test failure (exit 1), so a
# slow runner is never confused with a broken kernel.
if [[ $status -eq 124 ]]; then
    echo "TIMEOUT: self-test exceeded ${timeout_seconds}s before QEMU exited" >&2
    echo "         (usually a slow cold/emulated runner, not a guest failure)" >&2
    exit 2
fi

# isa-debug-exit encodes a guest value as (value << 1) | 1. The guest writes
# zero for success, so QEMU returns one; translate that to a normal shell zero.
if [[ $status -ne 1 ]]; then
    echo "FAIL: unexpected QEMU self-test exit status: $status" >&2
    exit 1
fi

grep -Fq "[ok] test-pmm" "$log"
grep -Fq "[ok] test-heap" "$log"
grep -Fq "scripted-shell-ok" "$log"
grep -Fq "[ok] test-shell" "$log"
grep -Fq "[ok] kernel-selftest" "$log"
