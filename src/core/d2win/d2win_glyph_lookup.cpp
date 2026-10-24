// Glyph record lookups behind DAT_6f901508 (set by D2Win+0x13110). Char arrives in CX (fastcall).
// Font table DAT_6f901510: 5-slot entries indexed by DAT_6f8ffd34; slot 2 -> header (u16 count at +8),
// slot 3 -> glyph array of 14-byte records (u16 code first).
#include <cstdint>
struct D2WinFontEnt { void* s[5]; };
extern "C" {
uint32_t d2win_font_cur;        // DAT_6f8ffd34
D2WinFontEnt d2win_font_tab[32];  // DAT_6f901510 (retail stride 20 bytes; slots are pointers here)

// 1.13c D2Win+0x12150: binary search by code (max 1001 probes), fallback record #31
uint8_t* d2win_glyph_12150(uint16_t c) {
  const D2WinFontEnt& e = d2win_font_tab[d2win_font_cur];
  uint8_t* g = (uint8_t*)e.s[3];
  int32_t lo = 0, hi = (int32_t)*(uint16_t*)((uint8_t*)e.s[2] + 8) - 1, n = 0;
  if (hi >= 0) for (;;) {
    int32_t mid = (lo + hi) >> 1;
    uint16_t code = *(uint16_t*)(g + mid * 14);
    ++n;
    if (c == code) return g + mid * 14;
    if (c < code) hi = mid - 1; else lo = mid + 1;
    if (n > 1000 || hi < lo) break;
  }
  return g + 0x1b2;
}

// 1.13c D2Win+0x121D0: chars <=0xFF index the array directly; others get record 0
uint8_t* d2win_glyph_121d0(uint16_t c) {
  const D2WinFontEnt& e = d2win_font_tab[d2win_font_cur];
  uint8_t* g = (uint8_t*)e.s[3];
  return c > 0xff ? g : g + (uint32_t)c * 14;
}
}
