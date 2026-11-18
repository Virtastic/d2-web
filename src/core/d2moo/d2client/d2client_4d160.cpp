// 1.13c D2Client+0x4D160: client-side monster initialisation. EAX = unit, stdcall (a0, a1, a2, hcIdx, a4), ret 0x14.
// Reads the MonStats row hcIdx (sgptDataTables +0xA78 rows of 0x1A8, +0xA80 count) and its MonStats2 record (0x1260),
// sets the base stats for the current difficulty (byte 0x11C390), the skills of the row, the unit's draw flags (+0xC4)
// and finally the palette/light byte handed to 0xC3C4. Every callee goes through Svc: `st` for the stdcall import thunks
// (words in argument order), `rg` for the module-internal register-convention callees. Module addresses (the source-file
// string 0xD6030, the callback 0x4C990) are formed from Svc::mod.
#include <cstdint>
#include <cstring>
namespace d2client_4d160 {
struct Svc {
    uint32_t (*st)(uint32_t rva, uint32_t n, const uint32_t* w);
    // r = eax ecx edx ebx esi edi at the call
    uint32_t (*rg)(uint32_t rva, const uint32_t* r, uint32_t n, const uint32_t* w);
    const uint8_t* dt;          // *sgptDataTables (IAT 0xCE474)
    const uint8_t* bm;          // gdwBitMasks (IAT 0xCEE50): dword +0, bytes +4 +0xC +0x18
    const uint8_t* diff;        // 0x11C390, the difficulty byte
    const uint32_t* g119854;    // 0x119854
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

void sub_4D160(const Svc& s, uint32_t u, uint32_t a0, uint32_t a1, uint32_t a2, int32_t hc, uint32_t a4) {
    const uint32_t dt = (uint32_t)(uintptr_t)s.dt;
    if (hc < 0 || hc >= (int32_t)rd32(dt + 0xA80)) return;
    const uint32_t row = (uint32_t)hc * 0x1A8u + rd32(dt + 0xA78);
    if (!row) return;
    const uint32_t m2 = R(s, 0x1260, (uint32_t)hc, 0, 0, 0, 0, u);
    if (!m2) return;
    const uint32_t d = *s.diff;
    if (d >= 3) { S(s, 0xBF92, 0x1F0); return; }   // retail: the Fog assert pair, then exit(-1); not reached by a valid difficulty
    S(s, 0xC490, u, 0, 0, 0);
    S(s, 0xC40C, u, 0xC, rd16(row + 0xAA + 2 * d), 0);
    S(s, 0xC40C, u, 0x44, 100, 0);
    S(s, 0xC40C, u, 0x43, 0x4B, 0);
    S(s, 0xC40C, u, 0x45, 100, 0);
    static const uint8_t kRes[6][2] = { { 0x24, 0x44 }, { 0x25, 0x4A }, { 0x27, 0x50 }, { 0x29, 0x56 }, { 0x2B, 0x5C }, { 0x2D, 0x62 } };
    for (const auto& k : kRes) S(s, 0xC40C, u, k[0], rd16(row + 0x100 + k[1] + 2 * d), 0);
    S(s, 0xCDEA, u, *s.g119854, (uint32_t)*s.diff);
    S(s, 0xC40C, u, 7, 0x6400, 0);
    S(s, 0xC40C, u, 6, 0x6400, 0);
    S(s, 0xC3F4, 0, a0, a1, a2, u, 1);
    const uint32_t p2c = rd32(u + 0x2C);
    const uint32_t src = s.mod + 0xD6030;
    S(s, 0xC328, p2c, (uint32_t)rd16(row + 0x32) << 8, src, 0x208);
    if (!rd32(u + 0x60) && ((s.bm[4] & rd8(row + 0xD)) || hc == 0x123 || hc == 0x165 || hc == 0x1A1 || hc == 0x1A2))
        wr32(u + 0x60, S(s, 0xC63A, 0, u));
    S(s, 0xC43C, u, a4);
    if (S(s, 0xC538, u) && !(s.bm[0xC] & rd8(m2 + 6))) {
        S(s, 0xCDBA, u, 1);
        S(s, 0xCDD2, u, 5);
        S(s, 0xC400, rd32(u + 0x2C), 0x8000);
    }
    wr32(u + 0x44, R(s, 0x1380, 0, u + 0x20, 0, 0, rd32(u + 0x48), u));
    uint32_t f = rd32(u + 0xC4);
    f = (s.bm[0xC] & rd8(m2 + 4)) ? (f | 2) : (f & ~2u); wr32(u + 0xC4, f);
    f = (rd8(m2 + 5) & s.bm[0x18]) ? (f & ~0x20u) : (f | 0x20); wr32(u + 0xC4, f);
    f |= 8; wr32(u + 0xC4, f);
    f = (s.bm[4] & rd8(m2 + 5)) ? (f | 4) : (f & ~4u); wr32(u + 0xC4, f);
    R(s, 0x4C370, u, 0, 0, 0, 0, u);
    R(s, 0x4CEC0, (uint32_t)hc, u, 0, 0, 0, u);
    R(s, 0x4C150, u, 0, 0, 0, 0, u);
    wr32(u + 0xA8, S(s, 0xCD6C, 0));
    R(s, 0x20F20, 0, 0, 0, u, 0, u);
    uint32_t slot = rd32(S(s, 0xC26E, d) + 0x10);
    for (uint32_t i = 0; i < 8; ++i) {
        const int32_t sk = rd16(row + 0x170 + 2 * i), lv = rs8(row + 0x198 + i);
        if (sk < 0 || lv <= 0) continue;
        S(s, 0xC550, u, sk, (uint32_t)lv + slot, 1, src, 0x23F);
        const uint32_t w = (uint32_t)rs8(row + 0x180 + i);
        S(s, 0xCDC0, R(s, 0xC838, 0, u, (uint32_t)sk, 0, 0, u, 1, &w));
    }
    S(s, 0xCDA8, u, slot);
    const int32_t cls = (int32_t)rd32(u + 4);
    if (cls >= 0 && cls < (int32_t)rd32(dt + 0xA80)) {
        const uint32_t r2 = (uint32_t)cls * 0x1A8u + rd32(dt + 0xA78);
        if (r2) { slot = rd8(r2 + 0xD) & rd32((uint32_t)(uintptr_t)s.bm); if (slot) return; }
    }
    slot &= ~0xFFu;   // only the low byte of the slot word is rewritten below
    uint32_t b = 0;
    const int32_t t = (int32_t)R(s, 0x1700, u, 0, 0, 0, 0, u);
    const int32_t e = (t >= 0 && t < (int32_t)rd32(dt + 0xA80)) ? t : -1;
    if (R(s, 0x1C10, (uint32_t)cls, 0, 2, 0, 0, u)) {
        const uint64_t n = (uint64_t)rd32(u + 0x20) * 0x6AC690C5u + rd32(u + 0x24);
        wr32(u + 0x20, (uint32_t)n); wr32(u + 0x24, (uint32_t)(n >> 32));
        b = (uint32_t)n & 0x3F;
    }
    if (e == 0x60) b = 7;
    else if (e == 0x12D) { const uint32_t w[2] = { 0, 0 }; b = R(s, 0x16C20, 0, 0, 0, s.mod + 0x4C990, 0, u, 2, w) & 0xFF; }
    if (hc == 0x15F) b = 0x18;
    else if (hc == 0x161) b = 0x28;
    else if (hc == 0x160 || hc == 0x165 || hc == 0x158) b = 0;
    S(s, 0xC3C4, p2c, slot | b);
}
}
