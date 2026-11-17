// Browser entry for the font width sum: runs the 1.13c D2Win+0x12080 loop (d2win_12080) over a UTF-16 string, with the
// glyph getter answered from a per-font advance table (the .tbl's width byte per code unit; byte +3 of the glyph record).
#include <cstdint>
#include <emscripten.h>

extern "C" int32_t d2win_12080(const uint16_t* text, int32_t n, const void* env);
extern "C" int32_t d2win_120c0(const uint16_t* text, int32_t n, const void* env);

namespace {
const uint8_t* g_adv;
const uint8_t* glyph(uint32_t ch) {
    static uint8_t rec[4];
    rec[3] = g_adv[ch & 0xffff];
    return rec;
}
}  // namespace

// adv: 65536 bytes, text: n UTF-16 units.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_win_text_width(const uint8_t* adv, const uint16_t* text, int32_t n) {
    struct Env { const uint8_t* (*glyph)(uint32_t); } e = { glyph };
    g_adv = adv;
    return d2win_12080(text, n, &e);
}

// Same table, but the 1.13c D2Win+0x120C0 loop: a 0xFF 'c' [0-6] colour code is skipped while more than 3 chars remain.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_win_text_width_cc(const uint8_t* adv, const uint16_t* text, int32_t n) {
    struct Env { const uint8_t* (*glyph)(uint32_t); } e = { glyph };
    g_adv = adv;
    return d2win_120c0(text, n, &e);
}
