// src/core/d2cmp/d2cmp_10270.cpp -- 1.13c D2CMP.dll sub_10270 transcribed from the cached Ghidra decompile
// (build-wasm/ghidra-d2cmp/decomp/0x10270.c). Prior art: none (D2MOO's D2CMP is stubs).
// The 14 specialised row blitters (0xE480..0xFB50) and the Fog error-string path are seams in the env.
#include <cstdint>
#include <cstdlib>

typedef void (*D2CMP10270Fn6)(uint8_t* p, int h, int pitch, int x, int lo, int hi);
typedef void (*D2CMP10270Fn3)(uint8_t* p, int h, int pitch);

struct D2CMP10270Env {
    int generic;  // DAT_6fe87efc: nonzero selects the inline per-run loop
    D2CMP10270Fn6 f2c0, f620, efd0, e860, eb50, e480;
    D2CMP10270Fn3 f8a0, f4b0, f190, ea20, edd0, e6b0;
    void (*faa0)(uint8_t* p, uint8_t* dst, int h, int pitch, int lo, int hi);
    void (*fb50)(uint8_t* p, uint8_t* dst, int h, int pitch);
};

// 1.13c D2CMP+0x10270 (1171 bytes): blit rows of a DC6-style frame (hdr: flags +0, width +4, end +8,
// data +0x20). Skips `skip` rows of runs, then draws `h` rows. t1 = palette-shift (colour map) table,
// t2 = blend table (indexed [dst*256+src']), t3 = remap table; lo/hi = x clip. Pre-checks are fatal.
// 1.13c D2CMP+0x10270
extern "C"
void d2cmp_10270(const uint8_t* fr, uint8_t* dst, int x, int lo, int hi, uint32_t pitch,
                 uint8_t* skipRows, int h, const uint8_t* t1, const uint8_t* t2, const uint8_t* t3,
                 const D2CMP10270Env* env) {
    uint32_t width = *(const uint32_t*)(fr + 4);
    if (pitch < width) _Exit(-1);
    if (*(const uint8_t* const*)(fr + 8) < skipRows + h) _Exit(-1);
    const uint8_t* p = fr + 0x20;
    for (uintptr_t n = (uintptr_t)skipRows; n; --n) {
        uint8_t b;
        do {
            for (;;) { b = *p++; if ((int8_t)b < 0) break; p += (int8_t)b; }
        } while ((b & 0x7f) != 0);
    }
    if ((fr[0] & 1) == 0) pitch = -pitch;
    int ipitch = (int)pitch;
    if (env->generic) {
        int x0 = x;
        uint8_t* rowNext = dst + ipitch;
        do {
            for (;;) {
                uint8_t b = *p++;
                if ((int8_t)b >= 0) {
                    uint32_t cnt = (uint32_t)(int8_t)b;
                    const uint8_t* src = p;
                    p += cnt;
                    uint8_t* after = dst + cnt;
                    uint32_t u = cnt;
                    if (lo != 0 || hi != 0) {
                        int a = lo - x; if (a < 1) a = 0;
                        int c = hi - x; if (c < 1) c = 0;
                        if ((int)cnt <= c) c = (int)cnt;
                        u = (uint32_t)(c - a);
                        if ((int)u < 1) u = 0; else { dst += a; src += a; }
                    }
                    const uint8_t* end = src + u;
                    for (; src < end; ++src, ++dst) {
                        if (!t2) {
                            if (!t3) *dst = t1 ? t1[*src + *dst * 0x100] : *src;
                            else *dst = t1 ? t1[*dst * 0x100 + t3[*src]] : t3[*src];
                        } else if (!t1) *dst = t3 ? t2[t3[*src]] : t2[*src];
                        else *dst = t1[*dst * 0x100 + t2[*src]];
                    }
                    x += cnt;
                    dst = after;
                } else {
                    b &= 0x7f;
                    if (b == 0) break;
                    x += b; dst += b;
                }
            }
            --h;
            dst = rowNext; x = x0; rowNext += ipitch;
        } while (h != 0);
        return;
    }
    bool inside = (lo == 0 && hi == 0) || (lo <= x && (int)(width + x) < hi);
    if (t2) {
        if (t1) { inside ? env->f8a0((uint8_t*)p, h, pitch) : env->f620((uint8_t*)p, h, pitch, x, lo, hi); return; }
        if (t3) { inside ? env->f4b0((uint8_t*)p, h, pitch) : env->f2c0((uint8_t*)p, h, pitch, x, lo, hi); return; }
        inside ? env->f190((uint8_t*)p, h, pitch) : env->efd0((uint8_t*)p, h, pitch, x, lo, hi); return;
    }
    if (t3) {
        if (!t1) { inside ? env->ea20((uint8_t*)p, h, pitch) : env->e860((uint8_t*)p, h, pitch, x, lo, hi); return; }
        inside ? env->edd0((uint8_t*)p, h, pitch) : env->eb50((uint8_t*)p, h, pitch, x, lo, hi); return;
    }
    if (!t1) { inside ? env->fb50((uint8_t*)p, dst, h, pitch) : env->faa0((uint8_t*)p, dst, h, pitch, lo, hi); return; }
    inside ? env->e6b0((uint8_t*)p, h, pitch) : env->e480((uint8_t*)p, h, pitch, x, lo, hi);
}
