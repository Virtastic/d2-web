// 1.13c D2Client+0x763C0 party-row hover tooltip (EBX player entry, no stack arguments, void). Globals (addresses in G): viewed
// unit 0x11BBFC (+0xC id), left 0x11B9A0, bottom 0x11B9A4, 0x11B980 (+0x1EB bit 4: the second line), box colour byte 0x11BFF8,
// string ids 0xE2658 / 0xE2660 (first line) and 0xE2668 / 0xE2670 (second line), each 2 x 2 words. Three 128-char Unicode buffers
// (??_FUnicode, IAT 0xCEB00): T1, T2, N. A null entry is the fatal 0xBF92(0x171) / 0xBF80 / 0x3664 path; N = toUnicode (IAT
// 0xCEAC4; ECX N, EDX entry+0x66 or "ERROR PLAYER NAME"; 0x80). With a viewed unit, 0x4FD90 (ESI, EDX; flag) answers four
// relations: a = (EDX unit id, ESI entry+0x10; 8), c = (same; 1), b = (ESI unit id, EDX entry+0x10; 8), d = (same; 1); no unit:
// all 0. Line 1: a == b copies (strcpy, IAT 0xCEAB8; ECX T1, EDX 0xD1EC(id)) id = table[b + 2a] from 0xE2658 when the entry's
// +0x1C kind is < 0 or > 1 and not 6, else from 0xE2660; a != b formats (Unicode sprintf, IAT 0xCEAB4, cdecl)(0x80, T1, 0xD1EC(id),
// N). Line 2 the same over c, d with 0xE2668 / 0xE2670 when +0x1EB bit 4 is set (height 0x1E, else 0x10). Colours: line 1 8 for a,
// 1 for b (b wins), line 2 9 for c, 4 for d. w = max(0xD35A(T1), 0xD35A(T2)) + 10; 0xD0BA(L + 0x24, 0x23 - B, L + 0x24 + w,
// height + 0x23 - B, colour byte, 2); 0xD37E(ECX T1, EDX L + 0x29; 0x32 - B, colour 1, 0); 0xD37E(ECX T2, EDX L + 0x29;
// 0x41 - B, colour 2, 0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

struct D2c763c0G { uint32_t unit, L, B, opt, colb, ids; };
static uint32_t rd32(uint32_t a) { uint32_t x; memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static uint16_t rd16(uint32_t a) { uint16_t x; memcpy(&x, (const void*)(uintptr_t)a, 2); return x; }
static uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_763c0(const D2cSvc& v, const D2c763c0G* g, uint32_t ent) {
    char16_t t1[0x80] = {}, t2[0x80] = {}, nm[0x80] = {};
    if (!ent) {
        const uint32_t s = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x171);
        d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB20D359Fu, s);
        d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    static const char err[] = "ERROR PLAYER NAME";
    const uint32_t name = ent + 0x66 ? ent + 0x66 : P(err);
    d2c_call(v, 0xceac4, 0, P(nm), name, 0, 0, 0x80);
    const uint32_t u = rd32(g->unit);
    uint32_t a = 0, b = 0, c = 0, d = 0;
    if (u) {
        a = d2c_call(v, 0x4fd90, 0, 0, rd32(u + 0xc), 0, rd32(ent + 0x10), 8) ? 1 : 0;
        c = d2c_call(v, 0x4fd90, 0, 0, rd32(u + 0xc), 0, rd32(ent + 0x10), 1) ? 1 : 0;
        b = d2c_call(v, 0x4fd90, 0, 0, rd32(ent + 0x10), 0, rd32(u + 0xc), 8) ? 1 : 0;
        d = d2c_call(v, 0x4fd90, 0, 0, rd32(ent + 0x10), 0, rd32(u + 0xc), 1) ? 1 : 0;
    }
    auto line = [&](char16_t* t, uint32_t x, uint32_t y, uint32_t tb) {
        const int32_t k = (int32_t)rd32(ent + 0x1c);
        const uint32_t tab = (k < 0 || (k > 1 && k != 6)) ? tb : tb + 8;
        const uint32_t id = rd16(tab + 2 * (y + 2 * x));
        if (x == y) d2c_call(v, 0xceab8, 0, P(t), d2c_call(v, 0xd1ec, 0, id, 0, 0, 0), 0, 0);
        else {
            const uint32_t f = d2c_call(v, 0xd1ec, 0, id, 0, 0, 0);
            d2c_call(v, 0xceab4, 0, 0, 0, 0, 0, 0x80, P(t), f, P(nm));
        }
    };
    line(t1, a, b, g->ids);
    int32_t h = 0x10;
    if (*(const uint8_t*)(uintptr_t)(rd32(g->opt) + 0x1eb) & 4) { h = 0x1e; line(t2, c, d, g->ids + 0x10); }
    const uint32_t c1 = b ? 1 : a ? 8 : 0, c2 = d ? 4 : c ? 9 : 0;
    int32_t w = (int32_t)d2c_call(v, 0xd35a, 0, P(t1), 0, 0, 0) + 10;
    const int32_t w2 = (int32_t)d2c_call(v, 0xd35a, 0, P(t2), 0, 0, 0) + 10;
    if (w <= w2) w = w2;
    const int32_t L = (int32_t)rd32(g->L), B = (int32_t)rd32(g->B);
    d2c_call(v, 0xd0ba, 0, 0, 0, 0, 0, (uint32_t)(L + 0x24), (uint32_t)(0x23 - B), (uint32_t)(L + 0x24 + w), (uint32_t)(h + 0x23 - B),
             *(const uint8_t*)(uintptr_t)g->colb, 2);
    d2c_call(v, 0xd37e, 0, P(t1), (uint32_t)((int32_t)rd32(g->L) + 0x29), 0, 0, (uint32_t)(0x32 - (int32_t)rd32(g->B)), c1, 0);
    d2c_call(v, 0xd37e, 0, P(t2), (uint32_t)((int32_t)rd32(g->L) + 0x29), 0, 0, (uint32_t)(0x41 - (int32_t)rd32(g->B)), c2, 0);
}
