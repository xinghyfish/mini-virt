# mini-virt Progress

Last updated: 2026-06-22

## Current Stage

v0.1 is complete. The VM can build, run, execute a minimal program, handle basic
arithmetic instructions, and reject invalid register indexes.

v0.2 is ready to start. The next focus is control flow.

## Completed

- Created the project skeleton.
- Added a `Makefile`.
- Defined the VM state in `src/vm.h`.
- Defined the instruction format in `src/instr.h`.
- Implemented VM initialization.
- Implemented program loading.
- Implemented the fetch/decode/execute loop.
- Implemented `HALT`.
- Implemented arithmetic instructions:
  - `MOVI`
  - `ADD`
  - `SUB`
- Added arithmetic instruction tests.
- Added register index checks before register access.
- Added invalid register tests.
- Added a basic test runner.
- Verified `make test` passes.

## Needs Verification

- None for v0.1.

## Notes

- `MOVI` now writes `instr.immediate` into register `dst`.
- Register indexes are checked before reading or writing `vm->regs`.
- `OP_JMP` is still intentionally unimplemented.

## Next Goal

Start v0.2 by implementing `JMP`.

Example program:

```text
MOVI r0, 1
JMP  4
MOVI r0, 99
MOVI r1, 7
HALT
```

Expected result:

```text
r0 = 1
r1 = 7
running = 0
```

Suggested encoding:

- Store the jump target in `instr.immediate`.
- Treat the target as a byte offset into VM program memory.
- Reject targets outside `program_size` with `VM_ERR_PC_OUT_OF_BOUNDS`.
