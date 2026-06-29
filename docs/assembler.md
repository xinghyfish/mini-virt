# Assembler

v0.4 adds a small assembler that converts text assembly into `Instruction`
structs.

The implementation is intentionally left as practice. This document describes
the planned API and behavior.

## Files

- `src/assembler.h`: public assembler API.
- `src/assembler.c`: parser implementation.
- `tests/test_assembler.c`: assembler tests.

## API

```c
AsmStatus asm_parse_line(const char *line, Instruction *out_instruction);
```

Parses one line of assembly into one `Instruction`.

```c
AsmStatus asm_parse_program(const char *source, Instruction *out_program,
                            size_t max_instructions, AsmResult *out_result);
```

Parses a multi-line source string into an array of `Instruction` values.

## Initial Syntax

Start with these forms:

```text
HALT
MOVI r0, 7
ADD r0, r1
SUB r0, r1
JMP 3
JZ r0, 3
```

For assembly text, jump targets should be instruction indexes. The assembler
should convert them into byte offsets:

```text
JMP 3 -> immediate = 3 * sizeof(Instruction)
```

## Suggested Implementation Order

1. Parse `HALT`.
2. Parse register names like `r0` through `r7`.
3. Parse unsigned immediates.
4. Parse `MOVI`.
5. Parse `ADD` and `SUB`.
6. Parse `JMP`.
7. Parse `JZ`.
8. Parse a multi-line program.
9. Add comments and blank line handling.
10. Add label support with a two-pass assembler.

See `docs/labels.md` for the next learning node.

## Comments

Suggested comment format:

```text
; this is a comment
MOVI r0, 7 ; inline comment
```

## Error Handling

Use `AsmStatus` values from `src/assembler.h`:

- `ASM_ERR_INVALID_ARGUMENT`
- `ASM_ERR_UNKNOWN_OPCODE`
- `ASM_ERR_INVALID_REGISTER`
- `ASM_ERR_INVALID_IMMEDIATE`
- `ASM_ERR_OUTPUT_TOO_SMALL`
- `ASM_ERR_UNIMPLEMENTED`
