// 1.13c D2Game+0x11A70: drop a rolled (or the unit's fixed-code) item at a unit.
// Retail convention: EBX = game, stack = unit, quality, &itemLevel, &itemDrop, a6, a7 (ret 0x18); the result is the new item.
// Retail is 32-bit, so the unit, game and drop record are byte blobs read by offset; every callee is a seam in retail argument order.
#pragma once
#include <cstdint>

struct D2G11A70Env {
    uint32_t (*itemLevel)(void* unit, int32_t zero);                                                   // 0xE5D0, unit in EAX, ret 4
    int32_t (*rollId)(void* game, void* seed, uint32_t ilvl, int32_t a6, int32_t a7, int32_t isMonster); // 0xFFC0, seed ESI, ilvl EDI, ret 0x10
    const void* (*itemsRec)(int32_t id);                                                                // #10695 thunk 0xA53E, ret 4
    int32_t (*rollIdForced)(void* game, void* seed, uint32_t ilvl, int32_t a6, int32_t a7);             // 0xF8F0, a6 ECX, ret 0x10
    int32_t (*idFromCode)(uint32_t code);                                                               // #10814 thunk 0xAE74, ret 4
    void (*unitCoords)(void* unit, uint32_t* outXY);                                                    // #10141 thunk 0xA544, ret 8
    void* (*unitRoom)(void* unit);                                                                      // #10331 thunk 0xA478, ret 4
    void* (*dropSpot)(void* room, uint32_t* outXY, int32_t one, const uint32_t* inXY);                  // 0xE930, inXY in ESI, ret 0xC
    int32_t (*createItem)(void* game, const uint8_t* drop, int32_t zero);                               // 0x11490, ret 0xC
    void (*fatal)(uint32_t line);                                                                       // #10265 / #10024 / exit(-1); never returns
};

extern "C" int32_t d2g_11a70(void* game, void* unit, int32_t quality, uint32_t* itemLevel, uint8_t* itemDrop,
                             int32_t a6, int32_t a7, const D2G11A70Env* env);
