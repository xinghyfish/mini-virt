#include "vm.h"

#include <stdio.h>
#include <string.h>

void vm_init(VM *vm)
{
    memset(vm, 0, sizeof(*vm));
    vm->running = 0;
}

VMStatus vm_load_program(VM *vm, const Instruction *program, size_t instruction_count)
{
    const size_t program_bytes = instruction_count * sizeof(Instruction);

    if (program_bytes > VM_MEMORY_SIZE) {
        return VM_ERR_PROGRAM_TOO_LARGE;
    }

    memset(vm->memory, 0, sizeof(vm->memory));
    memcpy(vm->memory, program, program_bytes);
    vm->program_size = program_bytes;
    vm->pc = 0;
    vm->running = 1;

    return VM_OK;
}

VMStatus vm_step(VM *vm)
{
    Instruction instr;

    if (vm->pc + sizeof(Instruction) > vm->program_size) {
        vm->running = 0;
        return VM_ERR_PC_OUT_OF_BOUNDS;
    }

    memcpy(&instr, &vm->memory[vm->pc], sizeof(instr));
    if (instr.src >= VM_REG_COUNT || instr.dst >= VM_REG_COUNT) {
        vm->running = 0;
        return VM_ERR_INVALID_REGISTER;
    }
    vm->pc += sizeof(Instruction);

    switch ((Opcode)instr.opcode) {
    case OP_HALT:
        vm->running = 0;
        return VM_OK;
    case OP_MOVI:
        vm->running = 1;
        vm->regs[instr.dst] = instr.immediate;
        return VM_OK;
    case OP_ADD:
        vm->running = 1;
        vm->regs[instr.dst] += vm->regs[instr.src];
        return VM_OK;
    case OP_SUB:
        vm->running = 1;
        vm->regs[instr.dst] -= vm->regs[instr.src];
        return VM_OK;
    case OP_JMP:
        return VM_ERR_INVALID_OPCODE;
    default:
        vm->running = 0;
        return VM_ERR_INVALID_OPCODE;
    }
}

VMStatus vm_run(VM *vm, size_t max_steps)
{
    VMStatus status = VM_OK;

    for (size_t step = 0; vm->running && step < max_steps; step++) {
        status = vm_step(vm);
        if (status != VM_OK) {
            return status;
        }
    }

    return status;
}

void vm_dump_registers(const VM *vm)
{
    printf("pc=%u running=%d\n", vm->pc, vm->running);

    for (size_t i = 0; i < VM_REG_COUNT; i++) {
        printf("r%zu=%u\n", i, vm->regs[i]);
    }
}

const char *vm_status_string(VMStatus status)
{
    switch (status) {
    case VM_OK:
        return "ok";
    case VM_ERR_INVALID_REGISTER:
        return "invalid register";
    case VM_ERR_INVALID_OPCODE:
        return "invalid opcode";
    case VM_ERR_PROGRAM_TOO_LARGE:
        return "program too large";
    case VM_ERR_PC_OUT_OF_BOUNDS:
        return "pc out of bounds";
    default:
        return "unknown status";
    }
}
