// 1.13c D2Gfx+0x051CD: VC7.1 CRT ___freetlocinfo. Retail globals and callees are explicit seams.
#include <cstdint>

struct D2GfxFreeTlocinfoEnv {
    void (*free_)(uint32_t token);                 // D2Gfx+0x02BD4
    void (*free_lconv_mon)(uint32_t lconv);        // D2Gfx+0x0670C
    void (*free_lconv_num)(uint32_t lconv);        // D2Gfx+0x066AD
    void (*free_lc_time)(uint32_t time);           // D2Gfx+0x0651D
    uint32_t default_lconv;                       // DAT_6fa91100, RVA 0x11100
    uint32_t static_ctype;                        // DAT_6fa9d6a0, RVA 0x1D6A0
    uint32_t static_mon;                          // DAT_6fa9d6a4, RVA 0x1D6A4
    uint32_t static_num;                          // DAT_6fa9d6a8, RVA 0x1D6A8
    uint32_t static_lc_time;                      // DAT_6fa910fc, RVA 0x110FC
};

// The retail record and every pointer stored in it are four-byte x86 values, also in wasm64.
extern "C" void d2gfx_freetlocinfo(uint8_t* locale, const D2GfxFreeTlocinfoEnv* env) {
    const uint32_t lconv = *(uint32_t*)(locale + 0x3C);
    const uint32_t collate = *(uint32_t*)(locale + 0x2C);
    if (lconv != env->default_lconv && lconv != 0 &&
        *(uint32_t*)(uintptr_t)collate == 0) {
        const uint32_t monetary = *(uint32_t*)(locale + 0x34);
        if (monetary != 0 && *(uint32_t*)(uintptr_t)monetary == 0 &&
            monetary != env->static_mon) {
            env->free_(monetary);
            env->free_lconv_mon(lconv);
        }
        const uint32_t numeric = *(uint32_t*)(locale + 0x30);
        if (numeric != 0 && *(uint32_t*)(uintptr_t)numeric == 0 &&
            numeric != env->static_num) {
            env->free_(numeric);
            env->free_lconv_num(lconv);
        }
        env->free_(collate);
        env->free_(lconv);
    }

    const uint32_t ctype = *(uint32_t*)(locale + 0x40);
    if (ctype != env->static_ctype && ctype != 0 &&
        *(uint32_t*)(uintptr_t)ctype == 0) {
        env->free_(ctype);
        env->free_(*(uint32_t*)(locale + 0x44) - 0xFEu);
    }

    const uint32_t time = *(uint32_t*)(locale + 0x50);
    if (time != env->static_lc_time && time != 0 &&
        *(uint32_t*)(uintptr_t)(time + 0xB4) == 0) {
        env->free_lc_time(time);
        env->free_(time);
    }
    env->free_((uint32_t)(uintptr_t)locale);
}
