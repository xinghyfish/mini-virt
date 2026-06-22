# Debugging Tools

v0.3 focuses on making VM execution visible one step at a time.

This file is a checklist for the debugging helpers. The function frames exist,
but the implementation is intentionally left for practice.

## `vm_step`

Goal:

- Execute exactly one instruction.
- Let tests inspect `pc`, registers, and `running` after each step.

Suggested first test:

```text
MOVI r0, 7
MOVI r1, 5
ADD  r0, r1
HALT
```

Assertions to add:

- after step 1: `r0 == 7`
- after step 2: `r1 == 5`
- after step 3: `r0 == 12`
- after step 4: `running == 0`

## `vm_dump_memory`

Current frame:

```c
VMStatus vm_dump_memory(const VM *vm, uint32_t start, size_t length);
```

Goal:

- Print memory bytes in a readable format.
- Reject ranges outside `VM_MEMORY_SIZE`.

Suggested output shape:

```text
0000: 01 00 00 00 07 00 00 00
0008: 00 00 00 00 00 00 00 00
```

## `vm_dump_instruction`

Current frame:

```c
void vm_dump_instruction(const Instruction *instruction);
```

Goal:

- Print one decoded instruction.
- Show only fields that matter for that opcode.

Suggested output shape:

```text
MOVI r0, 7
ADD r0, r1
JMP byte=24 index=3
JZ r0, byte=24 index=3
HALT
```
