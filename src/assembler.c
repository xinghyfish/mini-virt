#include "assembler.h"
#include "vm.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static int8_t is_valid_register_str(const char *reg_str)
{
    return strlen(reg_str) == 2 
        && reg_str[0] == 'r' 
        && isdigit(reg_str[1])
        && strtoull(&reg_str[1], NULL, 10) < VM_REG_COUNT ? 
        (int8_t)strtoull(&reg_str[1], NULL, 10) : -1;
}

static int64_t is_valid_immediate_str(const char *imm_str)
{
    return strspn(imm_str, "0123456789") == strlen(imm_str)
        && (uint64_t) (strtoull(imm_str, NULL, 10)) <= UINT32_MAX ? 
        (int64_t)strtoull(imm_str, NULL, 10) : -1;
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
    if (strcmp(tokens[0], "HALT") == 0) {
        if ((tokens[1] == NULL && tokens[2] == NULL) || strcmp(tokens[1], ";") == 0) {
            instr.opcode = OP_HALT;
        } else {
            // HALT should not have any operands
            return ASM_ERR_INVALID_ARGUMENT;
        }
    } else if (strcmp(tokens[0], "MOVI") == 0 || strcmp(tokens[0], "JZ") == 0) {
        if (!tokens[1] || !tokens[2] || tokens[1][2] != ',') {
            // MOVI requires two operands: destination register and immediate value
            return ASM_ERR_INVALID_ARGUMENT;
        } else  {
            // Invalid register format or out of bounds
            char reg_token[3];
            strncpy(reg_token, tokens[1], 2); // Remove the comma from the first token
            reg_token[2] = '\0';
            int8_t reg = is_valid_register_str(reg_token);
            int64_t immediate_val = is_valid_immediate_str(tokens[2]);
            if (reg == -1) {
               return ASM_ERR_INVALID_REGISTER;
            } else if (immediate_val == -1) {
                return ASM_ERR_INVALID_IMMEDIATE;
            } else {
                if (strcmp(tokens[0], "MOVI") == 0) {
                    instr.dst = reg;
                    instr.opcode = OP_MOVI;
                    instr.immediate = (uint32_t) immediate_val;
                } else {
                    instr.src = reg;
                    instr.opcode = OP_JZ;
                    instr.immediate = (uint32_t) (immediate_val * sizeof(Instruction));
                }
            }
        }
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
            if (immediate_val == -1) {
                return ASM_ERR_INVALID_IMMEDIATE;
            } else {
                instr.opcode = OP_JMP;
                instr.immediate = (uint32_t) (immediate_val * sizeof(Instruction));
            }
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

    size_t count = 0;
    char* line_save;
    char* line = strtok_r((char*)buffer, "\n", &line_save);
    while (line != NULL && count <= max_instructions) {
        // TODO(v0.4): reject output buffers that are too small.
        Instruction instr;
        AsmStatus status = asm_parse_line(line, &instr);
        
        if (status == ASM_SKIP) {
            line = strtok_r(NULL, "\n", &line_save);
            continue;
        }
        else if (status == ASM_OK) {
            if (count == max_instructions)
                return ASM_ERR_OUTPUT_TOO_SMALL;
            out_program[count++] = instr;
            line = strtok_r(NULL, "\n", &line_save);
        }
        else {
            return status;
        }
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
    default:
        return "unknown assembler status";
    }
}
