// Browser entry for the font table loader: the file image is set up the way 1.13c D2Win+0x12B40 leaves a font slot
// (slot 2 = the file, slot 3 = file + 0xC, the 14-byte glyph records) and every record is answered by the glyph
// lookup D2Win+0x12150 (binary search by code, bounded by the u16 count at +8), the lookup the font getter DAT_6F901508 runs.
#include <cstdint>
#include <emscripten.h>

struct D2WinFontEnt { void* s[5]; };
extern "C" {
extern uint32_t d2win_font_cur;
extern D2WinFontEnt d2win_font_tab[32];
uint8_t* d2win_glyph_12150(uint16_t c);

// tbl/len: the .tbl file. out: 5 bytes per record = code (u16 LE), advance (+3), height (+4), image index (+8).
// Returns the record count written, or -1 when the file is shorter than its header says.
EMSCRIPTEN_KEEPALIVE int32_t d2_win_font_tbl_decode(uint8_t* tbl, int32_t len, uint8_t* out) {
    if (len < 12) return -1;
    const int32_t count = *(uint16_t*)(tbl + 8);
    if (12 + count * 14 > len) return -1;
    d2win_font_cur = 0;
    d2win_font_tab[0].s[2] = tbl;
    d2win_font_tab[0].s[3] = tbl + 12;
    int32_t n = 0;
    for (int32_t i = 0; i < count; ++i) {
        const uint16_t code = *(uint16_t*)(tbl + 12 + i * 14);
        const uint8_t* g = d2win_glyph_12150(code);
        if (*(const uint16_t*)g != code) continue;
        out[n * 5] = (uint8_t)code; out[n * 5 + 1] = (uint8_t)(code >> 8);
        out[n * 5 + 2] = g[3]; out[n * 5 + 3] = g[4]; out[n * 5 + 4] = g[8];
        ++n;
    }
    return n;
}
}
