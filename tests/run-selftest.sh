#!/usr/bin/env bash
set -uo pipefail

iso=${1:-build/nutshell-test.iso}
log=build/selftest-serial.log

rm -f "$log"
timeout 25 qemu-system-x86_64 \
    -cdrom "$iso" \
    -display none \
    -serial "file:$log" \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
    -no-reboot >/dev/null 2>&1
status=$?
set -e

echo "----- selftest serial -----"
cat "$log"

# isa-debug-exit encodes a guest value as (value << 1) | 1. The guest writes
# zero for success, so QEMU returns one; translate that to a normal shell zero.
if [[ $status -ne 1 ]]; then
    echo "unexpected QEMU self-test status: $status" >&2
    exit 1
fi

grep -Fq "[ok] test-pmm" "$log"
grep -Fq "[ok] test-heap" "$log"
grep -Fq "scripted-shell-ok" "$log"
grep -Fq "[ok] test-shell" "$log"
grep -Fq "[ok] kernel-selftest" "$log"
