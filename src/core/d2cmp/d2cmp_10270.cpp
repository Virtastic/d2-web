// D2CMP run-length row blitter (DC6-style frame). Retail: fastcall ECX = frame, EDX = dst, stack (x, lo, hi, pitch, skip, h, t1, t2, t3),
// ret 0x24. Frame: +0 flags (bit 0 clear flips the pitch), +4 width, +8 row limit, +0x20 run data (a byte b >= 0 is a literal run of b
// source bytes, b = 0x80 ends a row, any other b < 0 skips b & 0x7f pixels). Frame, dst and tables are 32-bit retail addresses (flat guest
// image). DAT 0x77EFC selects the inline per-pixel loop; when it is 0 one of 14 specialised blitters (stdcall, registers below) draws.
// t1 and t3 are 256-byte remaps of the source byte, t2 a 64 KiB blend table indexed [dst * 256 + source'].
#include <cstdint>

struct D2Cmp10270Env {
    uint32_t inl;                                                                         // DAT 0x77EFC
    // a specialised blitter at rva: registers at the call and its stack words (stdcall, it pops nst)
    void (*blit)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi, const uint32_t* st, uint32_t nst);
    void (*fatal)(uint32_t code);                                                         // Fog 10265 / 10024 / _exit(-1)
};

static inline uint8_t rb(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline void wb(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
static inline uint32_t rd(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2CMP+0x10270
void d2cmp_10270(uint32_t fr, uint32_t dst, int32_t x, int32_t lo, int32_t hi, uint32_t pitch, uint32_t skip, uint32_t h,
                 uint32_t t1, uint32_t t2, uint32_t t3, const D2Cmp10270Env* e)
{
    const uint32_t width = rd(fr + 4);
    if (pitch < width) { e->fatal(0x5ea); return; }
    if (skip + h > rd(fr + 8)) { e->fatal(0x5eb); return; }
    uint32_t p = fr + 0x20;
    for (uint32_t n = skip; n; --n) {
        for (;;) {
            const int8_t b = (int8_t)rb(p++);
            if (b >= 0) { p += (uint32_t)(int32_t)b; continue; }
            if (!(b & 0x7f)) break;
        }
    }
    if (!(rb(fr) & 1)) pitch = 0u - pitch;
    if (e->inl) {
        const int32_t x0 = x;
        uint32_t next = dst + pitch;
        do {
            for (;;) {
                const int8_t b = (int8_t)rb(p++);
                if (b < 0) {
                    const int32_t s = b & 0x7f;
                    if (!s) break;
                    x += s; dst += (uint32_t)s;
                    continue;
                }
                const int32_t cnt = b;
                uint32_t src = p, d = dst;
                const uint32_t pn = p + (uint32_t)cnt, dn = dst + (uint32_t)cnt;
                int32_t u = cnt;
                if (lo || hi) {
                    int32_t a = lo - x; if (a <= 0) a = 0;
                    int32_t c = hi - x; if (c <= 0) c = 0;
                    if (c >= cnt) c = cnt;
                    u = c - a;
                    if (u <= 0) u = 0; else { d += (uint32_t)a; src += (uint32_t)a; }
                }
                for (const uint32_t end = src + (uint32_t)u; src < end; ++src, ++d) {
                    const uint32_t v = rb(src);
                    uint8_t o;
                    if (!t1) {
                        if (!t3) o = t2 ? rb(t2 + v + rb(d) * 0x100u) : (uint8_t)v;
                        else o = t2 ? rb(t2 + rb(d) * 0x100u + rb(t3 + v)) : rb(t3 + v);
                    } else if (!t2) o = t3 ? rb(t1 + rb(t3 + v)) : rb(t1 + v);
                    else o = rb(t2 + rb(d) * 0x100u + rb(t1 + v));
                    wb(d, o);
                }
                x += cnt; p = pn; dst = dn;
            }
            dst = next; x = x0; next += pitch;
        } while (--h);
        return;
    }
    const bool inside = (!lo && !hi) || (lo <= x && (int32_t)(width + (uint32_t)x) < hi);
    // ESI/EDI per table combination; inside blitters take (h, pitch), the clipping ones (h, pitch, x, lo, hi)
    uint32_t in, out, si, di;
    if (t1) {
        di = t1;
        if (t2) { in = 0xf8a0; out = 0xf620; si = t2; }
        else if (t3) { in = 0xf4b0; out = 0xf2c0; si = t3; }
        else { in = 0xf190; out = 0xefd0; si = t1; }
    } else {
        di = t3;
        if (t3) { if (t2) { in = 0xedd0; out = 0xeb50; si = t2; } else { in = 0xea20; out = 0xe860; si = t3; } }
        else if (t2) { in = 0xe6b0; out = 0xe480; si = t2; }
        else {
            if (inside) { const uint32_t st[2] = {h, pitch}; e->blit(0xfb50, p, p, dst, (uint32_t)lo, si = 0, 0, st, 2); return; }
            const uint32_t st[6] = {p, dst, h, pitch, (uint32_t)lo, (uint32_t)hi};
            e->blit(0xfaa0, (uint32_t)x, p, h, (uint32_t)lo, 0, 0, st, 6);
            return;
        }
    }
    if (inside) { const uint32_t st[2] = {h, pitch}; e->blit(in, dst, p, h, (uint32_t)lo, si, di, st, 2); return; }
    const uint32_t st[5] = {h, pitch, (uint32_t)x, (uint32_t)lo, (uint32_t)hi};
    e->blit(out, dst, p, h, (uint32_t)lo, si, di, st, 5);
}
