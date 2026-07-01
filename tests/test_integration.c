#include <stdio.h>
#include <stdlib.h>

#include "runner.h"

#define ASSERT_TRUE(expr)                                                        \
    do {                                                                        \
        if (!(expr)) {                                                          \
            fprintf(stderr, "assertion failed: %s:%d: %s\n", __FILE__,         \
                    __LINE__, #expr);                                           \
            exit(1);                                                            \
        }                                                                       \
    } while (0)

static void test_asm_program_runs_on_vm(void)
{
    // TODO(v0.4): assemble, load, and run a program with a loop.
    char *source = "start:\n"
                   "MOVI r0, 3\n"
                   "MOVI r1, 1\n"
                   "loop:\n"
                   "JZ r0, end\n"
                   "SUB r0, r1\n"
                   "JMP loop\n"
                   "end:\n"
                   "HALT\n";
    // TODO(v0.4): assert final VM register state.
    RunResult result;
    vm_init(&result.vm);
    RunStatus run_status = runner_run_source(source, 100, &result);
    ASSERT_TRUE(run_status == RUN_OK);
    ASSERT_TRUE(result.vm.regs[0] == 0);
    ASSERT_TRUE(result.vm.regs[1] == 1);
}

static void test_runner_reports_assembler_error(void)
{
    // TODO(v0.4): pass invalid assembly source.
    char *source = "start:\n"
                   "MOVI r0, 3\n"
                   "MOVI r1, 1\n"
                   "loop:\n"
                   "JZ r0, end\n"
                   "SUB r0, r1\n"
                   "JMP loop1\n"
                   "end:\n"
                   "HALT\n";
    // TODO(v0.4): assert final VM register state.
    RunResult result;
    vm_init(&result.vm);
    RunStatus run_status = runner_run_source(source, 100, &result);
    // TODO(v0.4): assert runner returns RUN_ERR_ASSEMBLE.
    ASSERT_TRUE(run_status == RUN_ERR_ASSEMBLE);
}

static void test_runner_reports_vm_error(void)
{
    // TODO(v0.4): pass assembly that creates a VM execution error.
    char *source = "start:\n"
                   "JMP 4096\n"
                   "end:\n"
                   "HALT\n";
    // TODO(v0.4): assert final VM register state.
    RunResult result;
    vm_init(&result.vm);
    RunStatus run_status = runner_run_source(source, 100, &result);
    // TODO(v0.4): assert runner returns RUN_ERR_EXECUTE.
    ASSERT_TRUE(run_status == RUN_ERR_EXECUTE);
}

int main(void)
{
    test_asm_program_runs_on_vm();
    test_runner_reports_assembler_error();
    test_runner_reports_vm_error();

    puts("integration test placeholders passed");
    return 0;
}
