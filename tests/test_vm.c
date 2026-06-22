#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

#define ASSERT_TRUE(expr)                                                        \
    do {                                                                        \
        if (!(expr)) {                                                          \
            fprintf(stderr, "assertion failed: %s:%d: %s\n", __FILE__,         \
                    __LINE__, #expr);                                           \
            exit(1);                                                            \
        }                                                                       \
    } while (0)

static void test_vm_init_clears_state(void)
{
    VM vm;

    vm_init(&vm);

    ASSERT_TRUE(vm.pc == 0);
    ASSERT_TRUE(vm.program_size == 0);
    ASSERT_TRUE(vm.running == 0);

    for (size_t i = 0; i < VM_REG_COUNT; i++) {
        ASSERT_TRUE(vm.regs[i] == 0);
    }
}

static void test_vm_runs_halt_program(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 1) == VM_OK);
    ASSERT_TRUE(vm.running == 1);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_OK);
    ASSERT_TRUE(vm.running == 0);
    ASSERT_TRUE(vm.pc == sizeof(Instruction));
}

static void test_vm_runs_mov_add_program(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 7 },
        { .opcode = OP_MOVI, .dst = 1, .immediate = 5 },
        { .opcode = OP_ADD, .dst = 0, .src = 1 },
        { .opcode = OP_SUB, .dst = 0, .src = 1 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 5) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_OK);
    ASSERT_TRUE(vm.regs[0] == 7);
    ASSERT_TRUE(vm.regs[1] == 5);
}

static void test_vm_runs_invalid_register(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 7 },
        { .opcode = OP_ADD, .dst = 0, .src = VM_REG_COUNT }, // Invalid register
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 3) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_ERR_INVALID_REGISTER);
    ASSERT_TRUE(vm.running == 0);
}

static void test_vm_jmp_normal(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 1 },
        { .opcode = OP_JMP, .immediate = 3 * sizeof(Instruction) },
        { .opcode = OP_MOVI, .dst = 0, .immediate = 99 },
        { .opcode = OP_MOVI, .dst = 1, .immediate = 7 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 5) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_OK);
    ASSERT_TRUE(vm.regs[0] == 1); // MOVI should have been skipped
    ASSERT_TRUE(vm.regs[1] == 7);
    ASSERT_TRUE(vm.running == 0);
}

static void test_vm_jmp_abnormal(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 7 },
        { .opcode = OP_JMP, .immediate = VM_MEMORY_SIZE },
        { .opcode = OP_JMP, .dst = 0, .immediate = 99 },
        { .opcode = OP_JMP, .dst = 1, .immediate = 7 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 5) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_ERR_PC_OUT_OF_BOUNDS);
}

static void test_vm_jz_taken(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 0 },
        { .opcode = OP_JZ, .src = 0, .immediate = 3 * sizeof(Instruction) },
        { .opcode = OP_MOVI, .dst = 1, .immediate = 7 },
        { .opcode = OP_MOVI, .dst = 2, .immediate = 9 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 5) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_OK);
    ASSERT_TRUE(vm.regs[1] == 0);
    ASSERT_TRUE(vm.regs[2] == 9);
    ASSERT_TRUE(vm.running == 0);
}

static void test_vm_jz_not_taken(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 1 },
        { .opcode = OP_JZ, .src = 0, .immediate = 3 * sizeof(Instruction) },
        { .opcode = OP_MOVI, .dst = 1, .immediate = 7 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 4) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_OK);
    ASSERT_TRUE(vm.regs[1] == 7);
    ASSERT_TRUE(vm.running == 0);
}

static void test_vm_jz_invalid_register(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_JZ, .src = VM_REG_COUNT, .immediate = sizeof(Instruction) },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 2) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_ERR_INVALID_REGISTER);
    ASSERT_TRUE(vm.running == 0);
}

static void test_vm_jz_invalid_target(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 0 },
        { .opcode = OP_JZ, .src = 0, .immediate = VM_MEMORY_SIZE },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 3) == VM_OK);
    ASSERT_TRUE(vm_run(&vm, 10) == VM_ERR_PC_OUT_OF_BOUNDS);
    ASSERT_TRUE(vm.running == 0);
}

static void test_vm_step_executes_one_instruction(void)
{
    // TODO(v0.3): load a short MOVI/ADD/HALT program.
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 7 },
        { .opcode = OP_MOVI, .dst = 1, .immediate = 5 },
        { .opcode = OP_ADD, .dst = 0, .src = 1 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 4) == VM_OK);
    // TODO(v0.3): call vm_step once.
    ASSERT_TRUE(vm_step(&vm) == VM_OK);
    // TODO(v0.3): assert that only the first instruction changed VM state.
    ASSERT_TRUE(vm.regs[0] == 7);
    ASSERT_TRUE(vm.regs[1] == 0);
    ASSERT_TRUE(vm.running == 1);
    ASSERT_TRUE(vm.pc == sizeof(Instruction));
    }

static void test_vm_step_updates_pc(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 7 },
        { .opcode = OP_JMP, .immediate = 3 * sizeof(Instruction) },
        { .opcode = OP_MOVI, .dst = 0, .immediate = 8 },
        { .opcode = OP_MOVI, .dst = 0, .immediate = 9 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 5) == VM_OK);
    // TODO(v0.3): call vm_step and verify pc moves by sizeof(Instruction).
    ASSERT_TRUE(vm_step(&vm) == VM_OK);
    ASSERT_TRUE(vm.pc == sizeof(Instruction));
    // TODO(v0.3): include a jump instruction case where pc changes differently.
    ASSERT_TRUE(vm_step(&vm) == VM_OK);
    ASSERT_TRUE(vm.pc == 3 * sizeof(Instruction));
}

static void test_vm_dump_memory_exists(void)
{
    // TODO(v0.3): call vm_dump_memory with a valid range.
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_MOVI, .dst = 0, .immediate = 7 },
        { .opcode = OP_JMP, .immediate = 3 * sizeof(Instruction) },
        { .opcode = OP_MOVI, .dst = 0, .immediate = 8 },
        { .opcode = OP_MOVI, .dst = 0, .immediate = 9 },
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    ASSERT_TRUE(vm_load_program(&vm, program, 5) == VM_OK);
    uint32_t start = 0;
    size_t length = sizeof(program);
    ASSERT_TRUE(vm_dump_memory(&vm, start, length) == VM_OK);
    // TODO(v0.3): later, assert invalid ranges return an error.
    ASSERT_TRUE(vm_dump_memory(&vm, VM_MEMORY_SIZE + 1, 10) == VM_ERR_PC_OUT_OF_BOUNDS);
}

static void test_vm_dump_instruction_exists(void)
{
    // TODO(v0.3): call vm_dump_instruction with one sample instruction.
    const Instruction instr = { .opcode = OP_MOVI, .dst = 0, .immediate = 7 };
    vm_dump_instruction(&instr);
    // TODO(v0.3): decide whether output should be visually inspected or captured.
}

int main(void)
{
    test_vm_init_clears_state();
    test_vm_runs_halt_program();
    test_vm_runs_mov_add_program();
    test_vm_runs_invalid_register();
    test_vm_jmp_normal();
    test_vm_jmp_abnormal();
    test_vm_jz_taken();
    test_vm_jz_not_taken();
    test_vm_jz_invalid_register();
    test_vm_jz_invalid_target();
    test_vm_step_executes_one_instruction();
    test_vm_step_updates_pc();
    test_vm_dump_memory_exists();
    test_vm_dump_instruction_exists();

    puts("all tests passed");
    return 0;
}
