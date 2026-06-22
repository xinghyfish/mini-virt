# mini-virt Roadmap

This project starts as a tiny teaching VM written in C. The first goal is to
understand how a CPU fetches, decodes, and executes instructions.

## v0.1: Minimal CPU Loop

- [x] Define a VM state struct.
- [x] Load an array of instructions into VM memory.
- [x] Execute `HALT`.
- [x] Add verified `MOVI`, `ADD`, and `SUB`.
- [x] Print registers after execution.
- [x] Reject invalid register indexes.
- [x] Add tests for arithmetic instructions.
- [x] Add tests for invalid register indexes.

## v0.2: Control Flow

- [x] Add `JMP`.
- [x] Add simple bounds checks for program counter changes.
- [ ] Add `JZ` or another conditional branch.
- [ ] Add tests for taken and skipped conditional branches.

## v0.3: Debugging Tools

- Add single-step mode.
- Add register dump output.
- Add memory dump output.

## v0.4: Assembler

- Parse a small text assembly format.
- Translate labels into instruction addresses.
- Generate bytecode for the VM.
