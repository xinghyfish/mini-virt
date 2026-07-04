#ifndef MINI_VIRT_LOADER_H
#define MINI_VIRT_LOADER_H

#include <stddef.h>
#include <stdint.h>

#include "instr.h"

#define MVE_MAGIC 0x45564d4dU
#define MVE_VERSION 1U

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t entry;
    uint32_t instruction_count;
} MveHeader;

typedef enum {
    LOAD_OK = 0,
    LOAD_ERR_INVALID_ARGUMENT,
    LOAD_ERR_OPEN,
    LOAD_ERR_BAD_MAGIC,
    LOAD_ERR_UNSUPPORTED_VERSION,
    LOAD_ERR_BAD_ENTRY,
    LOAD_ERR_TRUNCATED,
    LOAD_ERR_PROGRAM_TOO_LARGE,
    LOAD_ERR_WRITE
} LoadStatus;

LoadStatus loader_write_file(const char *path, const Instruction *program,
                             size_t instruction_count, uint32_t entry);
LoadStatus loader_load_file(const char *path, Instruction *out_program,
                            size_t max_instructions, size_t *out_count,
                            uint32_t *out_entry);
const char *loader_status_string(LoadStatus status);

#endif
