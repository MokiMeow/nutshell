#!/usr/bin/env bash
# Drive the real PS/2 input path through QEMU's local HMP monitor.
set -euo pipefail

iso=${1:-build/nutshell.iso}
qemu=${QEMU:-qemu-system-x86_64}
timeout_seconds=${INTERACTIVE_TIMEOUT:-60}
key_delay=${INTERACTIVE_KEY_DELAY:-0.03}
log=build/interactive-serial.log
monitor_log=build/interactive-monitor.log
temporary=$(mktemp -d)
qemu_pid=

cleanup() {
    local status=$?
    trap - EXIT
    if [[ -n $qemu_pid ]]; then
        kill "$qemu_pid" 2>/dev/null || true
        wait "$qemu_pid" 2>/dev/null || true
    fi
    exec 3>&- || true
    rm -rf "$temporary"
    if (( status != 0 )); then
        echo '----- interactive serial -----' >&2
        cat "$log" >&2 2>/dev/null || true
        echo '----- interactive monitor (tail) -----' >&2
        tail -n 20 "$monitor_log" >&2 2>/dev/null || true
    fi
    exit "$status"
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

fail() {
    echo "FAIL: $*" >&2
    exit 1
}

[[ -f $iso ]] || fail "ISO not found: $iso"
command -v "$qemu" >/dev/null || fail "QEMU not found: $qemu"
mkdir -p build
: > "$log"
: > "$monitor_log"
mkfifo "$temporary/monitor"
# Opening both ends prevents FIFO open from blocking during startup/cleanup.
exec 3<>"$temporary/monitor"
timeout --kill-after=5 "$timeout_seconds" "$qemu" \
    -m "${QEMU_MEMORY:-128M}" \
    -cdrom "$iso" -display none -serial "file:$log" -monitor stdio \
    -no-reboot -no-shutdown \
    < "$temporary/monitor" > "$monitor_log" 2>&1 &
qemu_pid=$!

assert_running() {
    kill -0 "$qemu_pid" 2>/dev/null ||
        fail "QEMU exited or exceeded ${timeout_seconds}s; inspect $monitor_log"
}

wait_line() {
    local expected=$1
    until tr -d '\r' < "$log" | grep -Fx -- "$expected" > /dev/null; do
        assert_running
        sleep 0.05
    done
}

wait_prompt() {
    local expected=$1 count
    while :; do
        count=$(grep -oF 'nutshell> ' "$log" | wc -l || true)
        (( count >= expected )) && return
        assert_running
        sleep 0.05
    done
}

send_key() {
    assert_running
    printf 'sendkey %s 10\n' "$1" >&3
    sleep "$key_delay"
}

send_text() {
    local text=$1 character key i
    for (( i = 0; i < ${#text}; i++ )); do
        character=${text:i:1}
        case $character in
            ' ') key=spc ;;
            '!') key=shift-1 ;;
            '-') key=minus ;;
            '_') key=shift-minus ;;
            [A-Z]) key="shift-${character,,}" ;;
            [a-z0-9]) key=$character ;;
            *) fail "unsupported test character: $character" ;;
        esac
        send_key "$key"
    done
}

prompt_count=1
command_line() {
    send_text "$1"
    send_key ret
    prompt_count=$((prompt_count + 1))
    wait_prompt "$prompt_count"
}

wait_prompt "$prompt_count"
command_line 'echo interactive-irq-ok'
wait_line 'interactive-irq-ok'

command_line 'echo Shift!_OK'
wait_line 'Shift!_OK'

send_text 'echo correction-bad'
send_key backspace
send_key backspace
send_key backspace
send_text 'ok'
send_key ret
prompt_count=$((prompt_count + 1))
wait_prompt "$prompt_count"
wait_line 'correction-ok'
grep -F -- $'bad\b \b\b \b\b \bok' "$log" > /dev/null ||
    fail 'serial backspace did not erase the deleted characters'

command_line 'missing-command'
wait_line "Unknown command: missing-command. Type 'help'."
command_line 'echo recovered-ok'
wait_line 'recovered-ok'

command_line help
wait_line 'help - list available commands'
wait_line 'panic - trigger a divide-error fault report'
command_line mem
tr -d '\r' < "$log" | grep -E '^frames: total=[0-9]+ used=[0-9]+ free=[0-9]+$' > /dev/null ||
    fail 'mem did not report frame counts'
tr -d '\r' < "$log" | grep -E '^heap: [0-9]+/[0-9]+ bytes$' > /dev/null ||
    fail 'mem did not report heap usage'
command_line uptime
tr -d '\r' < "$log" | grep -E '^uptime: [0-9]+ seconds$' > /dev/null ||
    fail 'uptime did not report seconds'

# The 127-character line limit leaves 122 characters after "echo ".
printf -v long_input '%150s' ''
long_input=${long_input// /x}
command_line "echo $long_input"
wait_line "${long_input:0:122}"
command_line 'echo after-overflow-ok'
wait_line 'after-overflow-ok'

send_text panic
send_key ret
wait_line '[FAULT] exception 0 error=0x0'
# Wait for the full diagnostic rather than stopping midway through its UART IO.
until tr -d '\r' < "$log" | grep -E '^rax=0x[0-9a-f]+ rbx=0x[0-9a-f]+ rcx=0x[0-9a-f]+ rdx=0x[0-9a-f]+$' > /dev/null; do
    assert_running
    sleep 0.05
done
echo 'PASS: PS/2 input, shift, editing, recovery, commands, overflow, and panic'
echo "Serial log: $log"
