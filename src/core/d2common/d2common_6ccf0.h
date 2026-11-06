// D2Common 1.13c 0x6CCF0 (automap.txt compile): the module words it writes and the seam its callees go through.
#pragma once
#include <cstdint>

struct D2Common6CCF0State {
    uint32_t seedLo, seedHi;          // module 0x9FDB0: the automap seed, set to {0, 0x29A}
    int32_t rand[36][2];              // module 0x9FDB8: per automap level type {first record, one past last}, -1 when unused
    uint32_t pAutoMap, nAutoMap;      // module 0xA1454, 0xA1458
    uint32_t flag;                    // module 0x99E20
};
struct D2Common6CCF0Env {
    uint32_t (*load)(uint32_t arch, uint32_t* count);          // 0x5EF40("automap", 10 fields, record size 0x2C): rows
    uint32_t (*alloc)(uint32_t size);                          // Fog 0x9222 (edx = size; LvlTbls.cpp:0x508)
    void (*freeFn)(uint32_t block, uint32_t line);             // Fog 0x9228 (block already adjusted by the flag)
    int32_t (*levelIdx)(uint32_t name);                        // 0x6C300 (eax = LevelName string)
    int32_t (*tileIdx)(uint32_t name);                         // 0x6C280 (eax = TileName string)
    void (*fatal)(uint32_t code);                              // 0x51E, 0x539: retail logs and _exit(-1)
};
extern "C" void d2_seam_d2common_6ccf0(uint32_t arch, const D2Common6CCF0Env* e, D2Common6CCF0State* s);
