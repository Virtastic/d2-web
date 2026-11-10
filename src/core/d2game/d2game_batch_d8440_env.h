// Seams of d2_fn_d2game_D8440 (d2game_batch_d62e0.cpp).
#pragma once
#include <cstdint>
struct D2GameBatchD8440Env {
    void*    (*at)(uint32_t guest);
    void     (*fatal)(uint32_t line);
    uint32_t (*o10695)(uint32_t classId);                            // 0xA53E: the item-type record
    uint32_t (*o10450)(uint32_t v, uint32_t* out);                   // 0xAA42: nonzero when it wrote *out
    void     (*sub1E10)(uint32_t game, uint32_t outV, uint32_t* esiBuf); // 0xD1E10 (esi = the 0x84-byte block it fills)
    void     (*sub39B0)(uint32_t eaxItem, uint32_t ecxUnit, uint32_t game); // 0xD39B0
    uint32_t (*sub31490)(uint32_t game, uint32_t* buf, uint32_t zero); // 0x11490
    uint32_t (*sub211D0)(uint32_t ecxSeedPtr, uint32_t esiRange);    // 0x11D0
    void     (*o10887)(uint32_t item, uint32_t stat, uint32_t v, uint32_t z); // 0xA472
    void     (*o10012)(uint32_t item, uint32_t v);                   // 0xA92E
    uint32_t (*sub76A0)(uint32_t line, const uint32_t* args);        // 0xD76A0 (ecx = file, edx = line, eight stack words)
    void     (*sub8310)(uint32_t eaxId, uint32_t game, uint32_t unit); // 0xD8310 (eax = id, edi = id)
};
