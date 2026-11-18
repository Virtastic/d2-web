// 1.13c D2Client+0x55B20, written from the disassembly: thiscall (ECX = thisU) + 7 stack words, ret 0x1c. Stack args: a0 unit, a1 dst (the Unicode
// string the lines are appended to), a2 unused, a3 hasDst, a4 p4, a5 p5, a6 sep. Locals: two 0x100-byte Unicode buffers tmp (esp+0x2c) and tmp2
// (esp+0x12c), each filled by 0x80 calls of the Unicode default constructor (IAT 0xCEB00, ECX = element, stride 2), the 168-byte stat scratch
// record at esp+0x22c (ESI of 0x54000, EAX of 0x54280), and the 0x1ff-entry property buffer at esp+0x2d4 (8-byte entries: WORD layer, DWORD value).
// Callee convention per call site (H(rva, s0..s4, eax, ecx, edx, ebx, esi)): thunks 0xC4DE/0xC442/0xCCA0/0xC4F0/0xC526/0xC568/0xD1EC (ECX = id)/0xC1CC/
// 0xC454/0xCCBE/0xC4B4/0xC1BA/0xC1C6/0xC916/0xC9C4/0xC220 are stdcall; 0x54880 3 words; 0x54780 1; 0x54000 1 + ESI; 0x54280 2 + EAX; 0x54E10 4 + EAX + ESI;
// Unicode::strcat (IAT 0xCEABC) is fastcall ECX = dst, EDX = src. sgptDataTables is the twice-indirect slot 0xCE474, read through e.dt.
#include <cstring>
#include "cl_env.h"
// 1.13c D2Client+0x55B20
extern "C" void d2c_55b20(uint32_t thisU, uint32_t unit, uint32_t dst, uint32_t a2, uint32_t hasDst, uint32_t p4, uint32_t p5, uint32_t sep, const cl::Env& e) {
    (void)a2;
    char16_t tmp[0x80], tmp2[0x80];
    uint32_t scratch[0x2a], rec[0x400];
    memset(tmp, 0, sizeof tmp); memset(tmp2, 0, sizeof tmp2); memset(scratch, 0, sizeof scratch); memset(rec, 0, sizeof rec);
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    const auto strcat_ = [&](uint32_t d, uint32_t s) { e.H(0xceabc, 0, 0, 0, 0, 0, 0, d, s); };
    for (int i = 0; i < 0x80; ++i) e.H(0xceb00, 0, 0, 0, 0, 0, 0, P(tmp + i));
    for (int i = 0; i < 0x80; ++i) e.H(0xceb00, 0, 0, 0, 0, 0, 0, P(tmp2 + i));
    if (e.H(0xc220, unit) == 0xb) { e.H(0x54880, unit, dst, 0xB20F20C8u); return; }
    const uint32_t a = e.H(0xc4de, unit, p4, p5);
    if (!a) return;
    uint32_t edx, ecx, eax;
    if (!unit) { edx = 0xffffffffu; ecx = 6; eax = 0; } else { edx = e.R(unit + 0xc); ecx = e.R(unit); eax = e.R(unit + 8); }
    const uint32_t h = e.H(0xc442, eax, 0, 0, ecx, edx);
    e.H(0xcca0, h, a);
    if (thisU) { const uint32_t b = e.H(0xc4de, unit, thisU, p5); if (b) e.H(0xcca0, h, b); }
    if (unit && e.R(unit) == 4 && e.R(unit + 0x60)) {
        uint32_t n = e.H(0xc4f0, e.R(unit + 0x60));
        while (n) {
            const uint32_t it = e.H(0xc526, n);
            if (!it) break;
            n = e.H(0xc568, n);
            const uint32_t c = e.H(0xc4de, it, p4, p5);
            if (c) e.H(0xcca0, h, c);
        }
    }
    const uint32_t s1 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf9e);
    const uint32_t s2 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf9b);
    const uint32_t s3 = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0xf0c);
    if (hasDst && e.H(0xc1cc, unit, 0x39) && e.H(0xc454, unit, 0x7a, 0) == 0) { e.H(0x54780, P(tmp2)); strcat_(dst, P(tmp2)); }
    e.H(0x54000, unit, 0, 0, 0, 0, 0, 0, 0, 0, P(scratch));
    uint32_t any = 0;
    uint32_t dt = e.dt;
    for (uint32_t i = 0; (int32_t)i < (int32_t)e.R(dt + 0xbdc); ++i) {
        const int32_t sid = e.R16s(e.R(dt + 0xbd8) + i * 2);
        if (sid >= 0 && sid < (int32_t)e.R(dt + 0xbd4)) {
            const int32_t n = (int32_t)e.H(0xccbe, h, (uint32_t)sid, P(rec), 0x1ff);
            for (int32_t k = 0; k < n; ++k) {
                const uint32_t val = rec[k * 2 + 1], layer = rec[k * 2] & 0xffffu;
                if (!val) continue;
                if (e.H(0x54280, (uint32_t)sid, dst, 0, 0, 0, P(scratch)) != 0) continue;
                if (!e.H(0x54e10, unit, h, layer, val, 0, (uint32_t)sid, 0, 0, 0, P(tmp))) continue;
                if (sid == 0x17) { if (e.H(0xc4b4, h, 0x15, 0)) continue; }
                else if (sid == 0x18) { if (e.H(0xc4b4, h, 0x16, 0)) continue; }
                if (sep) strcat_(P(tmp), s1);
                if (any && !sep) { strcat_(dst, s3); strcat_(dst, s2); }
                strcat_(dst, P(tmp));
                any = 1;
            }
        }
    }
    if (unit && e.R(unit) == 4) {
        const uint32_t mon = e.H(0xc1ba, e.R(unit + 4));
        if (e.R8(mon + 0x113) == 0 && e.R8(mon + 0x112) != 0 && (int32_t)e.H(0xc1c6, unit, 0x98, 0) <= 0 && e.H(0xc916, unit) == 0) {
            const uint32_t tail = e.H(0xd1ec, 0, 0, 0, 0, 0, 0, 0x52f8);
            if (any && !sep) { strcat_(dst, s3); strcat_(dst, s2); }
            strcat_(dst, tail);
            if (sep) strcat_(dst, s1);
        }
    }
    e.H(0xc9c4, h);
}
