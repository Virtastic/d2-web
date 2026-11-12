// The seam table of the AD860 batch (d2game_batch_ad860.cpp); a probe fills it with recorders.
#pragma once
#include <cstdint>

struct D2GameBatchAD860Env {
    const void* dt;                                                         // *sgptDataTables
    uint32_t (*ord)(uint32_t n, uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e);
    void     (*subDE0C0)(void* self, void* unit, uint32_t one, void* buf);  // 0xDE0C0
    void     (*subDCF20)(void* self, void* unit, void* buf);                // 0xDCF20
    uint32_t (*subE03A0)(uint32_t a, uint32_t b);                           // 0xE03A0
    uint32_t (*subC9E0)();                                                  // 0xAC9E0
    void     (*subF6BC0)();                                                 // 0xD6BC0
    void     (*subF1730)(int32_t a, uint32_t b, void* c);                   // 0xF1730
    void     (*subF17E0)(void* unit);                                       // 0xF17E0
    uint32_t (*sub211D0)(const void* p);                                    // D2Game+0x11D0 (64-bit at p)
    uint32_t (*sub21420)();                                                 // 0x1420
    uint32_t (*sub210D0)();                                                 // 0x10D0
    void     (*subBEA30)(int32_t a, uint32_t b);                            // 0x9EA30
    void     (*subBF7A0)();                                                 // 0x9F7A0 (callback address)
    void     (*subAEA80)(uint32_t a, void* b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g); // 0x8EA80
    uint32_t (*subCE4D0)(uint32_t a, void* b, void* c, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4); // 0xAE4D0 (ret 0x14)
    uint32_t (*subB0A20)(void* ebx, void* unit);                           // 0xB0A20: EBX = the searching unit, stack word = the candidate
    uint32_t (*sub41260)(void* unit);                                       // 0x21260
    uint32_t (*subC21320)();                                                // 0x1320
    uint32_t (*sub07A0)(uint32_t a);                                        // 0xE07A0
    void     (*subDDDE0)(uint32_t a, void* b, void* c);                     // 0xDDDE0
    uint32_t (*callPtr)(uint32_t fn, uint32_t a, uint32_t b);               // indirect call through a retail fn pointer
};
