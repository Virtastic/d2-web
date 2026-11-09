// 1.13c D2Client+0x4A4F0, written from the disassembly: stdcall 2 (ret 8). arg0 = the interact/unit record U (EBP; +4 type, +0xC id, +0xC4 flags),
// arg1 = a flag read only after the dispatch; retail overwrites the arg0 slot with [U+4] (the slot search compares that type, not the pointer).
// Every global is a DWORD store (the decompile's byte stores are wrong): 0x119663 := 1, 0x119729 := 0, 0x119721 := 1; 0xF23E8 is a WORD compared with 0xFFFF.
// Callees go through cl::Env::H with the registers and stack words of the call site (0x45CC0: EAX=[0xF23E4], ECX=&0xF2330, s0=type, s1=[0x11973B];
// 0x88A70: EBX=the 0x9CCE0 result, s0..s3 = player,0,1,0; 0xC2790: ECX=index, EDX=EDI (1), s0=0; 0x47B20: EDI=[0x11971D]; 0x4A060: EAX=k, s0=[0xF23E8]!=0xFFFF).
// Jump table 0x4A8A0 (state at [[0xDD538+4i]+0x20]): 0 next, 1 0xC2790, 4 0x47B20 when [0x119721], 2/3/above 4 the closing branch.
// Packets go through fx.send (the 0x143E0 stack buffer, EBX = length): 0x30 + dword 1 + dword id (9 bytes), 0x4D + word type (3 bytes).
#include <cstring>
#include "cl_env.h"
namespace d2c4a4f0 {
struct Fx { void* ctx; uint32_t (*ga)(void*, uint32_t); void (*send)(void*, const uint8_t*, uint32_t); };
}
// 1.13c D2Client+0x4A4F0
extern "C" void d2c_4a4f0(uint32_t u, uint32_t a1, const cl::Env& e, const d2c4a4f0::Fx& fx) {
    const auto P = [&](uint32_t rva) { return (uint8_t*)(uintptr_t)fx.ga(fx.ctx, rva); };
    const auto rd = [&](uint32_t rva) { uint32_t v; memcpy(&v, P(rva), 4); return v; };
    const auto wr = [&](uint32_t rva, uint32_t v) { memcpy(P(rva), &v, 4); };
    wr(0xF46C8, 0x10); wr(0xF46C4, 0x10);
    wr(0x11C3DC, 0); wr(0x11C3E0, 0); wr(0x11C3F8, 0); wr(0x11C3FC, 0); wr(0x11C2F8, 0); wr(0x11C2F4, 0);
    if (!u) { e.H(0x48150); return; }
    uint32_t edi = 1;
    wr(0x119663, edi); wr(0x119729, 0);
    wr(0x11966B, e.H(0xc1c6, rd(0x11BBFC), 0xe, 0));
    e.H(0xbe350, 0, 0);
    if (a1 == 0) {
        if (e.H(0x46150) == u) return;
        if (e.H(0x46150)) { e.H(0x48150); return; }
    } else {
        e.H(0x46150);
    }
    wr(0x119721, edi);
    wr(0x11971D, e.R(u + 0xc));
    wr(0x119725, e.R(u + 4));
    const uint32_t t = e.R(u + 4);
    e.W(u + 0xc4, e.R(u + 0xc4) & 0xfffffffdu);
    e.H(0x45cc0, t, rd(0x11973B), 0, 0, 0, rd(0xF23E4), 0xB20F2330u);
    if (rd(0x1197AC)) {
        if (rd(0x1197B0)) { e.H(0x88bf0, 0, 0, 0, 0, 0, 0, 0, rd(0x1197B0)); wr(0x1197B0, 0); }
        wr(0x1197AC, 0);
    }
    uint16_t w23e8; memcpy(&w23e8, P(0xF23E8), 2);
    uint32_t k = 0;
    switch (e.R(u + 4)) {
    case 0xfc: k = 0x18; break;
    case 0x96:
        if ((int32_t)e.H(0xc1c6, rd(0x11BBFC), 0xc, 0) >= 8) e.H(0x4a210);
        k = 0xb; break;
    case 0xc6: k = 8; break;
    case 0x16f: k = 0x15; break;
    case 0x203: k = 0x2b; break;
    default: break;
    }
    if (k) e.H(0x4a060, w23e8 != 0xffff, 0, 0, 0, 0, k);
    const uint32_t old = rd(0xFADA0);
    e.H(0xbf8c);
    if (rd(0xDD5D0) != 0x26) { e.H(0xbf80, 0xB20D359Fu, e.H(0xbf92, 0x139b)); e.H(0x3664, 0xffffffffu); return; }
    bool closing = rd(0x113A68) != 0;
    if (!closing) {
        for (uint32_t i = 0; (int32_t)i < (int32_t)rd(0xDD5D0); ++i) {
            if (rd(0xFAD80 + 4 * i) != edi) continue;
            const uint32_t st = e.R(rd(0xDD538 + 4 * i) + 0x20);
            if (st == 0) continue;
            if (st == 1) { e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, edi); continue; }
            if (st == 4) {
                if (rd(0x119721)) { edi = rd(0x11971D); e.H(0x47b20); e.Edj(0x47b20, edi); edi = 1; }
                continue;
            }
            closing = true; break;
        }
    }
    if (closing) {
        wr(0x119663, 0);
        e.W(u + 0xc4, e.R(u + 0xc4) | 2);
        uint8_t pk[12] = {};
        pk[0] = 0x30; memcpy(pk + 1, &edi, 4); const uint32_t id = e.R(u + 0xc); memcpy(pk + 5, &id, 4);
        fx.send(fx.ctx, pk, 9);
        wr(0x119721, 0);
        e.H(0x46a60, 0, 0, 0, 0, 0, 0, e.R(u + 0xc));
        return;
    }
    const uint32_t pl = rd(0x11BBFC);
    if (pl == 0 || (e.R(pl) == 0 && e.H(0xa5440) == 0)) wr(0xFADA0, edi);
    if (old && rd(0xFADA0) == 0) { e.H(0xbe2e0); e.H(0x492f0); }
    e.H(0x48b20);
    if (a1 != 0) return;
    uint32_t mode = 0, i = 0;
    const uint32_t n = rd(0xF3B9C);
    bool hit = false;
    for (; (int32_t)i < (int32_t)n; ++i) if (rd(0xF37A8 + 0x16 * i) == t) { hit = true; break; }
    if (hit) {
        uint8_t* s = P(0xF37A8 + 0x16 * i);
        if (s[0x12] == 1) { s[0x12] = 0; mode = 2; }
        if (s[0x15] != 1) return;
        s[0x15] = 0;
    }
    const uint32_t esi = e.H(0x9cce0, mode, 0, 0, 0, 0, u);
    if (esi == 0) return;
    e.H(0x22eb0, 0, 0, 0, 0, 0, u);
    wr(0x1197B0, e.H(0x88a70, rd(0x11BBFC), 0, 1, 0, 0, 0, 0, 0, esi));
    if (mode == 2) {
        const uint8_t pk[4] = { 0x4d, (uint8_t)t, (uint8_t)(t >> 8), 0 };
        fx.send(fx.ctx, pk, 3);
        wr(0x1197AC, 1);
        return;
    }
    wr(0x1197B0, 0);
}
