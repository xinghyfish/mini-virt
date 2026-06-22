# Instruction Reference

This document describes the current mini-virt instruction format and each
supported opcode.

## Instruction Format

Every instruction uses the same C struct:

```c
typedef struct {
    uint8_t opcode;
    uint8_t dst;
    uint8_t src;
    uint8_t reserved;
    uint32_t immediate;
} Instruction;
```

Field meaning:

- `opcode`: selects the operation.
- `dst`: destination register index, when the instruction writes a register.
- `src`: source register index, when the instruction reads a register.
- `reserved`: currently unused.
- `immediate`: literal value or jump target, depending on the instruction.

Register indexes are zero-based:

```text
r0 -> 0
r1 -> 1
...
r7 -> 7
```

The VM currently has `VM_REG_COUNT` registers. Invalid register indexes should
produce `VM_ERR_INVALID_REGISTER`.

## Jump Target Convention

Jump targets are byte offsets into loaded VM program memory.

Use `sizeof(Instruction)` when writing tests or hand-written programs:

```c
3 * sizeof(Instruction)
```

That target means "jump to instruction index 3", which is the fourth instruction
in the program.

Valid jump targets must:

- be less than `vm->program_size`
- land exactly on an instruction boundary

Invalid jump targets should produce `VM_ERR_PC_OUT_OF_BOUNDS`.

## `HALT`

Opcode:

```c
OP_HALT
```

Purpose:

Stops the VM.

Fields:

- `opcode`: `OP_HALT`
- `dst`: unused
- `src`: unused
- `immediate`: unused

Example:

```c
{ .opcode = OP_HALT }
```

Expected behavior:

```text
running = 0
```

## `MOVI`

Opcode:

```c
OP_MOVI
```

Purpose:

Moves an immediate value into a register.

Fields:

- `opcode`: `OP_MOVI`
- `dst`: register to write
- `src`: unused
- `immediate`: value to store

Example:

```c
{ .opcode = OP_MOVI, .dst = 0, .immediate = 7 }
```

Equivalent assembly-style notation:

```text
MOVI r0, 7
```

Expected behavior:

```text
r0 = 7
```

Errors:

- invalid `dst` returns `VM_ERR_INVALID_REGISTER`

## `ADD`

Opcode:

```c
OP_ADD
```

Purpose:

Adds one register into another register.

Fields:

- `opcode`: `OP_ADD`
- `dst`: destination register, also the left operand
- `src`: source register, also the right operand
- `immediate`: unused

Example:

```c
{ .opcode = OP_ADD, .dst = 0, .src = 1 }
```

Equivalent assembly-style notation:

```text
ADD r0, r1
```

Expected behavior:

```text
r0 = r0 + r1
```

Errors:

- invalid `dst` returns `VM_ERR_INVALID_REGISTER`
- invalid `src` returns `VM_ERR_INVALID_REGISTER`

## `SUB`

Opcode:

```c
OP_SUB
```

Purpose:

Subtracts one register from another register.

Fields:

- `opcode`: `OP_SUB`
- `dst`: destination register, also the left operand
- `src`: source register, also the right operand
- `immediate`: unused

Example:

```c
{ .opcode = OP_SUB, .dst = 0, .src = 1 }
```

Equivalent assembly-style notation:

```text
SUB r0, r1
```

Expected behavior:

```text
r0 = r0 - r1
```

Errors:

- invalid `dst` returns `VM_ERR_INVALID_REGISTER`
- invalid `src` returns `VM_ERR_INVALID_REGISTER`

## `JMP`

Opcode:

```c
OP_JMP
```

Purpose:

Unconditionally jumps to another instruction.

Fields:

- `opcode`: `OP_JMP`
- `dst`: unused
- `src`: unused
- `immediate`: target byte offset

Example:

```c
{ .opcode = OP_JMP, .immediate = 3 * sizeof(Instruction) }
```

Equivalent assembly-style notation:

```text
JMP 3
```

In the current C representation, the example above jumps to instruction index
`3`, but the stored value is the byte offset `3 * sizeof(Instruction)`.

Expected behavior:

```text
pc = immediate
```

Errors:

- invalid target returns `VM_ERR_PC_OUT_OF_BOUNDS`

## `JZ`

Opcode:

```c
OP_JZ
```

Purpose:

Jumps to another instruction only when a register is zero.

Fields:

- `opcode`: `OP_JZ`
- `dst`: unused
- `src`: condition register
- `immediate`: target byte offset

Example:

```c
{ .opcode = OP_JZ, .src = 0, .immediate = 3 * sizeof(Instruction) }
```

Equivalent assembly-style notation:

```text
JZ r0, 3
```

Expected behavior:

```text
if r0 == 0:
    pc = immediate
else:
    continue
```

Errors:

- invalid `src` returns `VM_ERR_INVALID_REGISTER`
- invalid target returns `VM_ERR_PC_OUT_OF_BOUNDS`, but only if the branch is
  taken

## Example Program

```c
const Instruction program[] = {
    { .opcode = OP_MOVI, .dst = 0, .immediate = 0 },
    { .opcode = OP_JZ, .src = 0, .immediate = 3 * sizeof(Instruction) },
    { .opcode = OP_MOVI, .dst = 1, .immediate = 7 },
    { .opcode = OP_MOVI, .dst = 2, .immediate = 9 },
    { .opcode = OP_HALT },
};
```

Execution result:

```text
r0 = 0
r1 = 0
r2 = 9
running = 0
```
