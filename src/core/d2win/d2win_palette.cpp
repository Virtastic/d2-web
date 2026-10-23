// 1.13c D2Win+0xF2D0 (sub_F2D0, ordinal 10065): load act 1..5's palette
// bank -- the transform tables of DATA\GLOBAL\palette\act<n>\pal.pl2 -- into the D2Win globals and
// hand the rgba palette to the renderer.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include "../tables/shared.h"

namespace {

// The banks of D2PL2FileStrc, sliced the way D2Win+0xEF20 copies them. They are named after what
// retail keeps at each destination; the addresses in the spec are their identity (the seam below is
// keyed by them), our own buffers of course live elsewhere.
uint8_t g_rgba[0x400];          // 0x6F922338  rgbaPalette[256], alpha forced to 5
uint8_t g_slg[0x3100];          // 0x6F932A38  shadows[32][256], light[16][256] at +0x2000, gamma[256] at +0x3000
uint8_t g_trans[0x30000];       // 0x6F93DB60  trans[3][256][256]
uint8_t g_luminance[0x10000];   // 0x6F911630
uint8_t g_screen[0x10000];      // 0x6F901630
uint8_t g_states[0x8000];       // 0x6F935B60  states[128][256]
uint8_t g_darkBlend[0x10000];   // 0x6F922738
uint8_t g_darken[0x100];        // 0x6F932838  darkenPalette[256]
uint8_t g_stdColors[0x28];      // 0x6F935B38  standardColors[13][3]; retail copies 0x27 bytes, the last byte stays as it was
uint8_t g_stdShifts[0xD00];     // 0x6F921638  standardShifts[13][256]
uint8_t g_nearScaled[0x100];    // 0x6F932738  nearest index at 170% brightness
uint8_t g_nearRed[0x100];       // 0x6F932938  nearest index for the red channel alone

const uint32_t kRgbaVA = 0x6F922338;   // the bank D2Gfx #10061 is handed

// The renderer seam. Retail reaches it through two D2Gfx imports: #10022 (the palette pointer table,
// built by +0xEE60) and #10061 (SetPalette, from +0xEF20 and from +0xF2D0 itself). The vector records
// the count and the last argument of one call to +0xF2D0, so the counter is per load. Our renderer
// takes the banks directly, so the 0x120-byte table of +0xEE60 has no consumer here and is not built.
uint32_t g_seamCalls, g_seamLast;
void palette_seam(uint32_t bank) { ++g_seamCalls; g_seamLast = bank; }

// D2CMP+0x9D30 (ordinal 10049, GetNearestPaletteIndex), a local copy: the summed squared distance
// (pal[0]-a0)^2 + (pal[1]-a1)^2 + (pal[2]-a2)^2 in 32-bit wrap, strict `<` so the first of equal
// minima wins; the low byte of the winner, 0 for an empty palette.
uint32_t nearest_index(const uint8_t* pal, uint32_t n, uint32_t a0, uint32_t a1, uint32_t a2) {
    if (!n) return 0;
    uint32_t best = 0xFFFFFFFFu, idx = 0;
    for (uint32_t i = 0; i < n; ++i) {
        const uint8_t* e = pal + i * 4;
        const uint32_t d = (e[0] - a0) * (e[0] - a0) + (e[1] - a1) * (e[1] - a1) + (e[2] - a2) * (e[2] - a2);
        if (d < best) { best = d; idx = i; }
    }
    return (n & 0xFFFFFF00u) | (idx & 0xFFu);
}

// 1.13c D2Win+0x0ef20 (sub_EF20): this whole bank load. Retail also
// opens pal.dat and reads it into a 0xCF*4 buffer, both unused, and prints a message (Fog #10265)
// then _exit(-1) when a file is missing -- not modelled, a miss is a 0 return.
int palette_banks_load(const char* pl2) {
    std::vector<uint8_t> f;
    if (!d2::sharedVfs().read(pl2, f) || f.size() < 0x6C327) return 0;
    const uint8_t* p = f.data();
    memcpy(g_rgba, p + 0x00000, 0x400);
    memcpy(g_slg, p + 0x00400, 0x3100);
    memcpy(g_trans, p + 0x03500, 0x30000);
    memcpy(g_screen, p + 0x33500, 0x10000);
    memcpy(g_luminance, p + 0x43500, 0x10000);
    memcpy(g_states, p + 0x53500, 0x8000);
    memcpy(g_darkBlend, p + 0x5B500, 0x10000);
    memcpy(g_darken, p + 0x6B500, 0x100);
    memcpy(g_stdColors, p + 0x6B600, 0x27);
    memcpy(g_stdShifts, p + 0x6B627, 0xD00);
    for (uint32_t i = 0; i < 0x100; ++i) g_rgba[i * 4 + 3] = 5;
    for (uint32_t i = 0; i < 0x100; ++i) {
        const uint32_t c0 = g_rgba[i * 4], c1 = g_rgba[i * 4 + 1], c2 = g_rgba[i * 4 + 2];
        const uint32_t s0 = c0 * 0xAA / 100, s1 = c1 * 0xAA / 100, s2 = c2 * 0xAA / 100;
        g_nearScaled[i] = (uint8_t)nearest_index(g_rgba, 0x100, s0 < 0x100 ? s0 : 0xFF, s1 < 0x100 ? s1 : 0xFF, s2 < 0x100 ? s2 : 0xFF);
        g_nearRed[i] = (uint8_t)nearest_index(g_rgba, 0x100, c0, 0, 0);
    }
    palette_seam(0);           // +0xEE60's palette-table call (its argument is a stack table retail never exposes)
    palette_seam(kRgbaVA);     // +0xEF20's SetPalette(rgba)
    return 1;
}

}  // namespace

// 1.13c D2Win+0xF2D0 (ordinal 10065). `act` is masked to a byte and used as `(act & 0xff) + 1`;
// outside 1..5 that is 1. The pal.dat path is formatted and never opened.
extern "C" int d2win_LoadActPalette(uint32_t act) {
    uint32_t n = (act & 0xFFu) + 1;
    if (n == 0 || n > 5) n = 1;
    char dat[260], pl2[260];
    snprintf(dat, sizeof dat, "%s\\palette\\act%u\\%s", "DATA\\GLOBAL", (unsigned)n, "pal.dat");
    snprintf(pl2, sizeof pl2, "%s\\palette\\act%u\\%s", "DATA\\GLOBAL", (unsigned)n, "pal.pl2");
    (void)dat;
    g_seamCalls = 0;
    const int r = palette_banks_load(pl2);
    palette_seam(kRgbaVA);
    return r;
}

// The probe's read-back: the fourteen banks the vector hashes, in the capture's order (the a dozen
// copy destinations, with light and gamma as banks of their own, then the two nearest tables).
extern "C" const uint8_t* d2win_palette_block(uint32_t k, uint32_t* len) {
    static const struct { const uint8_t* p; uint32_t n; } blk[14] = {
        { g_rgba, 0x400 }, { g_trans, 0x30000 }, { g_slg, 0x3100 }, { g_luminance, 0x10000 },
        { g_screen, 0x10000 }, { g_slg + 0x2000, 0x1000 }, { g_slg + 0x3000, 0x100 }, { g_states, 0x8000 },
        { g_darkBlend, 0x10000 }, { g_darken, 0x100 }, { g_stdColors, 0x28 }, { g_stdShifts, 0xD00 },
        { g_nearScaled, 0x100 }, { g_nearRed, 0x100 },
    };
    if (k < 1 || k > 14) return nullptr;
    *len = blk[k - 1].n;
    return blk[k - 1].p;
}
extern "C" uint32_t d2win_palette_seam_calls() { return g_seamCalls; }
extern "C" uint32_t d2win_palette_seam_last() { return g_seamLast; }
