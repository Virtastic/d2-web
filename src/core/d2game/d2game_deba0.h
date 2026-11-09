// 1.13c D2Game+0xDEBA0: weighted random pick among a player's skill-bearing items.
// EAX = unit A, EBX = unit B, one stack word (the game, passed on to 0x12EE0). Addresses are 32-bit retail words.
#pragma once
#include <cstdint>
struct D2GameDEBA0Env {
    uint32_t (*sub372C0)(uint32_t unit);                               // stdcall(unit)
    uint32_t (*sub12EE0)(uint32_t itemEsi, uint32_t ownerEdi, uint32_t game);  // ESI item, EDI owner, stack game
    uint32_t (*ordinal11139)(uint32_t ptr, uint32_t id);              // A7AE
    int32_t  (*ordinal10744)(uint32_t unit, uint32_t stat);           // A430
    uint32_t weightCount;                                              // D2Game+0x10E628
    const uint32_t* weightTable;                                       // D2Game+0x10E62C: {id, weight} pairs
};
uint32_t d2game_deba0_port(uint32_t eax, uint32_t ebx, uint32_t game, const D2GameDEBA0Env* e);
