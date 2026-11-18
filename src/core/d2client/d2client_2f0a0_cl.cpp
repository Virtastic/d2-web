// 1.13c D2Client+0x2F0A0: draw a centred "lo-hi" range (damage / quantity) line. ECX = first value, EAX = second value; stack s0 mode, s1 left, s2 y, s3 right (ret 0x10). Disassembly-derived.
// second := first + 1 unless first < second. Text (wsprintfA, "" is the string at 0xD359F): "%d" (dead, first != second), "%d-%d" both < 10000,
// "%s%d-%dK" (second >= 10000, first < 10000), "%s%dK-%dK" (first >= 10000), thousands = (v + 500) / 1000. Widened into a zeroed 32 wchar buffer (0xCEAC4).
// 0xD3B4(wide, &w, &h): w; scaled = w * 11 / 7; if scaled > right - left: 0xD3A8(6), y-- else 0xD3A8(1).
// ybase = [0xDBC4C] + [0x11B9A4] + y - 0x1E0; x = left + [0x11B9A0]; room = right - left + 1; w2 = 0xD35A(wide); w2 < room: x += (room - w2) >> 1; 0xD37E(wide, x, ybase, mode, 0).
// Globals: gl+0 = [0xDBC4C], gl+8 = [0x11B9A0], gl+12 = [0x11B9A4].
#include "cl_env.h"
namespace {
int put_int(char* o, int32_t v) {
    char t[12]; int n = 0, k = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    if (v < 0) o[k++] = '-';
    do { t[n++] = (char)('0' + u % 10); u /= 10; } while (u);
    while (n) o[k++] = t[--n];
    return k;
}
int32_t kdiv(int32_t v) { return (int32_t)((uint32_t)v + 500u) / 1000; }
}
extern "C" void d2cl_2f0a0(int32_t first, int32_t second, uint32_t mode, int32_t left, int32_t y, int32_t right, const cl::Env& e) {
    uint16_t wide[32] = {0};
    int32_t esi = first, edi = second;
    if (esi >= edi) edi = esi + 1;
    char nb[64]; int k = 0;
    if (esi == edi) k += put_int(nb + k, esi);
    else if (esi < 10000 && edi < 10000) { k += put_int(nb + k, esi); nb[k++] = '-'; k += put_int(nb + k, edi); }
    else if (esi < 10000) { k += put_int(nb + k, esi); nb[k++] = '-'; k += put_int(nb + k, kdiv(edi)); nb[k++] = 'K'; }
    else { k += put_int(nb + k, kdiv(esi)); nb[k++] = 'K'; nb[k++] = '-'; k += put_int(nb + k, kdiv(edi)); nb[k++] = 'K'; }
    for (int i = 0; i < k && i < 31; i++) wide[i] = (uint8_t)nb[i];
    const uint32_t wp = (uint32_t)(uintptr_t)wide;
    uint32_t wh[2] = {0, 0};
    e.H(0xd3b4, (uint32_t)(uintptr_t)&wh[1], 0, 0, 0, 0, 0, wp, (uint32_t)(uintptr_t)&wh[0]);
    int32_t sc = (int32_t)((uint32_t)wh[0] * 11u) ; sc = sc / 7;
    if (sc > right - left) { e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 6); y--; }
    else e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 1);
    const uint32_t ybase = e.R(e.gl) + e.R(e.gl + 12) + (uint32_t)y - 0x1e0u;
    int32_t x = left + (int32_t)e.R(e.gl + 8);
    const int32_t room = right - left + 1;
    const int32_t w2 = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, wp);
    if (w2 < room) x += (room - w2) >> 1;
    e.H(0xd37e, ybase, mode, 0, 0, 0, 0, wp, (uint32_t)x);
    if (e.blk) e.blk(e.ctx, 0xd37e, wp, 16);
}
