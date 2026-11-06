// 1.13c D2Client+0x4DC70: client unit mode change (apply a server mode packet to a live unit).
// ECX = mode (0..0x1d, larger values take the default body), EDX = unit, stack word = packet words (or null), ret 4.
// Resets the per-type draw state, applies the packet's light/animation fields, validates the unit's class against
// the MonStats table (sgptDataTables +0xA78 rows of 0x1A8, +0xA80 count), then runs one body per mode group and the
// common tail (stat 0x43 resync, 0x212F0 notify, player-light refresh). Mode 8 (death) dispatches on the class from
// 0x1700 through two retail jump tables. Every callee goes through Svc: `st` for the stdcall import thunks (words in
// argument order), `rg` for register-convention callees. Module addresses are formed from Svc::mod.
#include <cstdint>
#include <cstring>
namespace d2client_4dc70 {
struct Svc {
    uint32_t (*st)(uint32_t rva, uint32_t n, const uint32_t* w);
    // r = eax ecx edx ebx esi edi at the call
    uint32_t (*rg)(uint32_t rva, const uint32_t* r, uint32_t n, const uint32_t* w);
    const uint8_t* dt;          // *sgptDataTables (IAT 0xCE474)
    const uint8_t* bm;          // gdwBitMasks (IAT 0xCEE50): dword +0xC, byte +0x18
    const uint8_t* tbl;         // 0xD2E68: 0x1E pairs of dwords, word 0 = the animation mode of each unit mode
    const uint8_t* f46c4;       // 0xF46C4 / 0xF46C8: option dwords (bit 3 tested)
    uint8_t* g11c2f4;           // 0x11C2F4 / 0x11C2F8: written by the player-death path
    const uint32_t* g11bbfc;    // 0x11BBFC: the player unit
    uint32_t mod;               // module base (retail 0x6FAB0000)
};
static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline uint32_t rd16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline uint32_t gd(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

template <class... T> static uint32_t S(const Svc& s, uint32_t rva, T... v) { const uint32_t w[] = { (uint32_t)v... }; return s.st(rva, sizeof...(T), w); }
static uint32_t R(const Svc& s, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi,
                  uint32_t n = 0, const uint32_t* w = nullptr) {
    const uint32_t r[6] = { eax, ecx, edx, ebx, esi, edi }; return s.rg(rva, r, n, w);
}
template <class... T> static uint32_t RS(const Svc& s, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx,
                                         uint32_t esi, uint32_t edi, T... v) {
    const uint32_t w[] = { (uint32_t)v... }; return R(s, rva, eax, ecx, edx, ebx, esi, edi, sizeof...(T), w);
}
static void ev(const Svc& s, uint32_t code, uint32_t a) { RS(s, 0x6B300, code, 0, 0, 0, 0, 0, a, 2, 0, 0, 0, 0, 0); }
static void setMode(const Svc& s, uint32_t u, uint32_t m) { R(s, 0x2A6C0, 0, 0, 0, 0, u, m); }
static void f4DC30(const Svc& s, uint32_t u) { R(s, 0x4DC30, u, 0, 0, 0, 0, 0); }
static uint32_t ux(const Svc& s, uint32_t u) { return R(s, 0x1630, 0, u, 0, 0, 0, 0); }
static uint32_t uy(const Svc& s, uint32_t u) { return R(s, 0x1660, 0, u, 0, 0, 0, 0); }
static int32_t abs32(uint32_t v) { return (int32_t)((int32_t)v < 0 ? 0u - v : v); }   // neg: 0x80000000 stays negative

void sub_4DC70(const Svc& s, uint32_t mode, uint32_t u, uint32_t p3);

// mode-15/16 body shared tail: the class-dependent sequence (C838 -> C778 -> C7B4) and the new animation mode
static uint32_t seqMode(const Svc& s, uint32_t u, uint32_t p3) {
    const uint32_t v = R(s, 0xC838, 0, u, rd32(p3), 0, 0, 0);
    S(s, 0xC778, u, v);
    const uint32_t m = S(s, 0xC7B4, u, v);
    if (m == 0xE) R(s, 0x4D5B0, 0, 0, 0, 0, 0, u);
    wr32(p3 + 4, 0xFFFFFFFFu);
    setMode(s, u, m);
    return m;
}

// mode 8 (death): the per-class jump tables. Returns 0 for the T778 join, 1 for T781, 2 for the main tail.
static int death(const Svc& s, uint32_t u, int32_t e, uint32_t ms, uint32_t p3) {
    auto t766 = [&](uint32_t code) { ev(s, code, u); return 0; };
    if (e == 0x164) { RS(s, 0xA15C0, u, 0, 0, 0, 0, 0, 0x148, 0, 1); return 0; }
    if (e > 0x164) {
        switch (e) {
        case 0x168: case 0x22E: return t766(0xB9);
        case 0x1A1: case 0x1A2: return t766(0x10F);
        case 0x1A9: case 0x1AA: case 0x1AB: {
            const uint32_t y = uy(s, u), x = ux(s, u);
            RS(s, 0xAFF10, 0, 0, u, 0, 0, 0, 0x1D6, x, y, 0, 1);
            setMode(s, u, 0);
            RS(s, 0x88A70, 0, 0, 0, 0x316, 0, 0, u, 0, 0, 0);
            return 1;
        }
        case 0x1B9: {
            const uint32_t p2c = rd32(u + 0x2C);
            const uint32_t v = S(s, 0xC2D4, p2c) & 0xFFu;
            S(s, 0xC2B6, p2c, (v + 0x20) & 0x3F);
            return 0;
        }
        case 0x1C5: {
            const uint32_t b = S(s, 0xC610, u) & 0xFFu;
            const uint32_t k = S(s, 0xCC88, (ms & 0xFFFFFF00u) | b) & 3u;
            RS(s, 0x88A70, 0, 0, 0, 0x51C + 3 * k, 0, 0, u, 0, 0, 0);
            return 0;
        }
        case 0x1CD:
            ev(s, 0xCC, u);
            RS(s, 0x88A70, 0, 0, 0, 0x973, 0, 0, u, 0, 0, 0);
            setMode(s, u, 0);
            return 1;
        case 0x1D5: case 0x1DA: return t766(0xE5);
        case 0x1F1: {
            const uint32_t c = rd32(u + 4);
            if (c == 0x1F1 || c == 0x1F3) return t766(0xE1);
            if (c == 0x1F2) return t766(0xE2);
            return 0;
        }
        case 0x23A: {
            const uint32_t y = uy(s, u), x = ux(s, u);
            RS(s, 0xAFF10, 0, 0, u, 0, 0, 0, 0x266, x, y, 0, 1);
        }
        [[fallthrough]];
        case 0x22F:
            wr32(u + 0xC8, rd32(u + 0xC8) | 0x40000);
            wr32(u + 0xC4, rd32(u + 0xC4) | 0x20);
            setMode(s, u, 0xC);
            return 1;
        }
        return 0;
    }
    switch (e) {
    case 0x6E: {
        ev(s, 0x92, u);
        setMode(s, u, 0);
        if (!R(s, 0xC7F20, u, 0, 0, 0, 0, 0)) return 1;
        R(s, 0xC7BF0, u, 0, 0, 0, 0, 0);
        uint32_t p = rd32(u + 0x54), q;
        if (p && (q = rd32(p + 0x30))) wr32(q, rd32(q) & ~0x20u);
        p = rd32(u + 0x54);
        if (p && (q = rd32(p + 0x30))) { wr32(q + 0x10, 0); wr32(q + 0x14, 0); wr32(q + 0x18, 0xFFFF8000u); }
        p = rd32(u + 0x54);
        if (p && (q = rd32(p + 0x30))) { wr32(q + 0x28, 0); wr32(q + 0x2C, 0); wr32(q + 0x30, 0); }
        return 1;
    }
    case 0x76: return t766(0xB8);
    case 0x7F: return t766(0x4F);
    case 0xBE:
        if (rd32(u + 0x10) != 0xE) return 0;
        RS(s, 0x2A180, 0, 0, 0, 0, u, 0, 0);
        wr32(u + 0x44, 0xE00);
        return 1;
    case 0xD4: ev(s, 0xBC, u); return t766(0xBD);
    case 0xEE: return t766(0x4E);
    case 0xF7: return t766(0x52);
    case 0x102: case 0x105: return t766(0x9D);
    case 0x11C: {
        uint32_t blk[7] = { 0xD8, 0, 0, 0, 1, 0, 0 };
        sub_4DC70(s, 0x15, u, (uint32_t)(uintptr_t)blk);
        return 1;
    }
    case 0x136: case 0x137: case 0x138: return t766(0xBB);
    case 0x15F: case 0x160: case 0x161:
        R(s, 0x69610, 0, 0, 0, 0, 0, u);
        R(s, 0xA6F60, 0, rd32(u + 4) - 0xAD, 0, 0, u, 0);
        return 0;
    }
    (void)p3;
    return 0;
}

void sub_4DC70(const Svc& s, uint32_t mode, uint32_t u, uint32_t p3) {
    const uint32_t dt = (uint32_t)(uintptr_t)s.dt;
    const uint32_t p2c = rd32(u + 0x2C);
    if (rd32(u + 0x10) == 0xD) { S(s, 0xCD2A, p2c); S(s, 0xCD78, p2c); }
    bool skip = true;
    if (!(rd32(u + 0xC4) & 0x10000)) {
        const uint32_t ty = rd32(u), t = rd32(u + 0x10);
        if (ty == 0) skip = t == 0 || t == 0x11;
        else if (ty == 1) skip = t == 0 || t == 0xC;
        else skip = false;
    }
    if (!skip) S(s, 0xCD90, u);
    else if (mode != 8 && mode != 9) {
        S(s, 0xCD48, u, 0);
        R(s, 0x4C150, u, 0, 0, 0, 0, 0);
        wr32(u + 0xC8, rd32(u + 0xC8) & 0xFFFBFFFFu);
    }
    bool f24 = false;
    if (mode != 0x13 && mode != 0x16 && mode != 0x15 && p3) {
        f24 = true;
        const uint32_t a = rd32(p3 + 0xC);
        if ((int32_t)a > 0) {
            S(s, 0xC38E, p2c, a);
            if (mode == 0x14) { S(s, 0xC3B8, p2c, rd8(p3 + 0x14)); S(s, 0xC682, p2c, rd8(p3 + 8)); }
        }
        S(s, 0xC778, u, 0);
        S(s, 0x212F0, u, 0, 0);
    }
    const int32_t c = (int32_t)rd32(u + 4);
    if (c < 0) return;
    const int32_t cnt = (int32_t)rd32(dt + 0xA80);
    if (c >= cnt) return;
    const uint32_t row = (uint32_t)c * 0x1A8u + rd32(dt + 0xA78);
    if (!row) return;
    const uint32_t m2 = R(s, 0x1260, (uint32_t)c, row, 0, 0, 0, 0);
    if (!m2) return;
    const int32_t t = (int32_t)R(s, 0x1700, u, 0, 0, 0, 0, 0);
    const int32_t e = (t >= 0 && t < cnt) ? t : -1;
    const uint32_t tp = s.mod + 0xD2E68 + mode * 8;
    const uint32_t tw = mode <= 0x1D ? gd(s.tbl, mode * 8) : 0;
    const uint32_t x0 = p3 ? rd32(p3) : 0, y0 = p3 ? rd32(p3 + 4) : 0;
    int j = 2;   // 0 = T778, 1 = T781, 2 = main tail, 3 = 4DC30 then main tail
    switch (mode) {
    case 1: case 0x17:
        S(s, 0xC646, u, 1);
        if (RS(s, 0x1E40, u, 0, 0, 0, 0, 0, 1) || !R(s, 0x2A040, y0, x0, 0, 0, u, 0)) j = 3;
        else setMode(s, u, mode == 1 ? 2 : 0xF);
        break;
    case 0: case 0x18:
        S(s, 0xC646, u, 1);
        RS(s, 0xA6040, 0, y0, x0, 0, 0, 0, u);
        if (!RS(s, 0x1E40, u, 0, 0, 0, 0, 0, 1) && R(s, 0x2A070, 0, rd32(p3 + 4), x0, u, 0, 0)) setMode(s, u, mode == 0 ? 2 : 0xF);
        else j = 3;
        break;
    case 5: case 0xA: case 0xD: case 0xF: case 0x10: case 0x1B: case 0x1D: {
        S(s, 0xC646, u, 0);
        if (e == 0xE7) break;
        RS(s, 0xA6040, 0, y0, x0, 0, 0, 0, u);
        if (e == 0x130 && mode == 0xA) {
            if (const uint32_t r = R(s, 0xA5F70, 0, 0, 0, 0, u, 0)) ev(s, 0xA9, r);
        }
        RS(s, 0xA5FB0, u, 0, 0, 0, 0, 0, 0);
        if (e == 0x6E) R(s, 0x4CAD0, mode, u, 0, 0, 0, 0);
        else if (e == 0x9A && mode == 0xD) S(s, 0xC2B6, rd32(u + 0x2C), 0x38);
        if (rd32(u + 4) == 0x195 && mode == 0xD) S(s, 0xC2B6, rd32(u + 0x2C), 0x34);
        if (rd32(u + 4) == 0x195 && mode == 0xF) S(s, 0xC2B6, rd32(u + 0x2C), 0x30);
        if (e == 0x9B) { if (mode == 0xD) S(s, 0xC2B6, rd32(u + 0x2C), 0x34); }
        else if (e == 0xB2 && mode == 0xD) S(s, 0xC2B6, rd32(u + 0x2C), 4);
        setMode(s, u, tw);
        if (R(s, 0x4BFA0, tp, u, 0, 0, 0, 0)) R(s, 0x2A070, 0, rd32(p3 + 4), x0, u, 0, 0);
        break;
    }
    case 4: case 0xB: case 0xC: case 0xE: case 0x11: case 0x1A: case 0x1C:
        S(s, 0xC646, u, 0);
        S(s, 0xC62E, u, 0);
        if (rd32(p3) || rd32(p3 + 4)) {
            if (rd32(p3) != ux(s, u) || rd32(p3 + 4) != uy(s, u)) S(s, 0xC72A, u, rd32(p3), rd32(p3 + 4));
        }
        setMode(s, u, tw);
        if (R(s, 0x4BFA0, tp, u, 0, 0, 0, 0)) R(s, 0x2A040, rd32(p3 + 4), rd32(p3), 0, 0, u, 0);
        if (e == 0x6E) R(s, 0x4CAD0, mode, u, 0, 0, 0, 0);
        break;
    case 6: {
        if (!p3) break;
        RS(s, 0x1FCA0, u, 0, 0, 0, 0, 0, rd32(p3 + 8) & 0x80);
        uint32_t v = rd32(p3 + 8) & 0x7F;
        wr32(p3 + 8, v);
        if ((int32_t)v > 1) wr32(p3 + 8, v + 1);
        bool flag = true;
        if (const uint32_t a = rd32(p3 + 8)) {
            const uint32_t sv = S(s, 0xC1C6, u, 6, 0);
            if ((uint32_t)((int32_t)sv >> 8) == rd32(p3 + 8)) flag = false;
            else S(s, 0xC40C, u, 6, rd32(p3 + 8) << 8, 0);
            (void)a;
        }
        S(s, 0xC646, u, 0);
        wr32(u + 0xB0, rd32(p3 + 0x18));
        S(s, 0xCD2A, p2c);
        setMode(s, u, 3);
        if (flag) RS(s, 0x4C500, 0, u, 0, 0, 0, 0, 1);
        R(s, 0xA4B60, u, 0, 0, 0, 0, 0);
        S(s, 0x212F0, u, 0, 3);
        break;
    }
    case 0x13: {
        RS(s, 0x1FCA0, u, 0, 0, 0, 0, 0, rd32(p3 + 4) & 0x80);
        const uint32_t v = rd32(p3 + 4) & 0x7F;
        wr32(p3 + 4, v);
        if ((int32_t)v > 1) wr32(p3 + 4, v + 1);
        if (const uint32_t a = rd32(p3 + 4)) S(s, 0xC40C, u, 6, a << 8, 0);
        wr32(u + 0xB0, rd32(p3));
        RS(s, 0x4C500, 1, u, 0, 0, 0, 0, 1);
        R(s, 0xA4B60, u, 0, 0, 0, 0, 0);
        R(s, 0x2A590, 0, 0xD, u, 0, 0, 0);
        RS(s, 0x24490, u, 0, 0, 0, 0, 0, 0xD, 1);
        S(s, 0x212F0, u, 0, 3);
        break;
    }
    case 0x14:
        if (const uint32_t a = rd32(p3 + 0x14)) S(s, 0xC40C, u, 6, a << 8, 0);
        wr32(u + 0xB0, rd32(p3 + 0x18));
        S(s, 0xC646, u, 0);
        S(s, 0xCD2A, p2c);
        S(s, 0xC38E, p2c, 0xB);
        S(s, 0xCD36, p2c, rd8(p3 + 8));
        S(s, 0xC3EE, p2c, rd16(p3), rd16(p3 + 4));
        S(s, 0xC376, p2c, u, 0);
        setMode(s, u, 0xD);
        break;
    case 0x12:
        S(s, 0xC646, u, 0);
        S(s, 0xCD2A, p2c);
        RS(s, 0xA6040, 0, y0, x0, 0, 0, 0, u);
        RS(s, 0xA5FB0, u, 0, 0, 0, 0, 0, 0);
        setMode(s, u, 6);
        break;
    case 8: {
        S(s, 0xC646, u, 0);
        wr32(u + 0xB0, rd32(p3 + 0x18));
        if (R(s, 0x51A80, 0, 0, 0, 0, 0, 0) == u) {
            if ((s.f46c4[0] & 8) || (s.f46c4[4] & 8)) { R(s, 0x42AD0, 0, 0, 0, 0, 0, 0); std::memcpy(s.g11c2f4 + 4, "\1\0\0\0", 4); }
            else std::memset(s.g11c2f4 + 4, 0, 4);
            std::memset(s.g11c2f4, 0, 4);
        }
        S(s, 0xC40C, u, 6, 0, 0);
        if (S(s, 0xCBB0, u)) {
            wr32(u + 0xC8, rd32(u + 0xC8) | 0x40000);
            wr32(u + 0xC4, (rd32(u + 0xC4) & ~2u) | 0x20);
        }
        if (S(s, 0xCDB4, u)) {
            R(s, 0x4BEC0, 0, 0, 0, 0, u, 0);
            wr32(u + 0xB0, (rd32(u + 0xB0) & 0xF) | 0xA0);
        }
        if (!(s.bm[0xC] & rd8(m2 + 6))) S(s, 0xCD12, u, 0);
        const uint32_t ms = R(s, 0x1230, rd32(u + 4), 0, 0, 0, 0, 0);
        if (!ms) return;
        if (rd8(ms + 0x1A4) == 1) {
            const int32_t k = (int16_t)rd16(ms + 0x26);
            if (k >= 0 && k < (int32_t)rd32(dt + 0xA80)) {
                const uint32_t y = uy(s, u), x = ux(s, u);
                const uint32_t r = RS(s, 0xA7BF0, 0, rd32(u + 4), x, 0, 0, 0, y, 1, 0);
                if (r) wr32(r + 0xC4, rd32(r + 0xC4) & ~2u);
                wr32(r + 0xB0, rd32(u + 0xB0));
                setMode(s, r, 0);
                RS(s, 0x4D610, u, 0, 0, 0, 0, 0, (uint32_t)(int32_t)(int16_t)rd16(ms + 0x26));
            }
        }
        j = death(s, u, e, ms, p3);
        break;
    }
    case 9: {
        S(s, 0xC646, u, 0);
        const uint32_t pl = *s.g11bbfc;
        if (!(s.bm[0xC] & rd8(m2 + 6))) S(s, 0xCD12, u, 0);
        S(s, 0xC40C, u, 6, 0, 0);
        if (R(s, 0xA5F70, 0, 0, 0, 0, pl, 0) == u) S(s, 0xC62E, pl, 0);
        if (e == 0x11C && rd32(u + 0x10) == 0xE) S(s, 0xC778, u, R(s, 0xC838, 0, u, 0xD8, 0, 0, 0));
        else { setMode(s, u, 0xC); wr32(u + 0xC4, rd32(u + 0xC4) | 0x10000); }
        if (rd32(u + 4) != 0xD3 && e != 0x1F1 && e != 0x1A1 && e != 0x1A2) R(s, 0x69610, 0, 0, 0, 0, 0, u);
        if (e == 0x1B3) {
            const uint32_t y = uy(s, u), x = ux(s, u);
            if (const uint32_t r = RS(s, 0xAFF10, 0, 0, u, 0, 0, 0, 0x1A8, x, y, 0, 1)) wr32(r + 0xC4, rd32(r + 0xC4) | 0x10000);
        }
        if (e == 0x158 || e == 0x22F || e == 0x23A) wr32(u + 0xC8, rd32(u + 0xC8) | 0x40000);
        wr32(u + 0xC4, (rd32(u + 0xC4) & ~2u) | 0x20);
        if (S(s, 0xCBB0, u)) {
            wr32(u + 0xC8, rd32(u + 0xC8) | 0x40000);
            wr32(u + 0xC4, (rd32(u + 0xC4) & ~4u) | 0x20);
        }
        R(s, 0xA5420, 0, 0, 0, 0, 0, u);
        break;
    }
    case 7: {
        S(s, 0xC646, u, 0);
        if (p3) { if (const uint32_t a = rd32(p3 + 8)) S(s, 0xC40C, u, 6, a << 8, 0); }
        const uint32_t f = rd32(u + 0xC4);
        if ((f >> 9) & 1) wr32(u + 0xC4, f | 2);
        else R(s, 0x13D0, u, 2, rd8(m2 + 4) & gd(s.bm, 0xC), 0, 0, 0);
        R(s, 0x13D0, u, 0x20, (rd8(m2 + 5) & s.bm[0x18]) ? 0u : 1u, 0, 0, 0);
        const uint32_t r = R(s, 0xA6210, 0, 0, 0, 0, u, 0);
        if (!p3) { j = 3; break; }
        RS(s, 0x2A1B0, (r & 0xFFFF0000u) | rd16(p3), rd16(p3 + 4), 0, 0, 0, 0, rd16(p3 + 4), 0, 0, 0);
        if (abs32(ux(s, u) - rd32(p3)) <= 1 && abs32(uy(s, u) - rd32(p3 + 4)) <= 1) { j = 3; break; }
        if (RS(s, 0x1E40, u, 0, 0, 0, 0, 0, 1) || !R(s, 0x2A040, rd32(p3 + 4), rd32(p3), 0, 0, u, 0) ||
            S(s, 0xC1FC, u, 0x8F) || !R(s, 0x1C10, rd32(u + 4), 0, 2, 0, 0, 0)) j = 3;
        else setMode(s, u, 2);
        break;
    }
    case 0x16:
        S(s, 0xC646, u, 0);
        S(s, 0xC550, u, rd32(p3), rd32(p3 + 0x10), 0, s.mod + 0xD6030, 0x913);
        seqMode(s, u, p3);
        R(s, 0xA30C0, 0, u, p3, 0, 0, 0);
        break;
    case 0x15:
        S(s, 0xC646, u, 0);
        S(s, 0xC62E, u, 0);
        S(s, 0xC550, u, rd32(p3), rd32(p3 + 0x10), 0, s.mod + 0xD6030, 0x928);
        seqMode(s, u, p3);
        RS(s, 0xA22B0, u, 0, 0, 0, 0, 0, p3);
        break;
    default:
        S(s, 0xC646, u, 0);
        j = 3;
        break;
    }
    if (j == 0) { setMode(s, u, 0); j = 1; }
    if (j == 1) { wr32(u + 0xC4, rd32(u + 0xC4) & ~2u); R(s, 0xA5420, 0, 0, 0, 0, 0, u); }
    if (j == 3) f4DC30(s, u);
    if (f24 && p3 && rd32(p3 + 0x10)) {
        const uint32_t sv = S(s, 0xC1C6, u, 0x43, 0);
        if (sv != rd32(p3 + 0x10)) {
            S(s, 0xCD4E, u);
            S(s, 0xC4F6, u, 0x43, rd32(p3 + 0x10) - sv, 0);
            S(s, 0xC5C8, u, s.mod + 0xD6030, 0x945);
        }
    }
    S(s, 0x212F0, u, 0, 1);
    if (rd32(u) == 1) {
        const uint32_t pd = rd32(u + 0x14);
        if (pd && (rd8(pd + 0x28) & 1)) {
            if (rd32(u + 0x10) != 1) f4DC30(s, u);
            if (p2c) S(s, 0xCDD8, p2c);
        }
    }
}
}
