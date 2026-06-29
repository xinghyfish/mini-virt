#ifndef MINI_VIRT_ASSEMBLER_H
#define MINI_VIRT_ASSEMBLER_H
#define ASM_MAX_LABELS 64

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
    ASM_SKIP,
    ASM_LABEL,
    ASM_ERR_TOO_MANY_LABELS,
    ASM_ERR_INVALID_LABEL,
    ASM_ERR_DUPLICATE_LABEL,
    ASM_ERR_UNKNOWN_LABEL
} AsmStatus;

typedef struct {
    size_t instruction_count;
} AsmResult;

typedef struct {
    char *name;
    size_t address;
} Label;

typedef struct {
    Label labels[ASM_MAX_LABELS];
    size_t label_count;
} LabelTable;

AsmStatus asm_parse_line(const char *line, Instruction *out_instruction);
AsmStatus asm_parse_program(const char *source, Instruction *out_program,
                            size_t max_instructions, AsmResult *out_result);
const char *asm_status_string(AsmStatus status);
void show_labels(void);

#endif
