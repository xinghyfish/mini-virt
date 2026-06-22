# mini-virt

`mini-virt` is a small C project for learning systems programming, CPU
execution, and eventually computer architecture virtualization.

The current codebase is intentionally small. It gives you a VM skeleton with:

- a VM state struct
- a tiny instruction format
- program loading
- implemented `HALT`
- started `MOVI`, `ADD`, and `SUB`
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
- v0.2 is ready to start.
- The VM builds and runs.
- `HALT` is implemented and tested.
- `MOVI`, `ADD`, and `SUB` are implemented and tested.
- Register index checks are implemented.
- Invalid register tests are implemented.

## Current Implementation Goal

Start v0.2 by implementing `JMP`.

```text
MOVI r0, 1
JMP  4
MOVI r0, 99
MOVI r1, 7
HALT
```

Expected behavior:

- `JMP` reads the target from `instr.immediate`.
- the target is a byte offset into program memory.
- invalid jump targets return `VM_ERR_PC_OUT_OF_BOUNDS`.
- the example program skips `MOVI r0, 99`.

## Suggested Study Order

1. Read `src/instr.h` to understand the instruction format.
2. Read `src/vm.h` to understand VM state.
3. Read `src/vm.c` and trace how `vm_run` calls `vm_step`.
4. Run `make test`.
5. Implement one instruction at a time.
6. Add one test per instruction.
