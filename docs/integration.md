# Assembler to VM Integration

This is the next learning node after label support. The goal is to prove that
the project works as a small end-to-end system:

```text
assembly source -> assembler -> Instruction array -> VM -> final state
```

## New Files

- `src/runner.h`: public API for running assembly source.
- `src/runner.c`: implementation frame for the full pipeline.
- `tests/test_integration.c`: end-to-end test placeholders.

## Planned API

```c
RunStatus runner_run_source(const char *source, size_t max_steps,
                            RunResult *out_result);
```

This function should eventually:

1. Parse assembly source with `asm_parse_program`.
2. Load the parsed program with `vm_load_program`.
3. Run the VM with `vm_run`.
4. Store the final VM state and intermediate statuses in `RunResult`.

## First End-to-End Test

Use a loop that counts `r0` down to zero:

```text
MOVI r0, 3
MOVI r1, 1
loop:
SUB r0, r1
JZ r0, done
JMP loop
done:
HALT
```

Expected final state:

```text
r0 = 0
r1 = 1
running = 0
```

## Error Path Tests

Assembler error:

```text
MOVX r0, 1
HALT
```

Expected runner status:

```text
RUN_ERR_ASSEMBLE
```

VM execution error:

```text
JMP 999
HALT
```

Expected runner status:

```text
RUN_ERR_EXECUTE
```

## Completion Criteria

This node is complete when:

- `runner_run_source` handles invalid arguments.
- assembler errors are surfaced as `RUN_ERR_ASSEMBLE`.
- VM load errors are surfaced as `RUN_ERR_LOAD`.
- VM execution errors are surfaced as `RUN_ERR_EXECUTE`.
- a loop written in assembly runs to completion on the VM.
- `make test` passes.
