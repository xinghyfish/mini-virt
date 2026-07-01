#ifndef MINI_VIRT_RUNNER_H
#define MINI_VIRT_RUNNER_H

#include <stddef.h>

#include "assembler.h"
#include "vm.h"

typedef enum {
    RUN_OK = 0,
    RUN_ERR_INVALID_ARGUMENT,
    RUN_ERR_ASSEMBLE,
    RUN_ERR_LOAD,
    RUN_ERR_EXECUTE,
    RUN_ERR_UNIMPLEMENTED
} RunStatus;

typedef struct {
    VM vm;
    AsmResult asm_result;
    AsmStatus asm_status;
    VMStatus vm_status;
} RunResult;

RunStatus runner_run_source(const char *source, size_t max_steps,
                            RunResult *out_result);
const char *runner_status_string(RunStatus status);

#endif
