// 1.13c D2Client+0xA1CF0: apply a skill-table row to a unit. EAX = unit, stack id, s1, s2 (ret 0xC). Env-seam style: `h` is the call into a retail callee
// at the given D2Client RVA with the register and stack words its call site loads (a c d b s = eax ecx edx ebx esi; s0.. = stack words from the top).
#include <cstdint>
#include <cstring>
struct D2cA1CF0Env {
    void* c;
    const uint8_t* dt;        // *(*(D2Client+0xCE474)): sgptDataTables
    const uint8_t* tbl;       // *(D2Client+0xCEE50): the bitmask table
    uint32_t lp;              // *(D2Client+0x11BBFC): local player
    int32_t cbCount;          // *(D2Client+0xDEA40)
    const uint32_t* cbTab;    // D2Client+0xDE928
    uint32_t (*h)(void* c, uint32_t rva, uint32_t a, uint32_t cx, uint32_t d, uint32_t b, uint32_t s,
                  uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
    void (*ed)(void* c, uint32_t edi);                       // EDI for the next call (an out-pointer)
    void (*edi)(void* c, uint32_t rva, uint32_t edi);        // EDI as an argument of the call just made
    void (*blk)(void* c, uint32_t rva, const uint32_t* p, uint32_t n);
    uint32_t (*cb)(void* c, uint32_t fn, uint32_t player, uint32_t id, uint32_t s2);   // table entry: ECX = unit, EDX = id, stack s2
};
static inline uint32_t a1cf0_rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static inline int32_t a1cf0_w(const uint8_t* r, uint32_t o) { int16_t v; std::memcpy(&v, r + o, 2); return v; }
static inline uint32_t a1cf0_p(const void* p) { return (uint32_t)(uintptr_t)p; }
// 1.13c D2Client+0xA1CF0
extern "C" uint32_t d2c_a1cf0(const D2cA1CF0Env& e, uint32_t player, int32_t id, uint32_t s1, uint32_t s2) {
    void* c = e.c;
    auto K = [&](uint32_t rva, uint32_t a = 0, uint32_t cx = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t w0 = 0, uint32_t w1 = 0, uint32_t w2 = 0,
                 uint32_t w3 = 0, uint32_t w4 = 0, uint32_t w5 = 0, uint32_t w6 = 0) { return e.h(c, rva, a, cx, d, b, s, w0, w1, w2, w3, w4, w5, w6); };
    const uint32_t STR = 0x6FB844B4;
    if (id < 0) return 0;
    if (id >= (int32_t)a1cf0_rd(a1cf0_p(e.dt), 0xBA0)) return 0;
    const uint32_t rowp = a1cf0_rd(a1cf0_p(e.dt), 0xB98) + (uint32_t)id * 0x23C;
    if (!rowp) return 0;
    const uint8_t* row = (const uint8_t*)(uintptr_t)rowp;
    const uint32_t tbl = a1cf0_p(e.tbl);
    auto tb = [&](uint32_t o) { return *(const uint8_t*)(uintptr_t)(tbl + o); };
    uint32_t ebx = 0;
    if (row[0x18F]) {
        uint32_t W = 0, Hh = 0;
        e.ed(c, a1cf0_p(&W));
        if (K(0xA1340, 0, 0, 0, player, 0, a1cf0_p(&Hh))) {
            static const uint32_t kv[5] = { 4, 0x1C09, 0x180, 0x804, 0x805 };
            const uint32_t sel = (uint32_t)row[0x18F] - 1;
            if (sel > 4) return 0;
            if (!K(0xC7A2, 0, 0, 0, 0, 0, W, Hh, player, kv[sel])) return 0;
        }
    }
    { const uint32_t r = K(0xA11C0, (uint32_t)id); e.edi(c, 0xA11C0, player); if (!r) return 0; }
    ebx = K(0xC310, 0, 0, 0, 0, 0, player);
    if (!ebx || (int32_t)K(0xC42A, 0, 0, 0, 0, 0, ebx, STR, 0x504) != id) {
        ebx = K(0xC4EA, 0, player, (uint32_t)id, 0, 0, s1);
        if (!ebx) {
            K(0xC742, 0, 0, 0, 0, 0, player, (uint32_t)id);
            const uint32_t keep = s1;
            ebx = K(0xC4EA, 0, player, (uint32_t)id, 0, 0, keep);
            if (ebx && player == e.lp && keep == 0xFFFFFFFFu) K(0xC60A, 0, 0, 0, 0, 0, ebx, 8);
            const uint32_t n = K(0x17B0, (uint32_t)id);
            if ((int32_t)s2 > (int32_t)n) {
                K(0xC550, 0, 0, 0, 0, 0, player, (uint32_t)id, n, 0, STR, 0x513);
                K(0xC71E, 0, 0, 0, 0, 0, player, (uint32_t)id, s2 - n);
            } else K(0xC550, 0, 0, 0, 0, 0, player, (uint32_t)id, s2, 0, STR, 0x518);
            if (!ebx) return 0;
        }
        K(0xC778, 0, 0, 0, 0, 0, player, ebx);
    }
    if (s1 != 0xFFFFFFFFu) {
        uint32_t A = 0, B = 0, C = 0;
        const uint32_t r = K(0xC766, 0, 0, 0, 0, 0, ebx, a1cf0_p(&C), a1cf0_p(&B), a1cf0_p(&s2), a1cf0_p(&A));
        if (!r || !A) return 0;
    }
    if (player != e.lp) {
        const uint32_t n = K(0x17B0, (uint32_t)id);
        if ((int32_t)s2 > (int32_t)n) {
            K(0xC550, 0, 0, 0, 0, 0, player, (uint32_t)id, n, 0, STR, 0x53A);
            K(0xC71E, 0, 0, 0, 0, 0, player, (uint32_t)id, s2 - n);
        } else K(0xC550, 0, 0, 0, 0, 0, player, (uint32_t)id, s2, 0, STR, 0x53F);
    } else s2 = K(0xC718, 0, 0, 0, 0, 0, player, ebx, 1);
    {
        const uint32_t v = K(0xA5F70);
        if (v && !(tb(8) & row[7])) { const uint32_t r = K(0xA5B90); e.edi(c, 0xA5B90, v); if (r) return 0; }
    }
    if (!(tb(0) & row[5])) {
        if (K(0xC208, 0, 0, 0, 0, 0, K(0xC1F0, 0, 0, 0, 0, 0, player))) {
            K(0xC778, 0, 0, 0, 0, 0, player, 0);
            uint32_t z[7] = {};
            K(0x2A750, 0, 7, player, 0, 0, a1cf0_p(z), 0); e.blk(c, 0x2A750, z, 7);
            if (player == e.lp) K(0x23200, 0, 0, 0, 0, 0, player, 0x18);
            return 0;
        }
    }
    {
        uint32_t r;
        if (player && a1cf0_rd(player, 0) == 0 && id == 0) r = K(0xA1260);
        else r = K(0xC7B4, 0, 0, 0, 0, 0, player, ebx);
        if (r) { K(0x2A6C0); e.edi(c, 0x2A6C0, r); }
        else if (!(K(0xC1FC, 0, 0, 0, 0, 0, player, 0xC) && (tb(0xC) & row[5])) && player) {
            const uint32_t t = a1cf0_rd(player, 0);
            if (t == 0 || t == 1) { K(0x2A6C0); e.edi(c, 0x2A6C0, 1); }
        }
    }
    uint32_t ret = 1;
    {
        const int32_t ix = a1cf0_w(row, 0xF2);
        if (ix >= 0 && ix < e.cbCount && e.cbTab[ix]) {
            ret = e.cb(c, e.cbTab[ix], player, (uint32_t)id, s2);
            if (!ret) {
                K(0xC778, 0, 0, 0, 0, 0, player, 0);
                if (player) { const uint32_t t = a1cf0_rd(player, 0); if (t == 0 || t == 1) { K(0x2A6C0); e.edi(c, 0x2A6C0, 1); } }
                if (tb(0x10) & row[5]) return ret;
            }
        }
    }
    auto lvl = [&]() {   // 0x88A70's second stack word: unit level minus 3 floored at 0, only with the bit-0x14 flag
        if (!(tb(0x14) & row[5])) return 0u;
        K(0xC7A8, 0, player);
        const int32_t l = (int32_t)K(0x22220, 0, player) - 3;
        return l < 0 ? 0u : (uint32_t)l;
    };
    int32_t di = a1cf0_w(row, 0xFC);
    if (s1 != 0xFFFFFFFFu && a1cf0_w(row, 0x122) > 0) di = a1cf0_w(row, 0x122);
    if ((int16_t)di > 0) { const uint32_t v = lvl(); K(0x88A70, 0, 0, 0, (uint32_t)(int32_t)(int16_t)di, 0, player, v, 0, 0); }
    if (tb(0x18) & row[5]) {
        K(0xC7A8, 0, player);
        const uint32_t q = K(0x22280, 0, player);
        if (q) {
            K(0xC7A8, 0, player);
            const int32_t l = (int32_t)K(0x22220, 0, player);
            K(0x88A70, 0, 0, 0, q, 0, player, l < 0 ? 0u : (uint32_t)l, 0, 0);
        }
    }
    di = a1cf0_w(row, 0xFE);
    if ((s1 == 0xFFFFFFFFu || a1cf0_w(row, 0x122) <= 0) && (int16_t)di > 0 && player && a1cf0_rd(player, 0) == 0 && a1cf0_rd(player, 4) == row[0xC]) {
        const uint32_t v = lvl(); K(0x88A70, 0, 0, 0, (uint32_t)(int32_t)(int16_t)di, 0, player, v, 0, 0);
    }
    int32_t b = a1cf0_w(row, 0x106);
    if (s1 != 0xFFFFFFFFu && a1cf0_w(row, 0x124) > 0) b = a1cf0_w(row, 0x124);
    if (b > 0 && b < (int32_t)a1cf0_rd(a1cf0_p(e.dt), 0xBC0)) {
        if (!K(0x69130, player, (uint32_t)b)) K(0x6B300, (uint32_t)b, 0, 0, 0, 0, player, 2, 0, 0, 0, 0, 0);
    }
    if (ret && a1cf0_w(row, 0xE8) > 0) K(0xA0F60, (uint32_t)a1cf0_w(row, 0xE8), player);
    return ret;
}
