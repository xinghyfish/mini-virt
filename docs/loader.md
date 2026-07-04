# Executable Format and Loader

v0.5 starts the move from "run an in-memory instruction array" toward "load a
program artifact." This is a small step toward understanding real executable
formats such as ELF.

## Goal

Create a tiny executable format for mini-virt:

```text
source.asm -> assembler -> .mve file -> loader -> VM
```

`mve` means mini-virt executable.

## Suggested Header

Start with a fixed-size header:

```c
#define MVE_MAGIC 0x45564d4d
#define MVE_VERSION 1

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t entry;
    uint32_t instruction_count;
} MveHeader;
```

Fields:

- `magic`: identifies the file as a mini-virt executable.
- `version`: allows the format to change later.
- `entry`: byte offset where the VM should begin executing.
- `instruction_count`: number of `Instruction` records after the header.

## Suggested API

```c
typedef enum {
    LOAD_OK = 0,
    LOAD_ERR_INVALID_ARGUMENT,
    LOAD_ERR_OPEN,
    LOAD_ERR_BAD_MAGIC,
    LOAD_ERR_UNSUPPORTED_VERSION,
    LOAD_ERR_TRUNCATED,
    LOAD_ERR_PROGRAM_TOO_LARGE
} LoadStatus;
```

Current framework files:

- `src/loader.h`
- `src/loader.c`
- `tests/test_loader.c`

```c
LoadStatus loader_load_file(const char *path, Instruction *out_program,
                            size_t max_instructions, size_t *out_count,
                            uint32_t *out_entry);
```

The framework also includes:

```c
LoadStatus loader_write_file(const char *path, const Instruction *program,
                             size_t instruction_count, uint32_t entry);
```

`loader_write_file` exists so tests can create `.mve` files without hand-writing
binary blobs each time.

## Suggested Implementation Order

1. Implement argument checks in `loader_write_file`.
2. Write only the header and test that the file exists.
3. Write the instruction array after the header.
4. Implement argument checks in `loader_load_file`.
5. Read and validate the header.
6. Read the instruction array.
7. Add negative tests one file shape at a time.

## Tests To Add

- valid file loads successfully
- bad magic returns `LOAD_ERR_BAD_MAGIC`
- unsupported version returns `LOAD_ERR_UNSUPPORTED_VERSION`
- truncated header returns `LOAD_ERR_TRUNCATED`
- truncated instruction section returns `LOAD_ERR_TRUNCATED`
- too many instructions returns `LOAD_ERR_PROGRAM_TOO_LARGE`

## Completion Criteria

This node is complete when:

- a `.mve` file can store assembled instructions
- the loader can read instructions back
- the VM can run a loaded file
- invalid files fail with clear loader errors
- `make test` passes
