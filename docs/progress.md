# mini-virt Progress

Last updated: 2026-06-22

## Current Stage

v0.1 is complete. The VM can build, run, execute a minimal program, handle basic
arithmetic instructions, and reject invalid register indexes.

v0.2 is complete. The VM supports unconditional and conditional control flow.

v0.3 is complete. The VM now has basic debugging visibility through direct
single-step tests, register dumps, memory dumps, and instruction dumps.

v0.4 is in progress. The basic assembler can parse the current instruction set
and generate VM bytecode. The next focus is label support.

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
- Added direct `vm_step` tests.
- Added `pc` update tests for normal and jump instructions.
- Implemented memory dump output.
- Implemented instruction dump output.
- Added tests that call memory and instruction dump helpers.
- Added assembler module frames.
- Added assembler test placeholders.
- Added `make test-assembler`.
- Implemented `asm_parse_line`.
- Implemented `asm_parse_program`.
- Added assembler tests for `HALT`, `MOVI`, `ADD`, `JMP`, `JZ`, unknown
  opcodes, invalid arguments, comments, and output buffer limits.
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
- `vm_step` can now be tested directly to inspect VM state after each
  instruction.
- `vm_dump_memory` prints VM memory bytes for a requested range.
- `vm_dump_instruction` prints one decoded instruction.

## Next Goal

Continue v0.4 by adding label support:

- Parse label definitions such as `loop:`.
- Store label names and instruction indexes in a small table.
- Resolve `JMP label` and `JZ r0, label`.
- Convert label targets into byte offsets.
- Add tests for forward labels, backward labels, and unknown labels.
