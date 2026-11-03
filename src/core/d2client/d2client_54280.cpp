// 1.13c D2Client+0x54280 -- item-stat description text by selector. EAX = the stat block, stdcall(selector, dest). Selectors 0x12, 0x15-0x17, 0x30/0x32/0x34/0x36/0x39 append text
// (return 1, or 0 when gated off); 0x11, 0x18, 0x31, 0x33, 0x35, 0x37, 0x3A/0x3B return a field; everything else returns 0.
#include "d2c_wstr.h"
struct D2c54280Env { uint32_t (*str)(uint32_t id); };
static int32_t& F(uint8_t* s, uint32_t o) { return *(int32_t*)(s + o); }
extern "C" int32_t d2c_54280r(uint8_t* s, uint32_t sel, uint32_t dst, const D2c54280Env& e) {
    uint16_t loc[256] = {};
    const uint32_t L = (uint32_t)(uintptr_t)loc;
    auto out2 = [&](uint32_t id, int32_t a, int32_t b) { int32_t v[2] = { a, b }; d2c::wfmt(loc, e.str(id), v, 2); };
    auto out1 = [&](uint32_t id, int32_t a) { int32_t v[1] = { a }; d2c::wfmt(loc, e.str(id), v, 1); };
    switch (sel) {
    case 0x11: return F(s, 0x28);
    case 0x12: {
        const uint32_t s1 = e.str(0xfa1), s2 = e.str(0xf9b), s3 = e.str(0xfa2);
        uint16_t num[10]; d2c::wdec(num, F(s, 0x18));
        if (F(s, 0x28) == 0) return 0;
        d2c::wcpy(L, s3); d2c::wcat(L, (uint32_t)(uintptr_t)num); d2c::wcat(L, s1); d2c::wcat(L, s2); d2c::wcat(L, e.str(0x2727));
        break; }
    case 0x15: case 0x16: case 0x17:
        if (F(s, 0x14) == 0) {
            if (F(s, 0x10) == 0) return 0;
            if (F(s, 4) <= F(s, 0)) { F(s, 0x14) = 0; F(s, 0x10) = 0; return 0; }
            F(s, 0x14) = 1; out2(0xe27, F(s, 0), F(s, 4));
        }
        break;
    case 0x18: return F(s, 0x14) == 0 ? F(s, 0x10) : 1;
    case 0x30: if (F(s, 0x70) == 0) return 0; if (F(s, 0x60) < F(s, 0x64)) out2(0xe1d, F(s, 0x60), F(s, 0x64)); else out1(0xe1c, F(s, 0x64)); break;
    case 0x31: return F(s, 0x70);
    case 0x32: if (F(s, 0x58) == 0) return 0; if (F(s, 0x48) < F(s, 0x4c)) out2(0xe21, F(s, 0x48), F(s, 0x4c)); else out1(0xe20, F(s, 0x4c)); break;
    case 0x33: return F(s, 0x58);
    case 0x34: if (F(s, 0xa0) == 0) return 0; if (F(s, 0x94) <= F(s, 0x90)) out1(0xe22, F(s, 0x94)); else out2(0xe23, F(s, 0x90), F(s, 0x94)); break;
    case 0x35: return F(s, 0xa0);
    case 0x36: if (F(s, 0x40) == 0) return 0; if (F(s, 0x30) < F(s, 0x34)) out2(0xe1f, F(s, 0x30), F(s, 0x34)); else out1(0xe1e, F(s, 0x34)); break;
    case 0x37: return F(s, 0x40);
    case 0x39: {
        if (F(s, 0x88) == 0) return 0;
        if (F(s, 0x84) < 1) F(s, 0x84) = 1;
        const int32_t q = F(s, 0x80) / F(s, 0x84); F(s, 0x80) = q;
        const int32_t t = q / 25;
        const int32_t c = (int32_t)((uint32_t)F(s, 0x7c) * (uint32_t)q + 0x80u) >> 8;
        const int32_t a = (int32_t)((uint32_t)F(s, 0x78) * (uint32_t)q + 0x80u) >> 8;
        F(s, 0x78) = a; F(s, 0x7c) = c;
        if (c <= a) { int32_t v[2] = { c, t }; d2c::wfmt(loc, e.str(0xe24), v, 2); }
        else { int32_t v[3] = { a, c, t }; d2c::wfmt(loc, e.str(0xe25), v, 3); }
        break; }
    case 0x3a: case 0x3b: return F(s, 0x88);
    default: return 0;
    }
    d2c::wcat(dst, L);
    return 1;
}
