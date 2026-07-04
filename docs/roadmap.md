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
- [x] Add `JZ` or another conditional branch.
- [x] Add tests for taken and skipped conditional branches.

## v0.3: Debugging Tools

- [x] Add direct `vm_step` tests.
- [x] Add single-step mode.
- [x] Add register dump output.
- [x] Implement memory dump output.
- [x] Implement instruction dump output.
- [x] Add function frames for memory and instruction dumps.

## v0.4: Assembler

- [x] Add assembler module files.
- [x] Parse a small text assembly format.
- [x] Translate labels into instruction addresses.
- [x] Generate bytecode for the VM.
- [x] Add assembler tests.
- [x] Add assembler-to-VM integration tests.

## v0.5: Executable Format and Loader

- [ ] Define a small executable file format.
- [x] Add loader module files.
- [ ] Save assembled bytecode to a file.
- [ ] Load bytecode from a file.
- [ ] Set VM entry point from the executable header.
- [x] Add loader test placeholders.
- [ ] Add loader tests for valid and invalid files.
