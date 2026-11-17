// Browser entry for the DC6 frame decode: every frame of a DC6 file is drawn by the verified D2CMP run-length
// blitter (d2cmp_10270, the inline loop, no tables, no clip) into a cleared width x height canvas, once for the
// palette indices and once through an all-0xFF remap for the coverage plane (a literal pixel is opaque even at
// index 0, a skip is not). The pre-scan below only keeps a malformed stream inside its buffers; it decodes nothing.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include <vector>

struct D2Cmp10270Env {
    uint32_t inl;
    uint32_t (*blit)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi, const uint32_t* st, uint32_t nst);
    void (*fatal)(uint32_t code);
};
uint32_t d2cmp_10270(uint32_t fr, uint32_t dst, int32_t x, int32_t lo, int32_t hi, uint32_t pitch, uint32_t skip, uint32_t h,
                     uint32_t t1, uint32_t t2, uint32_t t3, const D2Cmp10270Env* e);

namespace {
std::vector<uint8_t> g_pix;     // per frame: indices (w*h) then coverage (w*h), top row first
std::vector<uint32_t> g_meta;   // per frame: pixel offset
bool g_fatal;
uint8_t g_cover[256];
void fatal(uint32_t) { g_fatal = true; }
uint32_t rd(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
uint32_t addr(const void* p) { return (uint32_t)(uintptr_t)p; }

// The frame's runs stay inside `length` bytes, fill exactly `h` rows, and no row runs past `w`.
bool wellFormed(const uint8_t* p, uint32_t length, uint32_t w, uint32_t h) {
    const uint8_t* end = p + length;
    uint32_t x = 0, rows = 0;
    while (p < end) {
        const uint8_t c = *p++;
        if (c == 0x80) { if (x > w) return false; if (++rows == h) break; x = 0; continue; }
        const uint32_t run = c & 0x7f;
        if (!run || x + run > w) return false;
        if (!(c & 0x80)) { if ((uint32_t)(end - p) < run) return false; p += run; }
        x += run;
    }
    return rows == h && p == end;
}
}

extern "C" {
// Decodes every frame of the DC6 file at `file`; returns the frame count, or -(i+1) for the first frame refused.
EMSCRIPTEN_KEEPALIVE int d2_dc6_decode(const uint8_t* file, uint32_t len) {
    if (len < 24) return -1;
    const uint32_t dirs = rd(file + 16), fpd = rd(file + 20);
    if (!dirs || !fpd || dirs > 16384 || fpd > 16384 || dirs > 16384 / fpd) return -1;
    const uint32_t count = dirs * fpd;
    if ((uint64_t)24 + count * 4ull > len) return -1;
    if (!g_cover[0]) memset(g_cover, 0xff, sizeof g_cover);
    const D2Cmp10270Env env{1, nullptr, fatal};
    g_meta.assign(count, 0);
    size_t total = 0;
    for (uint32_t i = 0; i < count; i++) {
        const uint32_t at = rd(file + 24 + i * 4);
        if (at > len || len - at < 32) return -(int)(i + 1);
        const uint8_t* fr = file + at;
        const uint32_t w = rd(fr + 4), h = rd(fr + 8), length = rd(fr + 28);
        if (!w || !h || w > 16384 || h > 16384 || (uint64_t)w * h > 268435456ull) return -(int)(i + 1);
        if (len - at - 32 < (uint64_t)length + 3 || !wellFormed(fr + 32, length, w, h)) return -(int)(i + 1);
        g_meta[i] = (uint32_t)total;
        total += (size_t)w * h * 2;
    }
    g_pix.assign(total, 0);
    g_fatal = false;
    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* fr = file + rd(file + 24 + i * 4);
        const uint32_t w = rd(fr + 4), h = rd(fr + 8);
        uint8_t* img = g_pix.data() + g_meta[i];
        // Bit 0 of the flags marks a top-down frame; otherwise the first row of runs is the bottom row (negated pitch).
        const uint32_t row0 = (rd(fr) & 1) ? 0 : (h - 1) * w;
        d2cmp_10270(addr(fr), addr(img + row0), 0, 0, 0, w, 0, h, 0, 0, 0, &env);
        d2cmp_10270(addr(fr), addr(img + (size_t)w * h + row0), 0, 0, 0, w, 0, h, addr(g_cover), 0, 0, &env);
        if (g_fatal) return -(int)(i + 1);
    }
    return (int)count;
}
EMSCRIPTEN_KEEPALIVE const uint32_t* d2_dc6_frame_offsets() { return g_meta.data(); }
EMSCRIPTEN_KEEPALIVE const uint8_t* d2_dc6_pixels() { return g_pix.data(); }
}
