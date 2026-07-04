#include "loader.h"

#include <limits.h>
#include <stdio.h>

static int loader_instruction_bytes(size_t instruction_count, size_t *out_bytes)
{
    if (instruction_count > SIZE_MAX / sizeof(Instruction)) {
        return 0;
    }

    *out_bytes = instruction_count * sizeof(Instruction);
    return 1;
}

static int loader_entry_is_valid(uint32_t entry, size_t instruction_count)
{
    size_t program_bytes = 0;
    if (!loader_instruction_bytes(instruction_count, &program_bytes)) {
        return 0;
    }

    return entry < program_bytes && entry % sizeof(Instruction) == 0;
}

LoadStatus loader_write_file(const char *path, const Instruction *program,
                             size_t instruction_count, uint32_t entry)
{
    // TODO(v0.5): validate arguments.
    if (!path || !program || instruction_count == 0) {
        return LOAD_ERR_INVALID_ARGUMENT;
    }
    if (instruction_count > UINT32_MAX) {
        return LOAD_ERR_PROGRAM_TOO_LARGE;
    }
    if (!loader_entry_is_valid(entry, instruction_count)) {
        return LOAD_ERR_BAD_ENTRY;
    }

    // TODO(v0.5): write MveHeader.
    FILE *file = fopen(path, "wb");
    if (!file)
        return LOAD_ERR_OPEN;

    MveHeader header = {
        .magic = MVE_MAGIC,
        .version = MVE_VERSION,
        .entry = entry,
        .instruction_count = (uint32_t) instruction_count,
    };
    size_t written = fwrite(&header, sizeof(MveHeader), 1, file);
    if (written != 1) {
        fclose(file);
        return LOAD_ERR_WRITE;
    }
    for (size_t i = 0; i < instruction_count; i++) {
        written = fwrite(&program[i], sizeof(Instruction), 1, file);
        if (written != 1) {
            fclose(file);
            return LOAD_ERR_WRITE;
        }
    }
    fclose(file);
    return LOAD_OK;
}

LoadStatus loader_load_file(const char *path, Instruction *out_program,
                            size_t max_instructions, size_t *out_count,
                            uint32_t *out_entry)
{
    // TODO(v0.5): validate arguments.
    if (!path || !out_program || !out_count || !out_entry || max_instructions == 0) {
        return LOAD_ERR_INVALID_ARGUMENT;
    }
    // TODO(v0.5): read and validate MveHeader.
    FILE *file = fopen(path, "rb");
    if (!file) {
        return LOAD_ERR_OPEN;
    }
    MveHeader header;
    size_t read = fread(&header, sizeof(MveHeader), 1, file);
    if (read != 1) {
        fclose(file);
        return LOAD_ERR_TRUNCATED;
    }
    // TODO(v0.5): reject bad magic and unsupported versions.
    if (header.magic != MVE_MAGIC) {
        fclose(file);
        return LOAD_ERR_BAD_MAGIC;
    }
    if (header.version != MVE_VERSION) {
        fclose(file);
        return LOAD_ERR_UNSUPPORTED_VERSION;
    }
    // TODO(v0.5): reject instruction counts larger than max_instructions.
    if (header.instruction_count > max_instructions) {
        fclose(file);
        return LOAD_ERR_PROGRAM_TOO_LARGE;
    }
    // TODO(v0.5): read Instruction records into out_program.
    size_t read_count = fread(out_program, sizeof(Instruction), header.instruction_count, file);
    // TODO(v0.5): return LOAD_ERR_TRUNCATED for short files.
    if (read_count != header.instruction_count) {
        fclose(file);
        return LOAD_ERR_TRUNCATED;
    }
    if (!loader_entry_is_valid(header.entry, header.instruction_count)) {
        fclose(file);
        return LOAD_ERR_BAD_ENTRY;
    }

    *out_count = header.instruction_count;
    *out_entry = header.entry;
    fclose(file);
    return LOAD_OK;
}

const char *loader_status_string(LoadStatus status)
{
    switch (status) {
    case LOAD_OK:
        return "ok";
    case LOAD_ERR_INVALID_ARGUMENT:
        return "invalid argument";
    case LOAD_ERR_OPEN:
        return "open failed";
    case LOAD_ERR_BAD_MAGIC:
        return "bad magic";
    case LOAD_ERR_UNSUPPORTED_VERSION:
        return "unsupported version";
    case LOAD_ERR_TRUNCATED:
        return "truncated file";
    case LOAD_ERR_PROGRAM_TOO_LARGE:
        return "program too large";
    case LOAD_ERR_WRITE:
        return "write failed";
    case LOAD_ERR_BAD_ENTRY:
        return "bad entry point";
    default:
        return "unknown loader status";
    }
}
