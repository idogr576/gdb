#include <sys/ptrace.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>
#include <logger.h>

#include "x86_64.h"
#include "registers.h"
#include "tracee.h"

void get_current_opcode(tracee *tracee, char *opcode)
{
    reg_t rip = get_register_value(tracee, "rip");
    char buffer[OPCODE_MAX_REPR];
    if (!rip)
    {
        LOG_ERROR("cannot access register rip");
        return;
    }
    ZyanU64 runtime_address = rip;
    ZyanU8 data[OPCODE_MAX_SIZE] = {0};
    for (int i = 0; i < OPCODE_MAX_SIZE; i++)
    {
        data[i] = (ZyanU8)ptrace(PTRACE_PEEKDATA, tracee->pid, runtime_address + i, 0);
    }
    // 1. Initialize Decoder
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

    // 2. Initialize Formatter
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
    ZyanUSize offset = 0;
    ZydisDecoderDecodeFull(&decoder, data + offset, sizeof(data) - offset, &instruction, operands);

    // Format the instruction into our buffer
    ZydisFormatterFormatInstruction(&formatter, &instruction, operands,
                                    instruction.operand_count_visible, buffer, sizeof(buffer),
                                    runtime_address, ZYAN_NULL);

    sprintf(opcode, "%016" PRIX64 "  %s", runtime_address, buffer);

    // // 3. Loop and Decode - not for now
    // while (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, data + offset, sizeof(data) - offset,
    //                                            &instruction, operands)))
    // {

    //     // Format the instruction into our buffer
    //     ZydisFormatterFormatInstruction(&formatter, &instruction, operands,
    //                                     instruction.operand_count_visible, buffer, sizeof(buffer),
    //                                     runtime_address, ZYAN_NULL);

    //     printf("%016" PRIX64 "  %s\n", runtime_address, buffer);

    //     offset += instruction.length;
    //     runtime_address += instruction.length;
    // }
}