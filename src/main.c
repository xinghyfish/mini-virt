#include <stdio.h>

#include "vm.h"

int main(void)
{
    VM vm;
    const Instruction program[] = {
        { .opcode = OP_HALT },
    };

    vm_init(&vm);

    VMStatus status = vm_load_program(&vm, program, 1);
    if (status != VM_OK) {
        fprintf(stderr, "load failed: %s\n", vm_status_string(status));
        return 1;
    }

    status = vm_run(&vm, 100);
    if (status != VM_OK) {
        fprintf(stderr, "run failed: %s\n", vm_status_string(status));
        return 1;
    }

    vm_dump_registers(&vm);
    return 0;
}
