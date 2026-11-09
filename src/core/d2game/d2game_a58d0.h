// D2Game+0xA58D0 seams: the callees of the 20% idle-packet gate, reachable through one table so the same rule runs under the FN replay.
#pragma once
#include <cstdint>
struct D2GameA58D0Env {
    void (*clearPath)(void* unit, int zero);                       // 0xA496 (Ordinal 11103)
    const uint8_t* (*monStats)(uint32_t classId);                  // 0x1320 (EAX = class id), row or null
    void (*sendFrame)(void* game, const uint32_t* pkt, int one);   // 0x75E10
    void (*defaultAction)(void* game, int action);                 // 0xF22C0 (EAX = game)
    uint32_t flagMask;                                             // **0xF8BE0
};
void d2game_a58d0(const D2GameA58D0Env& env, void* game, uint8_t* unit);
