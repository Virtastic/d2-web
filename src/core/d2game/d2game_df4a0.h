// 1.13c D2Game+0xDF4A0: monster drop-table pick. EAX = unit, ECX = game (0 allowed), EDX = param_2,
// stack: param_3, param_4 (out pointer), param_5 (byte), ret 0xC. Addresses are 32-bit retail words.
#pragma once
#include <cstdint>
struct D2GameDF4A0Env {
    uint32_t (*sub11D0)(uint32_t rng, uint32_t esi);   // ECX = rng pair address (game+0x6C or 0), ESI = argument
    uint32_t (*subA74E)(uint32_t id);                   // stdcall(id): ordinal returning a record pointer
    uint32_t tables;                                    // *sgptDataTables (via the IAT slot D2Game+0xF829C)
    uint32_t masks;                                     // gdwBitMasks (IAT slot D2Game+0xF8BE0 holds the array address)
};
int32_t d2game_df4a0_port(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t p3, uint32_t p4, uint32_t p5, const D2GameDF4A0Env* e);
