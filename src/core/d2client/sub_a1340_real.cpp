// D2Client sub_A1340 (FUN_6fb51340): real function. The body is d2c_a1340 (sub_a1340.cpp, Env form: every retail callee argument is explicit there,
// and the FN probe drives the same body); this adapter binds the imported ordinals and the 0x2A0B0 / 0xA0B30 callees.
#include <cstdint>
#include <unistd.h>
#define W __attribute__((weak))
struct D2cA1340Env {
    void (*fada0b0)(const uint8_t*); uint32_t (*ord10265)(uint32_t); void (*ord10024)(const void*, uint32_t); const void* g_6fb8359f;
    const void* (*ord10392)(uint32_t); int32_t (*ord10983)(uint32_t); int32_t (*ord10764)(uint32_t); int32_t (*ord10867)(uint32_t); int32_t (*ord10750)(uint32_t);
    void (*f50b30)(const uint8_t*, uint32_t*, uint32_t*); void (*exit_)(int32_t);
};
extern "C" {
uint32_t d2c_a1340(const D2cA1340Env*, const uint8_t*, int32_t*, int32_t*);
W uint32_t Ordinal_10265(uint32_t id);
W void Ordinal_10024(const void* fmt, uint32_t s);
W uint8_t DAT_6fb8359f[1];
W uint32_t* Ordinal_10392(uint32_t unitId);
W int32_t Ordinal_10983(uint32_t unitId);
W int32_t Ordinal_10764(uint32_t unitId);
W intptr_t Ordinal_10867(uintptr_t p);
W intptr_t Ordinal_10750(uintptr_t p);
W void FUN_6fada0b0(const uint8_t* in_EAX, uint32_t unaff_EDI);   // D2Client+0x2A0B0
W void FUN_6fb50b30(const uint8_t* eax_unit, int32_t* outW, int32_t* outH);   // D2Client+0xA0B30 (EAX = unit, stack outW outH)

// 1.13c D2Client+0xA1340
// param_1 = out height; unaff_EBX = object (u32 at +0x2c = unit id); unaff_EDI = out width. Returns 1.
uint32_t FUN_6fb51340(int32_t* param_1, const uint8_t* unaff_EBX, int32_t* unaff_EDI) {
    static const D2cA1340Env e = {
        [](const uint8_t* s) { FUN_6fada0b0(s, 0); }, Ordinal_10265, Ordinal_10024, DAT_6fb8359f,
        [](uint32_t id) -> const void* { return Ordinal_10392(id); }, [](uint32_t id) { return (int32_t)Ordinal_10983(id); },
        [](uint32_t id) { return (int32_t)Ordinal_10764(id); }, [](uint32_t p) { return (int32_t)Ordinal_10867(p); }, [](uint32_t p) { return (int32_t)Ordinal_10750(p); },
        [](const uint8_t* s, uint32_t* w, uint32_t* h) { FUN_6fb50b30(s, (int32_t*)w, (int32_t*)h); }, [](int32_t c) { _exit(c); } };
    return d2c_a1340(&e, unaff_EBX, unaff_EDI, param_1);
}
}
