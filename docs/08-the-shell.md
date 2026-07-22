# 08 — The shell

*Implemented in milestone 6.* This is the payoff: an interactive prompt running
on the bare kernel. Build steps in
[milestones/milestone-6-shell.md](milestones/milestone-6-shell.md).

## What it is

A read–eval–print loop (REPL). It reads a line from the keyboard buffer, parses
it into a command and arguments, dispatches to a built-in, prints the result,
and loops. No processes, no external programs — every command is a C function
compiled into the kernel.

```
loop:
    print "nutshell> "
    line = read_line()          # from the keyboard ring buffer (M4)
    argv = tokenize(line)       # split on spaces
    cmd  = lookup(argv[0])      # find matching built-in
    if cmd: cmd(argc, argv)
    else:   print "unknown command"
```

## Built-in commands (v1.0.0)

| Command | Does |
|---------|------|
| `help` | List available commands. |
| `echo <args>` | Print the arguments back. |
| `clear` | Clear the screen (VGA). |
| `mem` | Show memory stats (total/used/free frames — from M5). |
| `uptime` | Show timer ticks / seconds since boot (from M3). |
| `reboot` | Reboot the machine (8042 pulse or triple-fault). |
| `panic` | Deliberately trigger a fault — proves the exception handlers. |

Adding a command = one function + one row in the dispatch table. Keep them
tiny; the shell's value is the *machinery*, not a big command set.

## Command table pattern

```c
typedef int (*cmd_fn)(int argc, char **argv);

struct command {
    const char *name;
    const char *help;
    cmd_fn      fn;
};

static const struct command commands[] = {
    { "help",   "list commands",        cmd_help },
    { "echo",   "print arguments",      cmd_echo },
    { "clear",  "clear the screen",     cmd_clear },
    { "mem",    "memory statistics",    cmd_mem },
    { "uptime", "time since boot",      cmd_uptime },
    { "reboot", "restart the machine",  cmd_reboot },
};
```

Dispatch is a linear search over this table — clear and more than fast enough.

## Parsing

A minimal tokenizer: split the input line on runs of spaces into an `argv`
array (fixed max args), null-terminate each token in place. No quoting, no pipes
in v1 — those are stretch goals.

## What to demonstrate (the GIF)

Power on → banner → `help` → `echo hello world` → `mem` → `uptime` → `clear`.
That 10-second clip is the headline of the whole project.

## Stretch goals

- Command history (up/down arrows).
- Tab completion of built-in names.
- Quoting and simple argument parsing.
- A tiny in-RAM filesystem with `ls`/`cat`.
