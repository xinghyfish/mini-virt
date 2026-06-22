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

static void test_rm_runs_invalid_register(void)
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

int main(void)
{
    test_vm_init_clears_state();
    test_vm_runs_halt_program();
    test_vm_runs_mov_add_program();
    test_rm_runs_invalid_register();
    puts("all tests passed");
    return 0;
}
