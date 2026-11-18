// 1.13c D2CMP+0x0BEA0 (sub_BEA0, ordinal 10092): walk the cache list, count entries whose per-entry call returns 0.
// ours: src/core/d2cmp/d2cmp_sub_bea0.cpp:d2cmp_sub_bea0
#include <cstdint>

struct D2CmpBea0Env {
    uint8_t* head;                       // DAT_6fe7725c
    int32_t (*entryCall)(uint8_t* ent);  // Ordinal_10092 call at 0xBEA0+0x38 (thunk 0x28D0); entry in implicit reg
    void (*fatal)();                     // Fog error 0xba5 + _exit(-1)
};

int32_t d2cmp_sub_bea0(D2CmpBea0Env& e) {
    int32_t n = 0;
    for (int32_t* p = (int32_t*)e.head; p; p = (int32_t*)(intptr_t)p[5]) {
        uint8_t* ent = (uint8_t*)(intptr_t)*p;
        if (ent && !(ent[0x20] & 1)) {
            if (*(int32_t*)(ent + 0xb0) == 0) { e.fatal(); return n; }
            if (e.entryCall(ent) == 0) n++;
        }
    }
    return n;
}
