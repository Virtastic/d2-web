// 1.13c D2Client+0x373C0: the two-count caption line ("..." + count of table rows matching kind 6 with flag 1, then flag 0), centred at the frame top. No args. Disassembly-derived.
// buf[200 wchar] = GetStringById(0x1051) (0xD1EC, ECX = id); table at 0x6FBCA2E0 (RVA 0x11A2E0, tbl), 115 records of 10 bytes: dword +0, word +4, dword +6.
// First record with +0 == 6 and +6 == flag (1, then 0) and word +4 != 0xFFFF: x = 0x39F30(eax = 6, edx = flag) (returns a wide string pointer: GetStringById of the record's word); Unicode::sprintf(0x1E, tmp[30 wchar], GetStringById(0x1052), x); strcat(buf, tmp).
// Finally w = 0xD35A(buf); X = [0xDBC48] - ((w - (w >> 31)) >> 1) - 0x3A; Y = [0xDBC4C] - 0xC5; 0xD37E(buf, X, Y, 0, 0). Globals: gl+0 = [0xDBC48], gl+4 = [0xDBC4C].
#include "cl_env.h"
namespace {
uint32_t w16(const cl::Env& e, uint32_t a) { return e.R(a) & 0xffff; }
}
extern "C" void d2cl_373c0(uint32_t tbl, const cl::Env& e) {
    uint16_t buf[200] = {0}, tmp[30] = {0};
    const uint32_t bp = (uint32_t)(uintptr_t)buf, tp = (uint32_t)(uintptr_t)tmp;
    uint32_t s = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x1051);
    for (int i = 0; i < 199; i++) { uint16_t c = (uint16_t)w16(e, s + 2 * i); buf[i] = c; if (!c) break; }
    for (uint32_t flag = 1;; flag--) {
        uint32_t idx = 0; bool hit = false;
        for (uint32_t i = 0; i < 115; i++) {
            if (e.R(tbl + i * 10) == 6 && e.R(tbl + i * 10 + 6) == flag) { idx = i; hit = true; break; }
        }
        if (hit && w16(e, tbl + idx * 10 + 4) != 0xffff) {
            for (int i = 0; i < 30; i++) tmp[i] = 0;
            uint32_t x = e.H(0x39f30, 0, 0, 0, 0, 0, 6, 0, flag);
            uint32_t f = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x1052);
            int o = 0;
            for (int i = 0; o < 29; i++) {
                uint16_t c = (uint16_t)w16(e, f + 2 * i);
                if (!c) break;
                if (c == '%' && w16(e, f + 2 * i + 2) == 's') {
                    i++;
                    for (uint32_t k = 0; o < 29; k++) { uint16_t d = (uint16_t)w16(e, x + 2 * k); if (!d) break; tmp[o++] = d; }
                } else if (c == '%' && w16(e, f + 2 * i + 2) == 'd') {
                    i++;
                    char t[12]; int n = 0; int32_t v = (int32_t)x; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
                    if (v < 0 && o < 29) tmp[o++] = '-';
                    do { t[n++] = (char)('0' + u % 10); u /= 10; } while (u);
                    while (n && o < 29) tmp[o++] = (uint8_t)t[--n];
                } else tmp[o++] = c;
            }
            int b = 0; while (buf[b] && b < 199) b++;
            for (int i = 0; tmp[i] && b < 199; i++) buf[b++] = tmp[i];
            buf[b] = 0;
        }
        if (flag == 0) break;
    }
    const int32_t w = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, bp);
    const int32_t X = (int32_t)e.R(e.gl) - ((w - (w >> 31)) >> 1) - 0x3a;
    const int32_t Y = (int32_t)e.R(e.gl + 4) - 0xc5;
    e.H(0xd37e, (uint32_t)Y, 0, 0, 0, 0, 0, bp, (uint32_t)X);
    if (e.blk) e.blk(e.ctx, 0xd37e, bp, 64);
    (void)tp;
}
