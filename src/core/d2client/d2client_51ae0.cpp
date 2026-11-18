// 1.13c D2Client+0x51AE0: click-target resolve. Retail: EAX = unit (EDI), stack (ret 0x14) = a0 (EBP, the
// click id), a1 = &x, a2 = &y, a3 (byte flags), a4 (nonzero enables the 0x1500 free-cell search). Every callee and global is an injected seam;
// a callee's register arguments are named parameters in retail's order. Corrected 2026-11-05 from the old b63 port: the stack words are a0..a4
// (the decompile's param_3 test was a4, and 0x1390/0x1650 take a1/a2, not a4).
#include <cstdint>
#include <cstring>
namespace d2client_51ae0 {
struct Env {
    uint32_t (*c640)(uint32_t ecx);                              // Ordinal_10858, ECX = a0: the record whose bytes +6/+7 are class flags
    uint32_t (*f55440)(uint32_t eax_unit);
    uint32_t (*f2c5c0)();
    uint32_t (*f55b40)(uint32_t ecx, uint32_t edx);
    uint32_t (*o10331)(uint32_t p);                              // thunk 0xC1F0: D2Common 10331, stdcall, one word (ret 4)
    void (*o10742)(uint32_t v, uint32_t q);                      // thunk 0xCD0C: D2Common 10742, stdcall, two words (ret 8); q = p+4, pushed before the 10331 call
    uint32_t (*f1630)(uint32_t ecx_unit); uint32_t (*f1660)(uint32_t ecx_unit);
    int32_t (*pathX)(uint32_t); int32_t (*pathY)(uint32_t);      // thunks 0xC3BE / 0xC2EC
    void (*f50b30)(uint32_t eax_player, uint32_t a1, uint32_t a2);
    uint32_t (*o10143)(uint32_t player);                         // thunk 0xCCCA
    uint32_t (*o10787)(uint32_t player, uint32_t a0);            // thunk 0xCB74
    uint32_t (*f1650)(uint32_t eax_st, uint32_t a0, uint32_t edi, uint32_t a1, uint32_t a2);
    uint32_t (*f1500)(uint32_t ebx_player, uint32_t st);
    uint32_t (*f1390)(uint32_t edi, uint32_t ebx_a2, uint32_t a1);
};
struct G {                                                       // module globals, read live and written back
    uint32_t g2f8, g3a68, g2f4, g964c, g9638, g985c, g97f8;      // 0xCC2F8 0xC3A68 0xCC2F4 0xC964C 0xC9638 0xC985C 0xC97F8 (VA 6fbc....)
    uint8_t* g3afa;                                              // 6fbc3afa
    uint32_t* flags;                                             // *DAT_6fbcc3c4
    uint32_t player;                                             // DAT_6fbcbbfc
};
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// bm = *gdwBitMasks (dword view)
uint32_t sub_51AE0(const Env& e, G& g, const uint32_t* bm, uint32_t eax, uint32_t a0, uint32_t* a1, uint32_t* a2, uint32_t a3, uint32_t a4) {
    const uint32_t st = e.c640(a0);
    const uint32_t* stb = (const uint32_t*)(uintptr_t)st;
    const uint32_t st6 = ((const uint8_t*)stb)[6], st7 = ((const uint8_t*)stb)[7];
    const uint32_t esi = g.player;
    uint32_t edi = eax;
    bool skip = false;
    if (edi) {
        if (rd(edi, 0) == 0) {
            if (rd(edi, 0xC4) & 0x10000) return edi;
            if (rd(edi, 0x10) == 0 || rd(edi, 0x10) == 0x11) return edi;
        }
        if ((((const uint8_t*)bm)[0] & st7) != 0 || e.f55440(edi) == 0) skip = true;
        else {
            if (g.g2f8 == 0) {
                if (g.g3a68 == 0 || e.f2c5c0() != 0) {
                    *g.g3afa = 0;
                    if (g.g2f4 != 0 && g.g964c == 5) {
                        const uint32_t p = e.f55b40(g.g9638, 5);
                        if (p) { const uint32_t q = rd(p, 4); e.o10742(e.o10331(p), q); *g.flags |= 1; }
                    }
                    g.g2f4 = 0;
                } else *g.g3afa = 0;
            }
            *a1 = e.f1630(edi);
            *a2 = e.f1660(edi);
            edi = 0;
        }
    }
    if (!skip) {
        const uint32_t t = rd(esi, 0), f = rd(esi, 0x2C);
        const bool direct = t == 2 || (t > 3 && t < 6);
        const uint32_t px = direct ? rd(f, 0xC) : (f ? (uint32_t)e.pathX(f) : 0);
        if (*a1 == px) {
            const uint32_t t2 = rd(esi, 0), f2 = rd(esi, 0x2C);
            const bool d2 = t2 == 2 || (t2 > 3 && t2 < 6);
            const uint32_t py = d2 ? rd(f2, 0x10) : (f2 ? (uint32_t)e.pathY(f2) : 0);
            if (*a2 == py) e.f50b30(esi, (uint32_t)(uintptr_t)a1, (uint32_t)(uintptr_t)a2);
        }
    }
    if (((a3 & 8) && (a3 & 1) && !(a3 & 0x20) && e.o10143(esi) != 0)) return edi;
    if (e.o10787(esi, a0) != 0) return edi;
    if (esi == g.player && g.g97f8 < g.g985c) return edi;
    if (bm[6] & st6) return e.f1650(st, a0, edi, (uint32_t)(uintptr_t)a1, (uint32_t)(uintptr_t)a2);
    if (edi == 0 && (bm[5] & st6) && a4 != 0) return e.f1500(esi, st);
    if (bm[7] & st6) return e.f1390(edi, (uint32_t)(uintptr_t)a2, (uint32_t)(uintptr_t)a1);
    return edi;
}
}
