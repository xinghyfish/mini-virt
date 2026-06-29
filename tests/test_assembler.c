#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

#define ASSERT_TRUE(expr)                                                        \
    do {                                                                        \
        if (!(expr)) {                                                          \
            fprintf(stderr, "assertion failed: %s:%d: %s\n", __FILE__,         \
                    __LINE__, #expr);                                           \
            exit(1);                                                            \
        }                                                                       \
    } while (0)

static void test_asm_parse_line_halt(void)
{
    // Test Case 1: parse "HALT" and assert opcode == OP_HALT.
    char* instruction_str = "HALT";
    Instruction ins;
    AsmStatus res = asm_parse_line(instruction_str, &ins); 
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(ins.opcode == OP_HALT);
}

static void test_asm_parse_line_movi(void)
{
    // TODO(v0.4): parse "MOVI r0, 7".
    const char* instruction_str = "MOVI r3, 10";
    Instruction ins;
    AsmStatus res = asm_parse_line(instruction_str, &ins);
    // TODO(v0.4): assert opcode, dst, and immediate fields.
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(ins.opcode == OP_MOVI);
    ASSERT_TRUE(ins.dst == 3);
    ASSERT_TRUE(ins.immediate == 10);
}

static void test_asm_parse_line_add(void)
{
    // TODO(v0.4): parse "ADD r0, r1".
    const char* instruction_str = "ADD r0, r1";
    Instruction ins;
    AsmStatus res = asm_parse_line(instruction_str, &ins);
    // TODO(v0.4): assert opcode, dst, and src fields.
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(ins.opcode == OP_ADD);
    ASSERT_TRUE(ins.dst == 0);
    ASSERT_TRUE(ins.src == 1);
}

static void test_asm_parse_line_jmp(void)
{
    // TODO(v0.4): parse "JMP 5".
    const char* instruction_str = "JMP 5";
    Instruction ins;
    AsmStatus res = asm_parse_line(instruction_str, &ins);
    // TODO(v0.4): assert opcode, dst, and src fields.
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(ins.opcode == OP_JMP);
    ASSERT_TRUE(ins.immediate == 5 * sizeof(Instruction));
}

static void test_asm_parse_line_jz(void)
{
    // TODO(v0.4): parse "JZ r0, 5".
    const char* instruction_str = "JZ r0, 5";
    Instruction ins;
    AsmStatus res = asm_parse_line(instruction_str, &ins);
    // TODO(v0.4): assert opcode, dst, and src fields.
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(ins.opcode == OP_JZ);
    ASSERT_TRUE(ins.src == 0);
    ASSERT_TRUE(ins.immediate == 5 * sizeof(Instruction));
}

static void test_asm_parse_line_rejects_unknown_opcode(void)
{
    // TODO(v0.4): parse an unknown opcode and assert ASM_ERR_UNKNOWN_OPCODE.
    char* instruction_str = "ADDI r0, r1";
    Instruction ins;
    AsmStatus res = asm_parse_line(instruction_str, &ins);
    // TODO(v0.4): assert opcode, dst, and src fields.
    ASSERT_TRUE(res == ASM_ERR_UNKNOWN_OPCODE);
}

static void test_asm_parse_program_counts_instructions(void)
{
    // TODO(v0.4): parse a multi-line program.
    // TODO(v0.4): assert out_result.instruction_count.
    const char program_str[] = 
        "MOVI r0, 7\n"
        "ADD r0, r1\n"
        "; This is a comment line\n"
        "HALT\n"
        "; This is another comment line";
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(program_str, program, 3, &result);
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(result.instruction_count == 3); 
}

static void test_asm_parse_program_rejects_small_output_buffer(void)
{
    // TODO(v0.4): pass a source program with more instructions than the output
    // buffer can hold.
    // TODO(v0.4): assert ASM_ERR_OUTPUT_TOO_SMALL.
    const char program_str[] = 
        "MOVI r0, 7\n"
        "ADD r0, r1\n"
        "HALT\n"
        "; This is just a comment line";
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(program_str, program, 2, &result);
    ASSERT_TRUE(res == ASM_ERR_OUTPUT_TOO_SMALL);
}

static void test_asm_parse_program_rejects_invalid_source(void)
{
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(NULL, program, 2, &result);
    ASSERT_TRUE(res == ASM_ERR_INVALID_ARGUMENT);
}

static void test_asm_parse_normal_label(void)
{
    const char program_str[] = 
        "start:\n"
        "JMP done\n"
        "ADD r0, r1\n"
        "JZ r0, start ; This is a comment\n"
        "done:\n"
        "HALT\n"
        "; This is just a comment line";
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(program_str, program, 4, &result);
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(program[0].immediate == 3 * sizeof(Instruction));
    ASSERT_TRUE(program[2].immediate == 0);
}

static void test_asm_parse_backward_label(void)
{
    const char program_str[] =
        "loop:\n"
        "SUB r0, r1\n"
        "JMP loop\n";
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(program_str, program, 10, &result);
    ASSERT_TRUE(res == ASM_OK);
    ASSERT_TRUE(result.instruction_count == 2);
    ASSERT_TRUE(program[1].opcode == OP_JMP);
    ASSERT_TRUE(program[1].immediate == 0);
}

static void test_asm_parse_duplicate_label(void)
{
    const char program_str[] = 
        "start:\n"
        "MOVI r0, 7\n"
        "ADD r0, r1\n"
        "start:\n"
        "HALT\n"
        "; This is just a comment line";
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(program_str, program, 3, &result);
    ASSERT_TRUE(res == ASM_ERR_DUPLICATE_LABEL);
}

static void test_asm_parse_unknown_label(void)
{
    const char program_str[] = 
        "start:\n"
        "MOVI r0, 7\n"
        "ADD r0, r1\n"
        "JMP end\n"
        "HALT\n"
        "; This is just a comment line";
    Instruction program[10];
    AsmResult result;
    AsmStatus res = asm_parse_program(program_str, program, 4, &result);
    ASSERT_TRUE(res == ASM_ERR_UNKNOWN_LABEL);
}

static void test_asm_parse_too_many_labels(void)
{
    char program_str[2048] = "";
    Instruction program[10];
    AsmResult result;

    for (size_t i = 0; i <= ASM_MAX_LABELS; i++) {
        char line[32];
        snprintf(line, sizeof(line), "label%zu:\n", i);
        strcat(program_str, line);
    }

    AsmStatus res = asm_parse_program(program_str, program, 10, &result);
    ASSERT_TRUE(res == ASM_ERR_TOO_MANY_LABELS);
}

int main(void)
{
    test_asm_parse_line_halt();
    test_asm_parse_line_movi();
    test_asm_parse_line_add();
    test_asm_parse_line_jmp();
    test_asm_parse_line_jz();
    test_asm_parse_line_rejects_unknown_opcode();
    test_asm_parse_program_counts_instructions();
    test_asm_parse_program_rejects_small_output_buffer();
    test_asm_parse_program_rejects_invalid_source();
    test_asm_parse_normal_label();
    test_asm_parse_backward_label();
    test_asm_parse_duplicate_label();
    test_asm_parse_unknown_label();
    test_asm_parse_too_many_labels();

    puts("assembler test placeholders passed");
    return 0;
}
