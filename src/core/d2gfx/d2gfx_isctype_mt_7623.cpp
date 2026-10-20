#include <cstdint>
#include <emscripten.h>

// Slow-path callee: D2Gfx+0x06988 ___crtGetStringTypeA(type=1, src, len, out, codepage, lcid, err=1).
typedef int32_t (*D2GfxIsctypeStringType)(void* ctx, uint32_t type, const uint8_t* src, int32_t len,
                                          uint16_t* out, uint32_t codepage, int32_t lcid, int32_t err);

// 1.13c D2Gfx+0x07623 (119 B), ___isctype_mt(this, locale, c, mask) (__thiscall; `this` unused).
// c in [-1,0x100]: direct lookup in the locale's 16-bit ctype table (token at locale+0x48, resolved by
// the caller as `table`). Otherwise classify one byte, or two (low byte first) when the high byte's
// table entry carries the lead bit 0x80, through ___crtGetStringTypeA; 0 on its failure.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2gfx_isctype_mt_7623(
        const uint8_t* locale, const uint16_t* table, int32_t c, uint32_t mask,
        D2GfxIsctypeStringType crt_get_string_type_a, void* ctx) {
    uint16_t type;
    if (uint32_t(c) + 1u < 0x101u) {
        type = table[c];
    } else {
        uint8_t src[2] = {uint8_t(c), 0};
        int32_t len = 1;
        if (reinterpret_cast<const uint8_t*>(table)[1 + ((uint32_t(c) >> 8) & 0xFF) * 2] & 0x80) {
            src[1] = uint8_t(uint32_t(c) >> 8);
            len = 2;
        }
        type = 0;
        if (!crt_get_string_type_a(ctx, 1, src, len, &type, *reinterpret_cast<const uint16_t*>(locale + 4),
                                   *reinterpret_cast<const int32_t*>(locale + 0x14), 1))
            return 0;
    }
    return type & mask;
}
