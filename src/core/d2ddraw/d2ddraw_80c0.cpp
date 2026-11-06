// src/core/d2ddraw/d2ddraw_80c0.cpp -- 1.13c D2DDraw.dll palette set (copy, gamma remap, mark dirty), from the retail
// disassembly. The gamma ramp comes from Fog ordinal 10198 (fastcall); its double argument is built here from the integer
// global by an exact integer-to-IEEE conversion (retail: fild + fstp), so no float arithmetic is involved.
#include <cstdint>
#include <cstring>

struct D2DDrawSetPaletteEnv {
    uint8_t* copy;              // DAT 0xF8B0: the 256 entries as handed in (0x400 bytes)
    uint8_t* remapped;          // DAT 0xFDC8: the entries pushed to the surface palette (byte 3 of each entry left as it was)
    const int32_t* gamma;       // DAT 0xEB90 (an int, widened to a double for Fog)
    const uint32_t* contrast;   // DAT 0xEB94 (ECX of the Fog call)
    uint32_t* dirty;            // DAT 0x10240
    // Fog 10198: fastcall(ECX contrast, EDX the 256-byte table, stack: gamma as a double (lo, hi), 1, 0x100, 0xFF), callee pops 5
    void (*gammaRamp)(uint32_t contrast, uint8_t* table, uint32_t lo, uint32_t hi, uint32_t a, uint32_t n, uint32_t max);
};

// The bit pattern of (double)v, exact for every int32.
static uint64_t d2ddraw_int_to_double_bits(int32_t v) {
    if (v == 0) return 0;
    const uint64_t sign = v < 0 ? 1ull << 63 : 0;
    uint64_t m = v < 0 ? (uint64_t)(-(int64_t)v) : (uint64_t)v;
    int msb = 63; while (!(m >> msb)) --msb;
    return sign | (uint64_t)(1023 + msb) << 52 | ((m << (52 - msb)) & ((1ull << 52) - 1));
}

// 1.13c D2DDraw+0x80C0 (ECX = 256 RGBX entries). Copies the entries to 0xF8B0, has Fog build a 256-byte gamma table, writes
// table[c] for bytes 0..2 of each copied entry into 0xFDC8 (byte 3 untouched) and sets the palette-dirty word to 1.
extern "C" void d2ddraw_set_palette_80c0(const D2DDrawSetPaletteEnv* e, const uint8_t* entries) {
    uint8_t table[256];
    memcpy(e->copy, entries, 0x400);
    const uint64_t g = d2ddraw_int_to_double_bits(*e->gamma);
    e->gammaRamp(*e->contrast, table, (uint32_t)g, (uint32_t)(g >> 32), 1, 0x100, 0xFF);
    for (uint32_t i = 0; i < 0x400; i += 4) {
        e->remapped[i] = table[e->copy[i]];
        e->remapped[i + 1] = table[e->copy[i + 1]];
        e->remapped[i + 2] = table[e->copy[i + 2]];
    }
    *e->dirty = 1;
}
