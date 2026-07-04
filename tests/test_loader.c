#include <stdio.h>
#include <stdlib.h>

#include "loader.h"

#define ASSERT_TRUE(expr)                                                        \
    do {                                                                        \
        if (!(expr)) {                                                          \
            fprintf(stderr, "assertion failed: %s:%d: %s\n", __FILE__,         \
                    __LINE__, #expr);                                           \
            exit(1);                                                            \
        }                                                                       \
    } while (0)


static void test_load_unexistent_file(void)
{
    // TODO(v0.5): attempt to load a non-existent file.
    char path[] = "build/test_loader_load_nonexistent_file.mve";
    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &instruction_count, &loaded_entry);
    // TODO(v0.5): assert LOAD_ERR_OPEN.
    ASSERT_TRUE(load_status == LOAD_ERR_OPEN);
}

static void test_loader_invalid_arguments(void)
{
    // TODO(v0.5): attempt to load a file with invalid arguments.
    char path[] = "build/test_loader_invalid_arguments.mve";
    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(NULL, loaded_program, 10, &instruction_count, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_ERR_INVALID_ARGUMENT);
    load_status = loader_load_file(path, NULL, 10, &instruction_count, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_ERR_INVALID_ARGUMENT);
    load_status = loader_load_file(path, loaded_program, 10, NULL, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_ERR_INVALID_ARGUMENT);
    load_status = loader_load_file(path, loaded_program, 10, &instruction_count, NULL);
    ASSERT_TRUE(load_status == LOAD_ERR_INVALID_ARGUMENT);
}

static void test_loader_writes_and_loads_valid_file(void)
{
    // TODO(v0.5): write a small program to a temporary .mve file.
    Instruction program[10] = {
        {.opcode = OP_MOVI, .dst = 2, .immediate = 42},
        {.opcode = OP_MOVI, .dst = 0, .immediate = 3},
        {.opcode = OP_MOVI, .dst = 1, .immediate = 1},
        {.opcode = OP_JZ, .src = 0, .immediate = 5},
        {.opcode = OP_SUB, .dst = 0, .src = 1},
        {.opcode = OP_JMP, .immediate = 2},
        {.opcode = OP_HALT},
    };
    char path[] = "build/test_loader_writes_and_loads_valid_file.mve";
    uint32_t entry = sizeof(Instruction);
    LoadStatus write_status = loader_write_file(path, program, 7, entry);
    ASSERT_TRUE(write_status == LOAD_OK);
    // TODO(v0.5): load it back with loader_load_file.
    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &instruction_count, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_OK);
    // TODO(v0.5): assert instruction count, entry, and instruction fields.
    ASSERT_TRUE(instruction_count == 7);
    ASSERT_TRUE(entry == loaded_entry);
    ASSERT_TRUE(loaded_program[0].opcode == OP_MOVI && loaded_program[0].dst == 2 && loaded_program[0].immediate == 42);
    ASSERT_TRUE(loaded_program[1].opcode == OP_MOVI && loaded_program[1].dst == 0 && loaded_program[1].immediate == 3);
    ASSERT_TRUE(loaded_program[2].opcode == OP_MOVI && loaded_program[2].dst == 1 && loaded_program[2].immediate == 1);
    ASSERT_TRUE(loaded_program[3].opcode == OP_JZ && loaded_program[3].src == 0 && loaded_program[3].immediate == 5);
    ASSERT_TRUE(loaded_program[4].opcode == OP_SUB && loaded_program[4].dst == 0 && loaded_program[4].src == 1);
    ASSERT_TRUE(loaded_program[5].opcode == OP_JMP && loaded_program[5].immediate == 2);
    ASSERT_TRUE(loaded_program[6].opcode == OP_HALT);
}

static void test_loader_rejects_bad_magic(void)
{
    // TODO(v0.5): create a file with a bad magic value.
    Instruction program[5] = {
        {.opcode = OP_HALT},
    };
    char path[] = "build/test_loader_reject_bad_magic.mve";
    uint32_t entry = 0;
    LoadStatus write_status = loader_write_file(path, program, 1, entry);
    FILE *file = fopen(path, "r+b");
    fseek(file, 0, SEEK_SET);
    MveHeader header = {
        .magic = 0xDEADBEEF,
        .version = MVE_VERSION,
        .entry = entry,
        .instruction_count = 1,
    };
    fwrite(&header, sizeof(MveHeader), 1, file);
    fclose(file);

    ASSERT_TRUE(write_status == LOAD_OK);
    // TODO(v0.5): load it back with loader_load_file.
    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &instruction_count, &loaded_entry);
    // TODO(v0.5): assert LOAD_ERR_BAD_MAGIC.
    ASSERT_TRUE(load_status == LOAD_ERR_BAD_MAGIC);
}

static void test_loader_rejects_unsupported_version(void)
{
    // TODO(v0.5): create a file with an unsupported version.
    Instruction program[5] = {
        {.opcode = OP_HALT},
    };
    char path[] = "build/test_loader_reject_unsupported_version.mve";
    uint32_t entry = 0;
    LoadStatus write_status = loader_write_file(path, program, 1, entry);
    ASSERT_TRUE(write_status == LOAD_OK);
    FILE *file = fopen(path, "wb");
    MveHeader header = {
        .magic = MVE_MAGIC,
        .version = 2U,
        .entry = entry,
        .instruction_count = 1,
    };
    fwrite(&header, sizeof(MveHeader), 1, file);
    fclose(file);

    // TODO(v0.5): assert LOAD_ERR_UNSUPPORTED_VERSION.
    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &instruction_count, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_ERR_UNSUPPORTED_VERSION);
}

static void test_loader_rejects_truncated_header(void)
{
    // TODO(v0.5): create a file shorter than sizeof(MveHeader).
    char path[] = "build/test_loader_reject_truncated_header.mve";
    FILE *file = fopen(path, "wb");
    uint32_t magic = MVE_MAGIC;
    MveHeader header = {
        .magic = magic,
        .version = MVE_VERSION,
        .entry = 0,
        .instruction_count = 5,
    };
    fwrite(&header, sizeof(MveHeader) - 1, 1, file);
    fclose(file);

    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &instruction_count, &loaded_entry);
    // TODO(v0.5): assert LOAD_ERR_TRUNCATED.
    ASSERT_TRUE(load_status == LOAD_ERR_TRUNCATED);
}

static void test_loader_rejects_truncated_program(void)
{
    // TODO(v0.5): create a valid header but fewer Instruction records than
    // instruction_count says.
    Instruction program[] = {
        {.opcode = OP_HALT},
    };
    char path[] = "build/test_loader_reject_truncated_program.mve";
    size_t instruction_count = 2;
    LoadStatus write_status = loader_write_file(path, program, 1, 0);
    ASSERT_TRUE(write_status == LOAD_OK);
    FILE *file = fopen(path, "r+b");
    fseek(file, 0, SEEK_SET);
    MveHeader header;
    header.instruction_count = 1998;
    fseek(file, (long)offsetof(MveHeader, instruction_count), SEEK_SET);
    int written = fwrite(&instruction_count, sizeof(uint32_t), 1, file);
    ASSERT_TRUE(written == 1);
    fclose(file);
    size_t out_instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &out_instruction_count, &loaded_entry);
    // TODO(v0.5): assert LOAD_ERR_TRUNCATED.
    ASSERT_TRUE(load_status == LOAD_ERR_TRUNCATED);
}

static void test_loader_rejects_too_many_instructions(void)
{
    // TODO(v0.5): create a header whose instruction_count exceeds the output
    // buffer capacity.
    Instruction program[10] = {
        {.opcode = OP_MOVI, .dst = 2, .immediate = 42},
        {.opcode = OP_MOVI, .dst = 0, .immediate = 3},
        {.opcode = OP_MOVI, .dst = 1, .immediate = 1},
        {.opcode = OP_JZ, .src = 0, .immediate = 5},
        {.opcode = OP_SUB, .dst = 0, .src = 1},
        {.opcode = OP_JMP, .immediate = 2},
        {.opcode = OP_HALT},
    };
    char path[] = "build/test_loader_reject_too_many_instructions.mve";
    LoadStatus write_status = loader_write_file(path, program, 7, 0);
    ASSERT_TRUE(write_status == LOAD_OK);
    size_t instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    LoadStatus load_status = loader_load_file(path, loaded_program, 5, &instruction_count, &loaded_entry);
    // TODO(v0.5): assert LOAD_ERR_PROGRAM_TOO_LARGE.
    ASSERT_TRUE(load_status == LOAD_ERR_PROGRAM_TOO_LARGE);
}

static void test_loader_reject_bad_entry(void)
{
    // TODO(v0.5): create a file with a valid header but an entry point that is
    // out of bounds.
    Instruction program[10] = {
        {.opcode = OP_MOVI, .dst = 2, .immediate = 42},
        {.opcode = OP_MOVI, .dst = 0, .immediate = 3},
        {.opcode = OP_MOVI, .dst = 1, .immediate = 1},
        {.opcode = OP_JZ, .src = 0, .immediate = 5},
        {.opcode = OP_SUB, .dst = 0, .src = 1},
        {.opcode = OP_JMP, .immediate = 2},
        {.opcode = OP_HALT},
    };
    char path[] = "build/test_loader_reject_bad_entry.mve";
    remove(path);
    LoadStatus write_status = loader_write_file(path, program, 7, 7);
    ASSERT_TRUE(write_status == LOAD_ERR_BAD_ENTRY);
    FILE *file = fopen(path, "rb");
    ASSERT_TRUE(file == NULL);

    write_status = loader_write_file(path, program, 7, 0);
    ASSERT_TRUE(write_status == LOAD_OK);
    size_t out_instruction_count = 0;
    uint32_t loaded_entry = 0;
    Instruction loaded_program[10] = {0};
    file = fopen(path, "r+b");
    fseek(file, (long)offsetof(MveHeader, entry), SEEK_SET);
    uint32_t unaligned_entry = 3 * sizeof(Instruction) + 1;
    fwrite(&unaligned_entry, sizeof(uint32_t), 1, file);
    fseek(file, 0, SEEK_SET);
    MveHeader header;
    size_t read = fread(&header, sizeof(MveHeader), 1, file);
    ASSERT_TRUE(read == 1);
    fclose(file);
    // TODO(v0.5): assert LOAD_ERR_TRUNCATED.
    LoadStatus load_status = loader_load_file(path, loaded_program, 10, &out_instruction_count, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_ERR_BAD_ENTRY);
    file = fopen(path, "r+b");
    fseek(file, (long)offsetof(MveHeader, entry), SEEK_SET);
    uint32_t out_of_bounds_entry = 10 * sizeof(Instruction);
    fwrite(&out_of_bounds_entry, sizeof(uint32_t), 1, file);
    fseek(file, 0, SEEK_SET);
    read = fread(&header, sizeof(MveHeader), 1, file);
    ASSERT_TRUE(read == 1);
    fclose(file);
    load_status = loader_load_file(path, loaded_program, 10, &out_instruction_count, &loaded_entry);
    ASSERT_TRUE(load_status == LOAD_ERR_BAD_ENTRY);
}

int main(void)
{
    test_loader_invalid_arguments();
    test_load_unexistent_file();
    test_loader_writes_and_loads_valid_file();
    test_loader_rejects_bad_magic();
    test_loader_rejects_unsupported_version();
    test_loader_rejects_truncated_header();
    test_loader_rejects_truncated_program();
    test_loader_rejects_too_many_instructions();
    test_loader_reject_bad_entry();

    puts("loader tests passed");
    return 0;
}
