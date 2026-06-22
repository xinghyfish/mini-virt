# mini-virt

`mini-virt` is a small C project for learning systems programming, CPU
execution, and eventually computer architecture virtualization.

The current codebase is intentionally small. It gives you a VM skeleton with:

- a VM state struct
- a tiny instruction format
- program loading
- implemented `HALT`
- implemented `MOVI`, `ADD`, `SUB`, `JMP`, and `JZ`
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
See `docs/instructions.md` for the instruction reference.
See `docs/debugging.md` for the v0.3 debugging helper checklist.

Current status:

- v0.1 is complete.
- v0.2 is complete.
- v0.3 is in progress.
- The VM builds and runs.
- `HALT` is implemented and tested.
- `MOVI`, `ADD`, and `SUB` are implemented and tested.
- `JMP` is implemented and tested.
- `JZ` is implemented and tested.
- Register index checks are implemented.
- Invalid register tests are implemented.

## Current Implementation Goal

Start v0.3 by testing single-step execution with `vm_step`.

```text
MOVI r0, 7
MOVI r1, 5
ADD  r0, r1
HALT
```

Expected behavior:

- call `vm_step` once and verify `r0 == 7`
- call `vm_step` again and verify `r1 == 5`
- call `vm_step` again and verify `r0 == 12`
- call `vm_step` again and verify the VM stops

## Suggested Study Order

1. Read `src/instr.h` to understand the instruction format.
2. Read `src/vm.h` to understand VM state.
3. Read `src/vm.c` and trace how `vm_run` calls `vm_step`.
4. Run `make test`.
5. Implement one instruction at a time.
6. Add one test per instruction.
