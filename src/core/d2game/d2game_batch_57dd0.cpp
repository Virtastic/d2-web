// src/core/d2game/d2game_batch_57dd0.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Stage A batch lane: implementation +
// marker only, no spec, no vector, no test runner. They are the monster-AI mode handlers around the
// 0x57DD0..0x59A5A block (skill-state/mode transitions keyed on a unit's stat list and its type-0x20/
// 0x22/0x24 child nodes) plus the 0x7F9A0 chance-roll branch.
//
// Retail field offsets are kept verbatim; pointers are read with the host pointer width. Register-only
// arguments Ghidra shows as in_EAX / unaff_EBX are explicit parameters (`unit` / `reg`). Callees outside
// the batch are seams on D2GameBatch57dd0Env in retail argument order. Ordinals 10045/10141 are not used.
// Stack garbage in the packet frames is zeroed. Base 0x6FC20000.
#include <cstdint>
#include <cstring>
#include <unistd.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline int16_t  rds16(const void* p, uint32_t o) { return (int16_t)rd16(p, o); }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

static const char kModuleName[] = "D2Game.dll";  // 0x6FD19FF9

struct D2GameBatch57dd0Env {
    void*    (*ord10920)(void* unit);                          // Ordinal_10920 -> record (+0x10 table, +0x9C)
    int32_t  (*ord10174)(uint32_t a, uint32_t b, uint32_t c);  // Ordinal_10174
    void     (*ord10003)(uint32_t a, uint32_t b, uint32_t c);  // Ordinal_10003
    void     (*ord10003_v)(void);                              // Ordinal_10003, args in registers (0x58BE0)
    int32_t  (*sub2E660)(uint32_t tag);                        // 0x2E660
    void     (*sub33830)(void* p);                             // 0x33830
    void     (*sub33EB0)(void* ai, uint32_t a, uint32_t b);    // 0x33EB0
    void     (*sub354F0)(void* a, void* b);                    // 0x354F0
    void     (*sub8A3E0)(const void* pkt, uint32_t len);       // 0x8A3E0
    void*    (*ord10331)(void* unit);                          // Ordinal_10331
    void*    (*ord10826)(void* p);                             // Ordinal_10826
    int8_t   (*ord10459)(void* p);                             // Ordinal_10459
    uint32_t (*ord10265)(uint32_t id);                         // Ordinal_10265
    void     (*ord10024)(const char* module, uint32_t id);     // Ordinal_10024, tail is _exit(-1)
    void     (*ord10158)(void* unit);                          // Ordinal_10158
    void     (*ord11103)(void* unit, uint32_t a);              // Ordinal_11103
    void     (*sub21320)(void);                                // 0x21320
    void     (*sub95E10)(void* unit, void* frame);             // 0x95E10
    void     (*subF2540)(void* unit, void* m, void* a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);  // 0xF2540
    void     (*subF22C0)(int32_t v);                           // 0xF22C0
    int32_t  (*ord10826i)(void* p);                            // Ordinal_10826 as an int compare (0x599E0)
};
static D2GameBatch57dd0Env g;

// First node of the unit's child list (unit+0x10F4 -> head) whose type word equals `type`.
static void* findNode(const void* unit, uint32_t type) {
    void* l = rdptr(unit, 0x10f4);
    if (!l) return nullptr;
    for (void* n = rdptr(l, 0); n; n = rdptr(n, 0xf4)) if (rd32(n, 0) == type) return n;
    return nullptr;
}
// record->+0x10 + rd8(rec, 0x6d)*4 : the per-player stat-list handle.
static uint32_t statHandle(const void* tbl, const void* owner) { return rd32(tbl, 0x10 + rd8(owner, 0x6d) * 4); }
// (rec+10 & 0x20 | 0x80) >> 5 clamp of the 5-bit level field at rec+10 bits 8..12.
static void clampLevel(void* rec, uint8_t lvl) {
    uint32_t u = (rd16(rec, 10) & 0x20 | 0x80) >> 5;
    if ((rd16(rec, 10) & 0x1f00u) >> 8 <= u * lvl + 5)
        wr16(rec, 10, (uint16_t)((uint16_t)(u * lvl + 5) * 0x100 | (rd16(rec, 10) & 0xe0ff)));
}

// 1.13c D2Game+0x57DD0
void d2game_57dd0(void* ai, int32_t* p2) {
    if (g.sub2E660(0x20656369)) wr32(rdptr(ai, 0x18), 0xd8, rd32(rdptr(ai, 0x18), 0xd8) + 1);
    if (rd8(ai, 9) == 0 || rd8(ai, 0xc) > 4) {
        void* l = rdptr(rdptr(ai, 4), 0x10f4);
        void* n = l ? rdptr(l, 0) : nullptr;
        if (!n) return;
        while (rd32(n, 0) != 0x22) { n = rdptr(n, 0xf4); if (!n) return; }
        if (rd8(n, 9)) {
            void* tbl = g.ord10920(rdptr(p2, 12));
            uint32_t h = statHandle(tbl, rdptr(p2, 0));
            if (!g.ord10174(h, 0x25, 1) && !g.ord10174(h, 0x25, 6) && !g.ord10174(h, 0x25, 0))
                g.ord10003(h, 0x26, 0xe);
        }
    }
}

// 1.13c D2Game+0x57E80
void d2game_57e80(void* ai, void* p2) {
    void* st = rdptr(ai, 0x18);
    if (g.sub2E660(0x20656369)) wr32(st, 0xd8, 1);
    uint32_t h = statHandle(g.ord10920(rdptr(p2, 12)), rdptr(p2, 0));
    uint32_t e0 = rd32(ai, 0xe0);
    if (g.ord10174(h, e0, 0) || g.ord10174(h, e0, 0xf)) { wr32(st, 0x84, 2); wr32(st, 0x88, 1); return; }
    if (rd8(ai, 9) == 0) return;
    if (g.ord10174(h, e0, 3)) { wr8(ai, 0xc, 3); wr8(ai, 0xb, 1); }
    else if (g.ord10174(h, e0, 2)) { wr8(ai, 0xc, 2); wr8(ai, 0xb, 1); }
    if (rd32(st, 0xd8) == 0) return;
    wr8(ai, 0xc, 3); wr8(ai, 0xb, 4);
}

// 1.13c D2Game+0x58040
void d2game_58040(void* ai) {
    g.sub33830((uint8_t*)ai + 0x1c);
    void* st = rdptr(ai, 0x18);
    g.sub33830(st);
    if (g.sub2E660(0x20656369)) wr32(st, 0xd8, rd32(st, 0xd8) - 1);
}

// 1.13c D2Game+0x582E0
void d2game_582e0(void* unit, void* p2) {
    uint32_t h = statHandle(g.ord10920(p2), unit);
    if (g.ord10174(h, 0x24, 0) == 1 || g.ord10174(h, 0x24, 1) == 1) return;
    void* a = g.ord10331(p2);
    if (!a) return;
    void* b = g.ord10826(a);
    if (!b || g.ord10459(b) != 4) return;
    g.ord10003(h, 0x24, 0xd);
    g.ord10003(h, 0x24, 1);
    void* n = findNode(unit, 0x20);
    uint32_t v = rd32(rdptr(n, 0x18), 0xac);
    g.ord10003(h, 0x24, v == 0xf ? 5 : v == 0xe ? 6 : 7);
}

// 1.13c D2Game+0x583C0   (table = DAT_6fd2bc20, 12 dwords indexed by mode)
void d2game_583c0(void* ai, void* p2, const uint32_t* modeTable) {
    uint32_t h = statHandle(g.ord10920(rdptr(p2, 0xc)), rdptr(ai, 4));
    uint32_t e0 = rd32(ai, 0xe0);
    g.ord10174(h, e0, 0);
    int32_t r1 = g.ord10174(h, e0, 0);  // second call: result in a register, kept as iVar3
    if (g.ord10174(h, e0, 1)) { g.sub33EB0(ai, rd32(p2, 0x14), h); return; }
    void* t = rdptr(p2, 0xc);
    int32_t id = t ? (int32_t)rd32(t, 0xc) : -1;
    for (int i = 0; i < (int)rd16(ai, 0x9c); i++)
        if ((int32_t)rd32(ai, 0x1c + i * 4) == id) { g.sub33EB0(ai, rd32(p2, 0x14), h); return; }
    if (r1 != 1 && rd8(ai, 9)) {
        uint32_t m = modeTable[rd8(ai, 0xc)];
        if (m == 2 && h == 0x203) {
            if (g.ord10174(0x203, 0x24, 4)) { g.sub33EB0(ai, rd32(p2, 0x14), 0x203); return; }
        } else if (m == 0xffffffffu || m > 0xb) return;
        g.sub33EB0(ai, rd32(p2, 0x14), h);
    }
}

// 1.13c D2Game+0x58930
void d2game_58930(void* ai, void* p2) {
    uint32_t h = statHandle(g.ord10920(rdptr(p2, 12)), rdptr(p2, 0));
    uint32_t e0 = rd32(ai, 0xe0);
    if (!g.ord10174(h, e0, 0) && !g.ord10174(h, e0, 0xf)) {
        if (g.ord10174(h, e0, 1) == 1) return;
        if (g.ord10174(h, e0, 3) != 1) {
            if (g.ord10174(h, e0, 2) != 1) return;
            wr8(rdptr(ai, 0x18), 0x90, 1);
        }
        wr8(ai, 0xb, 1); wr8(ai, 0xc, 3);
        return;
    }
    g.ord10003(h, e0, 10);
    void* o = rdptr(p2, 12);
    void* rec = nullptr;
    if (o && rd32(o, 0) == 0) rec = rdptr(g.ord10920(o), 0x9c);
    clampLevel(rec, rd8(rdptr(ai, 4), 0x6d));
}

// 1.13c D2Game+0x58A40   (in_EAX = target unit, param_1 = owner)
void d2game_58a40(void* owner, void* target) {
    if (target && rd32(target, 0) == 0) g.ord10920(target);
    void* n = findNode(owner, 0x24);
    uint16_t w = rd16(rdptr(n, 0x18), 0xa4);
    uint8_t pkt[8] = {0x50, 0x24, 0, (uint8_t)w, (uint8_t)(w >> 8), 0, 0, 0};
    g.sub8A3E0(pkt, 0xf);
}

// 1.13c D2Game+0x58AE0
uint32_t d2game_58ae0(void* unit, void* p2) {
    void* tbl = g.ord10920(p2);
    if (g.ord10174(statHandle(tbl, unit), 0x28, 0xd)) {
        if (p2 && rd32(p2, 0) == 0) g.ord10920(p2);
        uint8_t pkt[6] = {0x5d, 0x24, 0x02, 0, 0, 0};
        g.sub8A3E0(pkt, 6);
        if (!p2) { g.ord10024(kModuleName, g.ord10265(0x558)); _exit(-1); }
        wr16(p2, 0x6e, 0x53);
        wrptr(p2, 0x70, p2);
        g.ord10158(p2);
        wr32(p2, 0xc4, rd32(p2, 0xc4) | 0x400);
    }
    return 0;
}

// 1.13c D2Game+0x58BE0   (reg = unaff_EBX unit)
void d2game_58be0(void* p1, void* reg) {
    g.ord10003_v();
    g.ord10003_v();
    void* rec = nullptr;
    if (p1 && rd32(p1, 0) == 0) rec = rdptr(g.ord10920(p1), 0x9c);
    clampLevel(rec, rd8(reg, 0x6d));
    void* n = findNode(reg, 0x24);
    if (n) { void* s = rdptr(n, 0x18); wr32(s, 0x88, rd32(s, 0x88) + 1); }
}

// 1.13c D2Game+0x590C0
uint32_t d2game_590c0(void* unit, void* p2) {
    uint32_t h = statHandle(g.ord10920(p2), unit);
    if ((g.ord10174(h, 0x26, 0) || g.ord10174(h, 0x26, 0xf)) || g.ord10174(h, 0x26, 0xd) || g.ord10174(h, 0x26, 0xe))
        g.sub354F0(p2, unit);
    return 0;
}

// 1.13c D2Game+0x599E0
uint32_t d2game_599e0(void* unit, void* p2) {
    uint32_t h0 = statHandle(g.ord10920(p2), unit);
    if (g.ord10174(h0, 0x28, 0) == 1) return 0;
    void* a = g.ord10331(p2);
    if (!a) return 0;
    uint32_t h = statHandle(g.ord10920(p2), unit);
    if (g.ord10826i(a) == 0x84 && !g.ord10174(h, 0x28, 0) && !g.ord10174(h, 0x28, 1))
        d2game_58be0(p2, unit);
    return 0;
}

// 1.13c D2Game+0x7F9A0
void d2game_7f9a0(void* unit, void* m, void* p3) {
    void* a = rdptr(p3, 8);
    void* st = rdptr(p3, 0);
    void* tbl = rdptr(p3, 28);
    uint8_t lv = rd8(unit, 0x6d);
    auto roll = [&]() {
        uint64_t s = (uint64_t)rd32(m, 0x20) * 0x6ac690c5u + rd32(m, 0x24);
        wr64(m, 0x20, s);
        return (int32_t)((uint32_t)s % 100);
    };
    if (rd32(p3, 24) == 0) {
        wr32(st, 0x14, 0);
        if (roll() < rds16(tbl, 0x62 + lv * 2)) { g.subF2540(unit, m, a, 2, 0, 0, 1); return; }
        g.subF22C0(rds16(tbl, 0x68 + lv * 2));
        return;
    }
    if (rd32(st, 0x14) == 0 && roll() < rds16(tbl, 0x56 + lv * 2)) {
        wr32(st, 0x14, 1);
        g.ord11103(m, 0);
        if (m) g.sub21320();
        uint8_t frame[64] = {0};  // stack0xffffffd8: caller-frame scratch, zeroed for replay
        g.sub95E10(unit, frame);
        return;
    }
    wr32(st, 0x14, 0);
    g.subF22C0(rds16(tbl, 0x5c + lv * 2));
}
