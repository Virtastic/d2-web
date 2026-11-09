// Env seam of the d2client_batch_a5a30.cpp leaves (shared with the FN probes).
#pragma once
#include <cstdint>
typedef uintptr_t U;
struct D2ClientA5Env {
    void (*fatal)(uint32_t code);                                  // Ordinal_10265+10024+_exit(-1)
    void (*ord_10574)(U esi, uint32_t edi, uint32_t ebp, uint32_t ebx); void (*ord_10032)(U esi, uint32_t edi, uint32_t ebp, uint32_t ebx); void (*ord_10857)(U esi, uint32_t edi);   // imports 0xC622 / 0xC67C (stdcall 4) / 0xC6AC (stdcall 2)
    U (*fn_9d630)(uint32_t a, uint32_t b);                         // 0x9D630
    U (*ord_10460)(U list); U (*ord_10464)(U node); uint32_t (*ord_10298)(U node);
    uint32_t (*fn_a52a0)(U owner);                                 // 0xA52A0
    void (*ord_10061)(U list); uint32_t (*ord_10202)(U unit, uint32_t, uint32_t, const char* file);
    void (*ord_11127)(); uint32_t (*ord_10646)(U list, U unit);
    void (*fn_a6ef0)(U row, uint32_t bucket, uint32_t id, uint32_t type);
    uint32_t (*fn_1510)(uint32_t a, uint32_t b);                   // 0x1510
    U (*ord_10688)(U p);
    U (*fn_a4e20)(uint32_t id, U row, uint32_t type);
    void (*fn_22b20)(U unit);
    int32_t (*ord_11044)(U a, uint32_t b); U stamp_src;            // DAT_6fbcc3b8
    void (*fn_25280)();
    void (*fn_6b300)(U unit, uint32_t a, uint32_t b, int32_t c, uint32_t d);
    void (*fn_88a70)(U unit, uint32_t a, uint32_t b, uint32_t c);
    U (*fn_51a80)();
    void (*fn_a6d60)(U unit);
    void (*fn_c7b00)(); void (*fn_c7e30)(int32_t a, int32_t b, int32_t c);
    void (*fn_53320)();
    U (*ord_10476)(); void (*ord_10561)();
    void (*fn_a7bf0)(uint32_t a, int32_t b, int32_t c, uint32_t d);
    const uint32_t* type_order;                                    // DAT_6fb8412c, 6 entries
};
