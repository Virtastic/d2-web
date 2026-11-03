// Seam shared by D2Game+0x6ABB0, +0x6AC30 and +0x6ACE0 (the effect-table missile spawners). Every callee is a hooked function in the capture;
// the port passes the words retail passes, the probe logs them as the descriptor's hook lines do.
#pragma once
#include <cstdint>
struct D2GameSpawnEnv {
    uint32_t (*statValue)(uint32_t esi, uint32_t ebx, uint32_t id);           // 0x69EE0 (ESI = ctx+8, EBX = rec+4, stack word 0 = selector; ret 4)
    uint32_t (*createMissile)(uint32_t ecx, uint32_t edx, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3, uint32_t w4);   // 0x6A960 (ret 0x14)
    uint32_t (*unitA460)(uint32_t unit);                                      // 0xA460 (1 stack word)
    void (*unitA472)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c);      // 0xA472 (4 stack words)
    void (*unitA76C)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c);      // 0xA76C (4 stack words)
    void (*fatal)(uint32_t nLine);                                            // Fog 10265/10024
};
// pCtx = {+4 game-side word, +8 unit word}; pRec = {byte idx at +0, +4, +8, +0xC words}; pTable = the effect table (module +0xFA5F8, 12-byte rows {-, sel, arg}),
// nCount = the row count (module +0x107D4C).
void D2Game_6ABB0(const uint8_t* pCtx, const uint8_t* pRec, const uint32_t* pTable, int32_t nCount, const D2GameSpawnEnv* pEnv);
void D2Game_6AC30(const uint8_t* pCtx, const uint8_t* pRec, const uint32_t* pTable, int32_t nCount, const D2GameSpawnEnv* pEnv);
void D2Game_6ACE0(const uint8_t* pCtx, const uint8_t* pRec, const uint32_t* pTable, int32_t nCount, const D2GameSpawnEnv* pEnv);
