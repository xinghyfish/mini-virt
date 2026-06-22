# mini-virt Progress

Last updated: 2026-06-22

## Current Stage

v0.1 is complete. The VM can build, run, execute a minimal program, handle basic
arithmetic instructions, and reject invalid register indexes.

v0.2 is complete. The VM supports unconditional and conditional control flow.

v0.3 is in progress. The next focus is debugging tools.

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
- Implemented `JZ`.
- Added taken, not-taken, invalid-register, and invalid-target `JZ` tests.
- Added v0.3 function frames for memory and instruction dumps.
- Added v0.3 test placeholders for single-step and dump helpers.
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
- `OP_JZ` reads its condition register from `instr.src`.
- `OP_JZ` jumps only when that register is zero.
- `OP_JZ` uses the same byte-offset target rules as `OP_JMP`.

## Next Goal

Start v0.3 by improving debug visibility:

- Add a test that calls `vm_step` directly.
- Verify `pc` changes after each step.
- Verify registers change after each step.
- Consider adding a small instruction dump helper after that.
