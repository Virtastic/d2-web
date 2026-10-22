// D2Game 1.13c batch b59-gam-b. Only sub_C3F40 is a clean stack/fastcall row; the other
// eleven in the slice (C8FD0 CB1F0 CB690 CBA20 CBF50 CE040 CE0D0 CE120 CE150 CE520 CE620)
// take register args (EAX/EBX/ESI) and are skipped.
#include <cstdint>

// 1.13c D2Game+0xC3F40
// param_1 = game/unit-like struct (+0x6d byte index, +0x10f4 list head); param_2 = unit for Ordinal_10920.
uint32_t d2_game_c3f40(uint8_t* param_1, uint32_t param_2,
                       uint8_t* (*ord10920)(uint32_t),
                       int (*ord10174)(uint32_t, int, int),
                       int (*fn2E660)(uint32_t tag)) {
    uint8_t* r = ord10920(param_2);
    uint32_t arg = *(uint32_t*)(r + 0x10 + (uint32_t)param_1[0x6d] * 4);
    if (ord10174(arg, 10, 0) != 0) return 0;
    if (fn2E660(0x2066736d) != 0) return 0;
    if (fn2E660(0x20747368) != 0) return 0;
    uint32_t* head = *(uint32_t**)(param_1 + 0x10f4);
    if (!head) return 0;
    uint32_t* p = (uint32_t*)(uintptr_t)*head;
    if (!p) return 0;
    while (*p != 9) {
        p = (uint32_t*)(uintptr_t)p[0x3d];
        if (!p) return 0;
    }
    *(int*)(uintptr_t)(p[6] + 0x14) += 1;
    return 0;
}
