# Label Support

This is the next learning node in v0.4. The goal is to make assembly programs
easier to write by replacing numeric jump targets with names.

## Why Labels Matter

Right now jumps use instruction indexes:

```text
JMP 3
JZ r0, 5
```

That works, but it is fragile. If you insert one instruction, every later jump
target may need to change.

Labels let the assembler do that bookkeeping:

```text
loop:
SUB r0, r1
JZ r0, done
JMP loop
done:
HALT
```

## Syntax

Support these forms first:

```text
label:
JMP label
JZ r0, label
```

Keep labels simple at first:

- start with a letter or underscore
- contain letters, digits, or underscores
- end with `:`
- appear alone on their own line

Examples:

```text
loop:
done:
_retry:
block2:
```

Invalid examples:

```text
2loop:
loop-name:
loop: MOVI r0, 1
```

## Two-Pass Assembly

Use a two-pass approach.

### Pass 1: Collect Labels

Walk through every source line.

- Ignore blank lines and comments.
- If the line is a label, store its name and current instruction index.
- If the line is an instruction, increment the current instruction index.

Example:

```text
loop:
MOVI r0, 3
SUB r0, r1
JZ r0, done
JMP loop
done:
HALT
```

Label table:

```text
loop -> 0
done -> 4
```

Notice that labels do not count as instructions.

### Pass 2: Emit Instructions

Walk through every source line again.

- Ignore blank lines and comments.
- Skip label definitions.
- Parse regular instructions.
- For `JMP label`, replace the label with `label_index * sizeof(Instruction)`.
- For `JZ r0, label`, do the same for the target.

## Suggested Data Structure

Start with a small fixed-size table:

```c
#define ASM_MAX_LABELS 64
#define ASM_MAX_LABEL_LENGTH 32

typedef struct {
    char name[ASM_MAX_LABEL_LENGTH];
    size_t instruction_index;
} AsmLabel;
```

This is enough for the learning project. You can replace it with a hash table
later.

## New Errors To Consider

You can add these later if useful:

```c
ASM_ERR_TOO_MANY_LABELS
ASM_ERR_INVALID_LABEL
ASM_ERR_DUPLICATE_LABEL
ASM_ERR_UNKNOWN_LABEL
```

For a first pass, using `ASM_ERR_INVALID_ARGUMENT` for label errors is also
acceptable.

## Tests To Add

Add these tests before or while implementing.

Forward label:

```text
JMP done
MOVI r0, 99
done:
HALT
```

Expected:

```text
program[0].opcode == OP_JMP
program[0].immediate == 2 * sizeof(Instruction)
```

Backward label:

```text
loop:
SUB r0, r1
JMP loop
```

Expected:

```text
program[1].opcode == OP_JMP
program[1].immediate == 0
```

Conditional label:

```text
JZ r0, done
MOVI r1, 7
done:
HALT
```

Expected:

```text
program[0].opcode == OP_JZ
program[0].src == 0
program[0].immediate == 2 * sizeof(Instruction)
```

Unknown label:

```text
JMP missing
HALT
```

Expected:

```text
ASM_ERR_UNKNOWN_LABEL
```

Duplicate label:

```text
loop:
MOVI r0, 1
loop:
HALT
```

Expected:

```text
ASM_ERR_DUPLICATE_LABEL
```

## Completion Criteria

This node is complete when:

- numeric `JMP` and `JZ` still work
- `JMP label` works
- `JZ rX, label` works
- labels do not increase `instruction_count`
- forward labels work
- backward labels work
- unknown labels fail with a clear error
- duplicate labels fail with a clear error
- `make test` passes
