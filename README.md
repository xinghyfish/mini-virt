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

You can also run each test group separately:

```sh
make test-vm
make test-assembler
```

## Current Progress

See `docs/progress.md` for the latest learning checkpoint.
See `docs/instructions.md` for the instruction reference.
See `docs/debugging.md` for the v0.3 debugging helper checklist.
See `docs/assembler.md` for the v0.4 assembler plan.
See `docs/labels.md` for the next label-support learning node.

Current status:

- v0.1 is complete.
- v0.2 is complete.
- v0.3 is complete.
- v0.4 is in progress: basic assembler is implemented, label support is next.
- The VM builds and runs.
- `HALT` is implemented and tested.
- `MOVI`, `ADD`, and `SUB` are implemented and tested.
- `JMP` is implemented and tested.
- `JZ` is implemented and tested.
- direct `vm_step` tests are implemented.
- memory and instruction dump helpers are implemented.
- Register index checks are implemented.
- Invalid register tests are implemented.

## Current Implementation Goal

Continue v0.4 by adding label support.

```text
loop:
SUB r0, r1
JZ r0, done
JMP loop
done:
HALT
```

Expected behavior:

- collect label definitions and their instruction indexes
- resolve labels used by `JMP` and `JZ`
- reject unknown labels
- convert label targets into byte offsets

## Suggested Study Order

1. Read `src/instr.h` to understand the instruction format.
2. Read `src/vm.h` to understand VM state.
3. Read `src/vm.c` and trace how `vm_run` calls `vm_step`.
4. Run `make test`.
5. Implement one instruction at a time.
6. Add one test per instruction.
