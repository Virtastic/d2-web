// 1.13c D2Game+0x0398E (208 B): MSVC 2003 ___freetlocinfo. The explicit environment supplies the
// module globals and callees so the host model keeps retail's exact branch and free order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameFreetlocinfoCaptureEnv {
    uint32_t default_locinfo;   // DAT_6fd3000c
    uint32_t lc_time_global;    // DAT_6fd30008
    uint32_t default_lc_time;   // DAT_6fd3a200
    uint32_t default_lconv_mon; // DAT_6fd3a204
    uint32_t default_lconv_num; // DAT_6fd3a208
    void (*free_)(uint32_t);
    void (*free_lconv_mon)(uint32_t);
    void (*free_lconv_num)(uint32_t);
    void (*free_lc_time)(uint32_t);
};

static uint32_t d2game_ftli_read(uint32_t p, uint32_t off = 0) {
    uint32_t value;
    std::memcpy(&value, (const void*)(uintptr_t)(p + off), sizeof value);
    return value;
}

// 1.13c D2Game+0x0398E ___freetlocinfo(locale): free non-default locale tables whose reference
// counts are zero, then free the locale itself. Internal callees remain explicit seams.
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_freetlocinfo_398e(
    uint32_t locale, const D2GameFreetlocinfoCaptureEnv* env) {
    const uint32_t lconv = d2game_ftli_read(locale, 0x3C);
    if (lconv != env->default_locinfo && lconv != 0 && d2game_ftli_read(d2game_ftli_read(locale, 0x2C)) == 0) {
        uint32_t p = d2game_ftli_read(locale, 0x34);
        if (p != 0 && d2game_ftli_read(p) == 0 && p != env->default_lconv_mon) {
            env->free_(p);
            env->free_lconv_mon(lconv);
        }
        p = d2game_ftli_read(locale, 0x30);
        if (p != 0 && d2game_ftli_read(p) == 0 && p != env->default_lconv_num) {
            env->free_(p);
            env->free_lconv_num(lconv);
        }
        env->free_(d2game_ftli_read(locale, 0x2C));
        env->free_(lconv);
    }
    uint32_t p = d2game_ftli_read(locale, 0x40);
    if (p != env->default_lc_time && p != 0 && d2game_ftli_read(p) == 0) {
        env->free_(p);
        env->free_(d2game_ftli_read(locale, 0x44) - 0xFE);
    }
    p = d2game_ftli_read(locale, 0x50);
    if (p != env->lc_time_global && p != 0 && d2game_ftli_read(p, 0xB4) == 0) {
        env->free_lc_time(p);
        env->free_(p);
    }
    env->free_(locale);
}
