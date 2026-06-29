#include "assembler.h"
#include "vm.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#define IMMEDIATE_TOO_LARGE -1
#define NOT_IMMEDIATE -2
#define UNKNOWN_LABEL -1

static LabelTable label_table;

static int8_t is_valid_register_str(const char *reg_str)
/**
 * @brief Check if a register string is valid according to the following rules:
 * 1. The register string must be exactly 2 characters long.
 * 2. The first character must be 'r'.
 * 3. The second character must be a digit (0-7), and the value must be less than VM_REG_COUNT.
 */
{
    return strlen(reg_str) == 2 
        && reg_str[0] == 'r' 
        && isdigit(reg_str[1])
        && strtoull(&reg_str[1], NULL, 10) < VM_REG_COUNT ? 
        (int8_t)strtoull(&reg_str[1], NULL, 10) : -1;
}

static int64_t is_valid_immediate_str(const char *imm_str)
/**
 * @brief Check if an immediate string is valid according to the following rules:
 * 1. The immediate string must be non-empty.
 * 2. The immediate string can only contain digits (0-9).
 * 3. The immediate value must be within the range of a 32-bit unsigned integer
 */
{
    char *end = NULL;
    unsigned long long value;

    if (!imm_str || *imm_str == '\0') {
        return NOT_IMMEDIATE;
    }

    errno = 0;
    value = strtoull(imm_str, &end, 10);
    if (end == imm_str || *end != '\0') {
        return NOT_IMMEDIATE;
    }

    if (errno == ERANGE || value > UINT32_MAX) {
        return IMMEDIATE_TOO_LARGE;
    }

    return (int64_t)value;
}

static int64_t label_string_to_address(const char *label)
{
    for (size_t i = 0; i < label_table.label_count; i++) {
        if (strcmp(label_table.labels[i].name, label) == 0) {
            return label_table.labels[i].address;
        }
    }
    return UNKNOWN_LABEL;
}

static void clear_label_table(void)
{
    for (size_t i = 0; i < label_table.label_count; i++) {
        free(label_table.labels[i].name);
        label_table.labels[i].name = NULL;
        label_table.labels[i].address = 0;
    }
    label_table.label_count = 0;
}

static int8_t add_label(const char *label, int64_t address)
{
    if (label_table.label_count == ASM_MAX_LABELS)
        return 0;
    label_table.labels[label_table.label_count].name = strdup(label);
    label_table.labels[label_table.label_count].address = address;
    label_table.label_count++;
    return 1;
}

static int8_t jump_target_to_byte_offset(int64_t instruction_index,
                                         uint32_t *out_offset)
{
    if (instruction_index < 0 ||
        (uint64_t)instruction_index > UINT32_MAX / sizeof(Instruction)) {
        return 0;
    }

    *out_offset = (uint32_t)((uint64_t)instruction_index * sizeof(Instruction));
    return 1;
}

static int8_t is_valid_label(const char *label)
/**
 * @brief Check if a label is valid according to the following rules:
 * 1. The label must be non-empty.
 * 2. The label can only contain alphanumeric characters and underscores.
 * 3. The first character of the label must be an alphabetic character or an underscore
 */
{
    return strlen(label) > 0 
        && strspn(label, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == strlen(label)
        && (isalpha(label[0]) || label[0] == '_')
        && !(label[0] >= '0' && label[0] <= '9');
}

AsmStatus asm_parse_line(const char *line, Instruction *out_instruction)
{
    // TODO(v0.4): parse one assembly line into one Instruction.
    char line_buffer[256];
    char* tokens[3] = {NULL, NULL, NULL};

    if (!line || !out_instruction) {
        return ASM_ERR_INVALID_ARGUMENT;
    }

    if (strlen(line) >= sizeof(line_buffer)) {
        return ASM_ERR_INVALID_ARGUMENT;
    }

    strcpy(line_buffer, line);

    char* token_save;
    char* t = strtok_r(line_buffer, " ", &token_save);
    size_t i = 0;
    while (t != NULL && i < 3) {
        tokens[i] = t;
        t = strtok_r(NULL, " ", &token_save);
        i++;
    }
    // TODO(v0.4): support HALT, MOVI, ADD, SUB, JMP, and JZ.
    Instruction instr;
    memset(&instr, 0, sizeof(Instruction));
    instr.opcode = 0xFF; // Invalid opcode by default
    
    if (!tokens[0] || strcmp(tokens[0], ";") == 0) {
        // Ignore comment lines
        return ASM_SKIP;
    }
    size_t token0_len = strlen(tokens[0]);
    char candidate_label[token0_len];
    strncpy(candidate_label, tokens[0], token0_len - 1);
    candidate_label[token0_len - 1] = '\0';
    if (i == 1 && tokens[0][token0_len - 1] == ':') {
        if (is_valid_label(candidate_label)) {
            return ASM_LABEL;
        } else {
            return ASM_ERR_INVALID_LABEL;
        }
    }
    if (strcmp(tokens[0], "HALT") == 0) {
        if ((tokens[1] == NULL && tokens[2] == NULL) || strcmp(tokens[1], ";") == 0) {
            instr.opcode = OP_HALT;
        } else {
            // HALT should not have any operands
            return ASM_ERR_INVALID_ARGUMENT;
        }
    } else if (strcmp(tokens[0], "MOVI") == 0) {
        if (!tokens[1] || !tokens[2] || tokens[1][2] != ',')
            // MOVI requires two operands: destination register and immediate value
            return ASM_ERR_INVALID_ARGUMENT;
        
        // Invalid register format or out of bounds
        char reg_token[3];
        strncpy(reg_token, tokens[1], 2); // Remove the comma from the first token
        reg_token[2] = '\0';
        int8_t reg = is_valid_register_str(reg_token);
        if (reg == -1)
            return ASM_ERR_INVALID_REGISTER;
        int64_t immediate_val = is_valid_immediate_str(tokens[2]);
        if (immediate_val == IMMEDIATE_TOO_LARGE)
            return ASM_ERR_INVALID_IMMEDIATE;
        if (immediate_val == NOT_IMMEDIATE)
            return ASM_ERR_INVALID_IMMEDIATE;

        instr.dst = reg;
        instr.opcode = OP_MOVI;
        instr.immediate = (uint32_t) immediate_val;
    } else if (strcmp(tokens[0], "JZ") == 0) {
        if (!tokens[1] || !tokens[2] || tokens[1][2] != ',')
            return ASM_ERR_INVALID_ARGUMENT;

        char reg_token[3];
        strncpy(reg_token, tokens[1], 2);
        reg_token[2] = '\0';
        int8_t reg = is_valid_register_str(reg_token);
        if (reg == -1)
            return ASM_ERR_INVALID_REGISTER;

        int64_t immediate_val = is_valid_immediate_str(tokens[2]);
        if (immediate_val == IMMEDIATE_TOO_LARGE) {
            return ASM_ERR_INVALID_IMMEDIATE;
        } else if (immediate_val == NOT_IMMEDIATE) {
            immediate_val = label_string_to_address(tokens[2]);
            if (immediate_val == UNKNOWN_LABEL) {
                return ASM_ERR_UNKNOWN_LABEL;
            }
        }

        uint32_t target_offset;
        if (!jump_target_to_byte_offset(immediate_val, &target_offset))
            return ASM_ERR_INVALID_IMMEDIATE;

        instr.src = reg;
        instr.opcode = OP_JZ;
        instr.immediate = target_offset;
    } else if (strcmp(tokens[0], "ADD") == 0 || strcmp(tokens[0], "SUB") == 0) {
        if (!tokens[1] || !tokens[2] || tokens[1][2] != ',') {
            // MOVI requires two operands: destination register and immediate value
            return ASM_ERR_INVALID_ARGUMENT;
        } else  {
            // Invalid register format or out of bounds
            char reg_token[3];
            strncpy(reg_token, tokens[1], 2); // Remove the comma from the first token
            reg_token[2] = '\0';
            int8_t dst_reg = is_valid_register_str(reg_token);
            int8_t src_reg = is_valid_register_str(tokens[2]);
            if (dst_reg == -1 || src_reg == -1) {
               return ASM_ERR_INVALID_REGISTER;
            } else {
                instr.dst = (uint8_t) dst_reg;
                instr.src = (uint8_t) src_reg;
                if (strcmp(tokens[0], "ADD") == 0) {
                    instr.opcode = OP_ADD;
                } else {
                    instr.opcode = OP_SUB;
                }
            }
        }
    } else if (strcmp(tokens[0], "JMP") == 0) {
        if (!tokens[1] || (tokens[2] && (strcmp(tokens[2], ";") == 0))) {
            return ASM_ERR_INVALID_ARGUMENT;
        } else {
            int64_t immediate_val = is_valid_immediate_str(tokens[1]);
            if (immediate_val == IMMEDIATE_TOO_LARGE) {
                return ASM_ERR_INVALID_IMMEDIATE;
            } else if (immediate_val == NOT_IMMEDIATE) {
                immediate_val = label_string_to_address(tokens[1]);
                if (immediate_val == UNKNOWN_LABEL) {
                    return ASM_ERR_UNKNOWN_LABEL;
                }
            }
            uint32_t target_offset;
            if (!jump_target_to_byte_offset(immediate_val, &target_offset))
                return ASM_ERR_INVALID_IMMEDIATE;

            instr.opcode = OP_JMP;
            instr.immediate = target_offset;
        }
    }

    if (instr.opcode == 0xFF) {
        return ASM_ERR_UNKNOWN_OPCODE;
    } else {
        *out_instruction = instr;
        return ASM_OK;
    }
}

AsmStatus asm_parse_program(const char *source, Instruction *out_program,
                            size_t max_instructions, AsmResult *out_result)
{
    if (!source || !out_program || !out_result) {
        return ASM_ERR_INVALID_ARGUMENT;
    }
    char buffer[strlen(source) + 1];
    // TODO(v0.4): split source into lines and call asm_parse_line.
    strcpy(buffer, source);
    clear_label_table();

    size_t count = 0;
    char* line_save;
    char* line = strtok_r((char*)buffer, "\n", &line_save);
    while (line != NULL && label_table.label_count <= ASM_MAX_LABELS)
    {
        char line_copy[256];
        char *token_save = NULL;
        char *first_token;

        if (strlen(line) >= sizeof(line_copy)) {
            return ASM_ERR_INVALID_ARGUMENT;
        }

        strcpy(line_copy, line);
        first_token = strtok_r(line_copy, " ", &token_save);
        if (!first_token || strcmp(first_token, ";") == 0) {
            line = strtok_r(NULL, "\n", &line_save);
            continue;
        }

        size_t n = strlen(first_token);
        if (n > 0 && first_token[n - 1] == ':') {
            char label[n];
            strncpy(label, first_token, n - 1);
            label[n - 1] = '\0';
            if (!is_valid_label(label)) {
                return ASM_ERR_INVALID_LABEL;
            }
            if (label_string_to_address(label) == -1) {
                if (!add_label(label, count)) {
                    return ASM_ERR_TOO_MANY_LABELS;
                }
            } else {
                return ASM_ERR_DUPLICATE_LABEL;
            }
        } else {
            count++;
        }
        line = strtok_r(NULL, "\n", &line_save);
    }
    count = 0;
    strcpy(buffer, source);
    line = strtok_r((char*)buffer, "\n", &line_save);
    while (line != NULL && count <= max_instructions) {
        // TODO(v0.4): reject output buffers that are too small.
        Instruction instr;
        AsmStatus status = asm_parse_line(line, &instr);
        
        if (status == ASM_OK) {
            if (count == max_instructions)
                return ASM_ERR_OUTPUT_TOO_SMALL;
            out_program[count++] = instr;
        } else if (status != ASM_SKIP && status != ASM_LABEL) {
            return status;
        }
        line = strtok_r(NULL, "\n", &line_save);
    }

    // TODO(v0.4): set out_result->instruction_count.
    out_result->instruction_count = count;
    return ASM_OK;
}

const char *asm_status_string(AsmStatus status)
{
    switch (status) {
    case ASM_OK:
        return "ok";
    case ASM_ERR_INVALID_ARGUMENT:
        return "invalid argument";
    case ASM_ERR_UNKNOWN_OPCODE:
        return "unknown opcode";
    case ASM_ERR_INVALID_REGISTER:
        return "invalid register";
    case ASM_ERR_INVALID_IMMEDIATE:
        return "invalid immediate";
    case ASM_ERR_OUTPUT_TOO_SMALL:
        return "output too small";
    case ASM_ERR_UNIMPLEMENTED:
        return "unimplemented";
    case ASM_SKIP:
        return "skip";
    case ASM_LABEL:
        return "label";
    case ASM_ERR_TOO_MANY_LABELS:
        return "too many labels";
    case ASM_ERR_INVALID_LABEL:
        return "invalid label";
    case ASM_ERR_DUPLICATE_LABEL:
        return "duplicate label";
    case ASM_ERR_UNKNOWN_LABEL:
        return "unknown label";
    default:
        return "unknown assembler status";
    }
}

void show_labels(void)
{
    for (size_t i = 0; i < label_table.label_count; i++) {
        printf("%s -> %zu\n", label_table.labels[i].name,
               label_table.labels[i].address);
    }
}
