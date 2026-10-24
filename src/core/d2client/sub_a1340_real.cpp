// D2Client sub_A1340 (FUN_6fb51340): real function; the Env-struct form in sub_a1340.cpp stays for old callers.
#include <cstdint>
#include <unistd.h>
#define W __attribute__((weak))
extern "C" {
W uint32_t Ordinal_10265(uint32_t id);
W void Ordinal_10024(const void* fmt, uint32_t s);
W uint8_t DAT_6fb8359f[1];
W uint32_t* Ordinal_10392(uint32_t unitId);
W int32_t Ordinal_10983(uint32_t unitId);
W int32_t Ordinal_10764(uint32_t unitId);
W intptr_t Ordinal_10867(uintptr_t p);
W intptr_t Ordinal_10750(uintptr_t p);
W void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);   // D2Client+0x2A0B0
W void FUN_6fb50b30(int32_t* outW);                                // D2Client+0xA0B30 (EAX/stack args hidden)

// 1.13c D2Client+0xA1340
// param_1 = out height; unaff_EBX = object (u32 at +0x2c = unit id); unaff_EDI = out width. Returns 1.
// The 2A0B0 prologue's hidden EAX/EDI are unknown here: passed through as (EBX, EDI).
uint32_t FUN_6fb51340(int32_t* param_1, const uint8_t* unaff_EBX, int32_t* unaff_EDI) {
    FUN_6fada0b0(unaff_EBX, (uint32_t)(uintptr_t)unaff_EDI);
    uint32_t id = *(const uint32_t*)(unaff_EBX + 0x2c);
    if (!id) { Ordinal_10024(DAT_6fb8359f, Ordinal_10265(0x946)); _exit(-1); }
    const uint32_t* u = Ordinal_10392(id);
    int32_t h;
    if (!u) { *unaff_EDI = Ordinal_10983(id); h = Ordinal_10764(id); }
    else {
        int32_t t = (int32_t)u[0]; uint32_t p = u[0xb];
        bool direct = t == 2 || (t > 3 && t < 6);
        *unaff_EDI = direct ? *(const int32_t*)(uintptr_t)(p + 0xc) : p ? Ordinal_10867(p) : 0;
        h = direct ? *(const int32_t*)(uintptr_t)(p + 0x10) : p ? Ordinal_10750(p) : 0;
    }
    *param_1 = h;
    if (!*unaff_EDI || !*param_1) FUN_6fb50b30(unaff_EDI);
    return 1;
}
}
