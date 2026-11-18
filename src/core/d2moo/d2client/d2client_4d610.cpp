// 1.13c D2Client+0x4D610: client-side monster class change (re-initialise a live monster as another MonStats row).
// EAX = unit, stdcall (class), ret 4. Tears down the unit's light, overlays and client data, rewrites the class
// (+4), the animation (0xC328), the base stats for the difficulty byte 0x11C390, the draw flags (+0xC4/+0xC8),
// the skills of the row, the palette byte (0xC3C4), and finally the mode words +0x34/+0x38 of the unit's path
// record (+0x54). Every callee goes through Svc: `st` for the stdcall import thunks (words in argument order),
// `rg` for register-convention callees. Module addresses are formed from Svc::mod.
#include <cstdint>
#include <cstring>
namespace d2client_4d610 {
struct Svc {
    uint32_t (*st)(uint32_t rva, uint32_t n, const uint32_t* w);
    // r = eax ecx edx ebx esi edi at the call
    uint32_t (*rg)(uint32_t rva, const uint32_t* r, uint32_t n, const uint32_t* w);
    const uint8_t* dt;          // *sgptDataTables (IAT 0xCE474)
    const uint8_t* bm;          // gdwBitMasks (IAT 0xCEE50): dword +0, bytes +4 +0xC +0x18
    const uint8_t* diff;        // 0x11C390, the difficulty byte
    uint32_t mod;               // module base (retail 0x6FAB0000)
};
static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline int32_t rd16(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline int32_t rs8(uint32_t a) { return *(const int8_t*)(uintptr_t)a; }
static inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }

template <class... T> static uint32_t S(const Svc& s, uint32_t rva, T... v) { const uint32_t w[] = { (uint32_t)v... }; return s.st(rva, sizeof...(T), w); }
static uint32_t R(const Svc& s, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi,
                  uint32_t n = 0, const uint32_t* w = nullptr) {
    const uint32_t r[6] = { eax, ecx, edx, ebx, esi, edi }; return s.rg(rva, r, n, w);
}

void sub_4D610(const Svc& s, uint32_t u, int32_t cls) {
    const uint32_t dt = (uint32_t)(uintptr_t)s.dt;
    if (cls < 0 || cls >= (int32_t)rd32(dt + 0xA80)) return;
    const uint32_t row = (uint32_t)cls * 0x1A8u + rd32(dt + 0xA78);
    if (!row) return;
    const uint32_t m2 = R(s, 0x1260, (uint32_t)cls, 0, 0, u, 0, row);
    if (!m2) return;
    const uint32_t src = s.mod + 0xD6030;
    R(s, 0x69700, u, 0, 0, u, 0, row);
    { const uint32_t w = 0; R(s, 0xA5C50, 0, 0, 0, u, 0, row, 1, &w); }
    S(s, 0xCA78, rd32(u + 0x60));
    const uint32_t l64 = rd32(u + 0x64);
    wr32(u + 0x60, 0);
    if (l64) { R(s, 0xA8630, 0, 0, 0, u, l64, row); wr32(u + 0x64, 0); }
    if (const uint32_t v = S(s, 0xC5C2, u, 0)) R(s, 0xA8630, v, 0, 0, u, v, row);
    if (rd32(u) == 1) {
        const uint32_t pd = rd32(u + 0x14);
        if (pd && rd32(pd + 0x2C)) {
            const uint32_t w[2] = { 0x66, 0 };
            R(s, 0xBF86, 0, rd32(pd + 0x2C), src, u, pd, row, 2, w);
            wr32(pd + 0x2C, 0);
        }
    }
    const uint32_t p2c = rd32(u + 0x2C);
    wr32(u + 0x6C, 0);
    wr32(u + 4, (uint32_t)cls);
    if (!p2c) return;
    S(s, 0xC328, p2c, (uint32_t)rd16(row + 0x32) << 8, src, 0x2B7);
    S(s, 0xCD48, u, 0);
    if ((s.bm[4] & rd8(row + 0xD)) && !rd32(u + 0x60)) wr32(u + 0x60, S(s, 0xC63A, 0, u));
    S(s, 0xC490, u, 0, 0, 0);
    S(s, 0xC40C, u, 0x44, 100, 0);
    S(s, 0xC40C, u, 0x43, 100, 0);
    S(s, 0xC40C, u, 0x45, 100, 0);
    const uint32_t d = *s.diff;
    static const uint8_t kRes[6][2] = { { 0x24, 0x44 }, { 0x25, 0x4A }, { 0x27, 0x50 }, { 0x29, 0x56 }, { 0x2B, 0x5C }, { 0x2D, 0x62 } };
    for (const auto& k : kRes) S(s, 0xC40C, u, k[0], rd16(row + 0x100 + k[1] + 2 * d), 0);
    S(s, 0xC40C, u, 7, 0x8000, 0);
    S(s, 0xC40C, u, 6, 0x8000, 0);
    S(s, 0xC43C, u, 1);
    wr32(u + 0x44, R(s, 0x1380, 0, u + 0x20, 0, u, rd32(u + 0x48), row));
    uint32_t f = rd32(u + 0xC4);
    f = (s.bm[0xC] & rd8(m2 + 4)) ? (f | 2) : (f & ~2u);
    wr32(u + 0xC4, f); wr32(u + 0xC8, rd32(u + 0xC8) & ~0x40000u);
    f = (rd8(m2 + 5) & s.bm[0x18]) ? (f & ~0x20u) : (f | 0x20); wr32(u + 0xC4, f);
    f |= 8; wr32(u + 0xC4, f);
    f = (s.bm[4] & rd8(m2 + 5)) ? (f | 4) : (f & ~4u); wr32(u + 0xC4, f);
    R(s, 0x4C370, u, 0, 0, u, 0, row);
    R(s, 0x4CEC0, (uint32_t)cls, u, 0, u, 0, row);
    R(s, 0x4C150, u, 0, 0, u, 0, row);
    R(s, 0x20F20, 0, 0, 0, u, 0, row);
    wr32(u + 0xA8, S(s, 0xCD6C, 0));
    for (uint32_t i = 0; i < 8; ++i) {
        const int32_t sk = rd16(row + 0x170 + 2 * i);
        if (sk < 0) continue;
        S(s, 0xC550, u, sk, rs8(row + 0x198 + i), 0, src, 0x2E9);
    }
    if (!(rd8(row + 0xD) & s.bm[0])) {
        const uint64_t n = (uint64_t)rd32(u + 0x20) * 0x6AC690C5u + rd32(u + 0x24);
        wr32(u + 0x20, (uint32_t)n); wr32(u + 0x24, (uint32_t)(n >> 32));
        S(s, 0xC3C4, p2c, (uint32_t)n & 0x3F);
    }
    R(s, 0x66950, 0, 0, 0, u, u, row);
    const uint8_t m = rd8(row + 0x4D);
    uint32_t mode = m < 8 ? m + 2u : 2u;
    const int32_t c = (int32_t)rd32(u + 4);
    const int32_t e = (c >= 0 && c < (int32_t)rd32(dt + 0xA80)) ? c : -1;
    if (e == 0x9C || e == 0xD3 || e == 0xF2 || e == 0xF3) {
        if (*s.diff == 0) mode = 2;
        else if (*s.diff == 1) mode = 3;
        else if (*s.diff == 2) mode = 4;
    } else if (e == 0x14D) mode = 5;
    else if (e == 0x20F) { const uint32_t w[7] = { u, 5, 0, 0, 0, 0, 0 }; R(s, 0x6B300, 0xE3, 0, 0, u, 0, row, 7, w); }
    if (mode >= 8) mode = 2;
    wr32(rd32(u + 0x54) + 0x34, mode);   // retail asserts on a NULL +0x54 (never reached by a live unit)
    wr32(rd32(u + 0x54) + 0x38, mode);
}
}
