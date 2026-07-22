#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "heap.h"
#include "io.h"
#include "isr.h"
#include "keyboard.h"
#include "kprintf.h"
#include "shell.h"
#include "string.h"
#include "timer.h"
#include "vga.h"

#define SHELL_LINE_CAPACITY 128
#define SHELL_MAX_ARGUMENTS 16
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_COMMAND_PORT 0x64
#define KEYBOARD_INPUT_BUFFER_FULL 0x02
#define KEYBOARD_RESET_COMMAND 0xFE
#define REBOOT_WAIT_LIMIT 100000

typedef void (*command_function_t)(int argument_count, char **arguments);

struct shell_command {
    const char *name;
    const char *help;
    command_function_t function;
};

static void command_help(int argument_count, char **arguments);
static void command_echo(int argument_count, char **arguments);
static void command_clear(int argument_count, char **arguments);
static void command_mem(int argument_count, char **arguments);
static void command_uptime(int argument_count, char **arguments);
static void command_reboot(int argument_count, char **arguments);
static void command_panic(int argument_count, char **arguments);

static const struct shell_command commands[] = {
    { "help", "list available commands", command_help },
    { "echo", "print arguments", command_echo },
    { "clear", "clear the VGA display", command_clear },
    { "mem", "show physical and heap memory usage", command_mem },
    { "uptime", "show seconds since timer start", command_uptime },
    { "reboot", "reset through the PS/2 controller", command_reboot },
    { "panic", "trigger a divide-error fault report", command_panic },
};

static const size_t command_count = sizeof(commands) / sizeof(commands[0]);

static void command_help(int argument_count, char **arguments) {
    (void)argument_count;
    (void)arguments;

    for (size_t i = 0; i < command_count; i++)
        kprintf("%s - %s\n", commands[i].name, commands[i].help);
}

static void command_echo(int argument_count, char **arguments) {
    for (int i = 1; i < argument_count; i++) {
        if (i > 1)
            kprintf(" ");
        kprintf("%s", arguments[i]);
    }
    kprintf("\n");
}

static void command_clear(int argument_count, char **arguments) {
    (void)argument_count;
    (void)arguments;
    vga_clear();
}

static void command_mem(int argument_count, char **arguments) {
    struct memory_stats stats = mem_stats();

    (void)argument_count;
    (void)arguments;
    kprintf("frames: total=%u used=%u free=%u\n",
            (unsigned int)stats.total_frames,
            (unsigned int)stats.used_frames,
            (unsigned int)stats.free_frames);
    kprintf("heap: %u/%u bytes\n", (unsigned int)stats.heap_used,
            (unsigned int)stats.heap_capacity);
}

static void command_uptime(int argument_count, char **arguments) {
    uint64_t seconds = timer_ticks() / TIMER_FREQUENCY_HZ;

    (void)argument_count;
    (void)arguments;
    kprintf("uptime: %u seconds\n", (unsigned int)seconds);
}

static void command_reboot(int argument_count, char **arguments) {
    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) null_idt = { 0, 0 };

    (void)argument_count;
    (void)arguments;
    kprintf("Rebooting...\n");

    for (uint32_t wait = 0; wait < REBOOT_WAIT_LIMIT; wait++) {
        if ((inb(KEYBOARD_STATUS_PORT) & KEYBOARD_INPUT_BUFFER_FULL) == 0)
            break;
        io_wait();
    }
    outb(KEYBOARD_COMMAND_PORT, KEYBOARD_RESET_COMMAND);

    for (volatile uint32_t delay = 0; delay < REBOOT_WAIT_LIMIT; delay++)
        __asm__ volatile ("pause");
    __asm__ volatile ("lidt %0; int3" : : "m"(null_idt) : "memory");
    for (;;)
        __asm__ volatile ("hlt");
}

static void command_panic(int argument_count, char **arguments) {
    (void)argument_count;
    (void)arguments;
    isr_trigger_divide_error();
}

static int tokenize(char *line, char **arguments) {
    int count = 0;
    char *cursor = line;

    while (*cursor != '\0' && count < SHELL_MAX_ARGUMENTS) {
        while (*cursor == ' ' || *cursor == '\t')
            cursor++;
        if (*cursor == '\0')
            break;

        arguments[count++] = cursor;
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t')
            cursor++;
        if (*cursor != '\0')
            *cursor++ = '\0';
    }

    return count;
}

static bool dispatch(char *line) {
    char *arguments[SHELL_MAX_ARGUMENTS];
    int argument_count = tokenize(line, arguments);

    if (argument_count == 0)
        return true;

    for (size_t i = 0; i < command_count; i++) {
        if (strcmp(arguments[0], commands[i].name) == 0) {
            commands[i].function(argument_count, arguments);
            return true;
        }
    }

    kprintf("Unknown command: %s. Type 'help'.\n", arguments[0]);
    return false;
}

bool shell_execute(const char *line) {
    char copy[SHELL_LINE_CAPACITY];
    size_t length = strlen(line);

    if (length >= sizeof(copy))
        return false;
    memcpy(copy, line, length + 1);
    return dispatch(copy);
}

static void read_line(char *line) {
    size_t length = 0;

    for (;;) {
        char character = keyboard_getchar();

        if (character == '\n') {
            line[length] = '\0';
            kprintf("\n");
            return;
        }
        if (character == '\b') {
            if (length > 0) {
                length--;
                kprintf("\b");
            }
            continue;
        }
        if (character >= ' ' && character <= '~'
                && length + 1 < SHELL_LINE_CAPACITY) {
            line[length++] = character;
            kprintf("%c", character);
        }
    }
}

void shell_run(void) {
    char line[SHELL_LINE_CAPACITY];

    for (;;) {
        kprintf("nutshell> ");
        read_line(line);
        dispatch(line);
    }
}
