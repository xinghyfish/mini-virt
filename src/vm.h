#ifndef MINI_VIRT_VM_H
#define MINI_VIRT_VM_H

#include <stddef.h>
#include <stdint.h>

#include "instr.h"

#define VM_REG_COUNT 8
#define VM_MEMORY_SIZE 4096

typedef enum {
    VM_OK = 0,
    VM_ERR_INVALID_REGISTER,
    VM_ERR_INVALID_OPCODE,
    VM_ERR_PROGRAM_TOO_LARGE,
    VM_ERR_PC_OUT_OF_BOUNDS
} VMStatus;

typedef struct {
    uint32_t regs[VM_REG_COUNT];
    uint32_t pc;
    uint8_t memory[VM_MEMORY_SIZE];
    size_t program_size;
    int running;
} VM;

void vm_init(VM *vm);
VMStatus vm_load_program(VM *vm, const Instruction *program, size_t instruction_count);
VMStatus vm_step(VM *vm);
VMStatus vm_run(VM *vm, size_t max_steps);
void vm_dump_registers(const VM *vm);
VMStatus vm_dump_memory(const VM *vm, uint32_t start, size_t length);
void vm_dump_instruction(const Instruction *instruction);
const char *vm_status_string(VMStatus status);

#endif
