// 1.13c D2Game+0x8CF80: ten-byte corpse-assignment packet {0x8E, kind byte, player id dword, corpse id dword} handed to the
// 0xDFC10 per-client broadcast with the per-client sender 0x6FCAC530 as callback (EBX). EAX = player id, ECX = corpse id,
// stack byte = kind (ret 4).
#include <cstdint>
#include <cstring>
void D2Game_8CF80_CorpseAssignBroadcast(void (*broadcast)(const void* packet), uint32_t eax, uint32_t ecx, uint8_t kind)
{
    uint8_t b[12] = { 0x8E };
    b[1] = kind;
    std::memcpy(b + 2, &eax, 4);
    std::memcpy(b + 6, &ecx, 4);
    broadcast(b);
}
