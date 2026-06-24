#ifndef MINI_VIRT_ASSEMBLER_H
#define MINI_VIRT_ASSEMBLER_H

#include <stddef.h>

#include "instr.h"

typedef enum {
    ASM_OK = 0,
    ASM_ERR_INVALID_ARGUMENT,
    ASM_ERR_UNKNOWN_OPCODE,
    ASM_ERR_INVALID_REGISTER,
    ASM_ERR_INVALID_IMMEDIATE,
    ASM_ERR_OUTPUT_TOO_SMALL,
    ASM_ERR_UNIMPLEMENTED,
    ASM_SKIP
} AsmStatus;

typedef struct {
    size_t instruction_count;
} AsmResult;

AsmStatus asm_parse_line(const char *line, Instruction *out_instruction);
AsmStatus asm_parse_program(const char *source, Instruction *out_program,
                            size_t max_instructions, AsmResult *out_result);
const char *asm_status_string(AsmStatus status);

#endif
