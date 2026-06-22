#ifndef MINI_VIRT_INSTR_H
#define MINI_VIRT_INSTR_H

#include <stdint.h>

typedef enum {
    OP_HALT = 0x00,
    OP_MOVI = 0x01,
    OP_ADD = 0x02,
    OP_SUB = 0x03,
    OP_JMP = 0x04
} Opcode;

typedef struct {
    uint8_t opcode;
    uint8_t dst;
    uint8_t src;
    uint8_t reserved;
    uint32_t immediate;
} Instruction;

#endif
