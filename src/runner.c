#include "runner.h"

RunStatus runner_run_source(const char *source, size_t max_steps,
                            RunResult *out_result)
{
    if (source == NULL || out_result == NULL)
        return RUN_ERR_INVALID_ARGUMENT;

    // TODO(v0.4): parse source with asm_parse_program.
    Instruction program[max_steps];
    AsmResult result;
    AsmStatus status = asm_parse_program(source, program, max_steps, &result);
    out_result->asm_result = result;
    out_result->asm_status = status;
    // TODO(v0.4): load parsed instructions with vm_load_program.
    if (status != ASM_OK)
        return RUN_ERR_ASSEMBLE;
    VMStatus vm_status = vm_load_program(&out_result->vm, program, result.instruction_count);
    out_result->vm_status = vm_status;
    if (vm_status != VM_OK)
        return RUN_ERR_LOAD;
    vm_status = vm_run(&out_result->vm, max_steps);
    out_result->vm_status = vm_status;
    if (vm_status != VM_OK)
        return RUN_ERR_EXECUTE;

    // TODO(v0.4): execute the VM with vm_run.
    // TODO(v0.4): store intermediate statuses in out_result.
    return RUN_OK;
}

const char *runner_status_string(RunStatus status)
{
    switch (status) {
    case RUN_OK:
        return "ok";
    case RUN_ERR_INVALID_ARGUMENT:
        return "invalid argument";
    case RUN_ERR_ASSEMBLE:
        return "assemble failed";
    case RUN_ERR_LOAD:
        return "load failed";
    case RUN_ERR_EXECUTE:
        return "execute failed";
    case RUN_ERR_UNIMPLEMENTED:
        return "unimplemented";
    default:
        return "unknown runner status";
    }
}
