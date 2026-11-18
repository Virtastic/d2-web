// 1.13c D2CMP+0xA2E0 (export #10040, 425 bytes): select the reduction palette (0x9A10's source).
#include "d2cmp_palstate.h"
uint8_t  g_d2cmp_raw_6fe872e8[0x300];  // 0x6FE872E8
uint32_t g_d2cmp_loaded_6fe88318;      // 0x6FE88318
// 1.13c D2CMP+0x0A2E0
// Fatal (Fog #10265 code, #10024, _exit) on name == NULL (0x3B6), hi < lo (0x3B7), lo < 0 (0x3B8),
// hi >= 256 (0x3B9). Then palStart = 1 (lo is only validated), palEnd = hi, squares table rebuilt,
// lastColor = 0. Empty name: identity grey palette, loaded = 1, return 1. Otherwise open the file
// (failure: debug "Unable to open <name>\n", return 0 with loaded unchanged); a 768-byte file is read
// raw into 0x6FE872E8 and loaded = 0 (returned); any other size is a PCX for 0x9B00, whose result
// becomes loaded and the return.
extern "C" uint32_t d2cmp_a2e0(const D2CmpPalEnv* env, const char* name, int32_t lo, int32_t hi) {
    if (name == nullptr) env->fatal(0x3b6);
    else if (hi < lo) env->fatal(0x3b7);
    else if (lo < 0) env->fatal(0x3b8);
    else if (hi >= 0x100) env->fatal(0x3b9);
    g_d2cmp_palStart_6fe87ef0 = 1;
    g_d2cmp_palEnd_6fe875ec = (uint32_t)hi;
    d2cmp_9c90();
    g_d2cmp_lastColor_6fe8831c = 0;
    if (env->strLen(name) == 0) {
        for (uint32_t i = 0; i < 0x100; ++i)
            g_d2cmp_pal_6fe87f18[i * 4] = g_d2cmp_pal_6fe87f18[i * 4 + 1] = g_d2cmp_pal_6fe87f18[i * 4 + 2] = (uint8_t)i;
        g_d2cmp_loaded_6fe88318 = 1;
        return 1;
    }
    intptr_t h = env->open(name);
    if (h == -1) { env->debug("Unable to open "); env->debug(name); env->debug("\n"); return 0; }
    if (env->size(h) == 0x300) {
        // A full read converts the BGR triples to the 4-byte palette entries (alpha byte untouched), loaded = 1.
        uint32_t got = env->read(h, g_d2cmp_raw_6fe872e8, 0x300);
        if (got == 0x300) {
            for (uint32_t i = 0; i < 0x100; ++i) {
                g_d2cmp_pal_6fe87f18[i * 4 + 2] = g_d2cmp_raw_6fe872e8[i * 3];
                g_d2cmp_pal_6fe87f18[i * 4 + 1] = g_d2cmp_raw_6fe872e8[i * 3 + 1];
                g_d2cmp_pal_6fe87f18[i * 4] = g_d2cmp_raw_6fe872e8[i * 3 + 2];
            }
            g_d2cmp_loaded_6fe88318 = 1;
        } else {
            g_d2cmp_loaded_6fe88318 = 0;
        }
        env->close(h);
        return g_d2cmp_loaded_6fe88318;
    }
    env->close(h);
    g_d2cmp_loaded_6fe88318 = d2cmp_9b00(env, name);
    return g_d2cmp_loaded_6fe88318;
}
