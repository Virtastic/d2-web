#pragma once
#include <cstdint>

// 1.13c D2Client+0x46230 (price_line_46230.cpp): the callees of the tooltip's "Sell value" line, bound by the caller.
struct D2ClientPriceLineCb {
    int32_t (*itemType)(void* item);                        // 0xC220 (4 gives no line)
    int32_t (*itemId)(uint32_t id);                         // 0xC1BA
    bool (*vendor)(int32_t* cls);                           // 0x46150: false = no vendor unit
    int32_t (*sellable)(void* item);                        // 0xC970
    int32_t (*cost)(void* item, int32_t npc, int32_t type); // 0xC844
    const uint16_t* (*str)(uint32_t id);                    // 0xD1EC
    int32_t (*repairFlag)();                                // 0xC58C
    int32_t (*gambleOk)(void* item);                        // 0xC1A8
    int32_t (*b5b40)(void* item);                           // 0x45B40
    int32_t (*b5bd0)(void* item);                           // 0x45BD0
    void (*done)(int32_t);                                  // 0xBF5F0
};
extern "C" int32_t d2_client_46230(int32_t* amt, void* item, int32_t carried, uint16_t* buf, int32_t cap,
                                   int32_t uiState, int32_t vstock, const D2ClientPriceLineCb* cb);
