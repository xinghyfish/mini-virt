# mini-virt

`mini-virt` is a small C project for learning systems programming, CPU
execution, and eventually computer architecture virtualization.

The current codebase is intentionally small. It gives you a VM skeleton with:

- a VM state struct
- a tiny instruction format
- program loading
- implemented `HALT`
- implemented `MOVI`, `ADD`, `SUB`, and `JMP`
- a basic test runner

## Build

```sh
make
```

## Run

```sh
make run
```

## Test

```sh
make test
```

The tests are compiled with AddressSanitizer and UndefinedBehaviorSanitizer.
This helps catch memory bugs early while you are learning C.

## Current Progress

See `docs/progress.md` for the latest learning checkpoint.

Current status:

- v0.1 is complete.
- v0.2 is in progress.
- The VM builds and runs.
- `HALT` is implemented and tested.
- `MOVI`, `ADD`, and `SUB` are implemented and tested.
- `JMP` is implemented and tested.
- Register index checks are implemented.
- Invalid register tests are implemented.

## Current Implementation Goal

Continue v0.2 by implementing a conditional branch such as `JZ`.

```text
MOVI r0, 1
JZ   r0, target
MOVI r1, 7
HALT
```

Expected behavior:

- `JZ` reads the condition register from `src` or `dst`.
- `JZ` reads the target from `instr.immediate`.
- the target is a byte offset into program memory.
- if the register is zero, the VM jumps.
- if the register is not zero, execution continues normally.

## Suggested Study Order

1. Read `src/instr.h` to understand the instruction format.
2. Read `src/vm.h` to understand VM state.
3. Read `src/vm.c` and trace how `vm_run` calls `vm_step`.
4. Run `make test`.
5. Implement one instruction at a time.
6. Add one test per instruction.
