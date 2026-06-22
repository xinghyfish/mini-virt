# mini-virt Progress

Last updated: 2026-06-22

## Current Stage

v0.1 is complete. The VM can build, run, execute a minimal program, handle basic
arithmetic instructions, and reject invalid register indexes.

v0.2 is in progress. The next focus is control flow.

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
- Implemented `JMP`.
- Added normal and out-of-bounds `JMP` tests.
- Added a basic test runner.
- Verified `make test` passes.

## Needs Verification

- None for v0.1.

## Notes

- `MOVI` now writes `instr.immediate` into register `dst`.
- Register indexes are checked before reading or writing `vm->regs`.
- `OP_JMP` reads its target from `instr.immediate`.
- `OP_JMP` treats its target as a byte offset into VM program memory.
- `OP_JMP` rejects targets outside `program_size`.
- `OP_JMP` rejects targets that do not land on an instruction boundary.

## Next Goal

Continue v0.2 by implementing a conditional branch, such as `JZ`.

Example program:

```text
MOVI r0, 1
JZ   r0, 4
MOVI r1, 7
HALT
```

Because `r0` is not zero, the branch should not be taken:

```text
r1 = 7
running = 0
```

Then add the taken case:

```text
MOVI r0, 0
JZ   r0, 4
MOVI r1, 7
MOVI r2, 9
HALT
```

Expected result:

```text
r1 = 0
r2 = 9
running = 0
```
