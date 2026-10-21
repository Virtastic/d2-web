// src/core/d2game/d2game_batch_58f90.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (0x<RVA>.c), RVA order. Batch lane: implementation + marker only.
// 32-bit record offsets are byte offsets; register-only args (in_EAX / unaff_EDI) are explicit params.
// Callees outside the batch are seams on D2GameBatch58F90Env at their retail address. 0x5A160 walks
// stack words filled by Ordinal_10383 in retail; modelled as the seam returning up to 8 pointers.
#include <cstdint>
#include <cstring>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }

struct D2GameBatch58F90Env {
    void*   (*ordinal10920)(void* p);                       // Ordinal_10920
    uint32_t(*ordinal10174)(uint32_t a, uint32_t b, uint32_t c); // Ordinal_10174
    void    (*sub354F0)(void* a, void* b);                  // 0x354F0 (FUN_6fc554f0)
    void    (*subA3E0)(void* buf, uint32_t n);              // 0x8A3E0 (FUN_6fcaa3e0)
    void    (*subDFCC0)(void* node, uint32_t z);            // 0xDFCC0 (FUN_6fcffcc0)
    void*   (*sub4AD10)(uint32_t z);                        // 0x4AD10
    void*   (*ordinal10331)(void* p);                       // Ordinal_10331
    uint32_t(*ordinal10383)(void* p, void** out);           // Ordinal_10383 (fills out[])
    int32_t (*sub58260)(void* node);                        // 0x58260
    void    (*sub58A40)(void* p);                           // 0x58A40
    void    (*sub33830)(void* p);                           // 0x33830
    void    (*sub35EB0)(void);                              // 0x35EB0
    void    (*sub58D40)(void);                              // 0x58D40 (passed as FUN_6fc78d40)
};
extern D2GameBatch58F90Env g_d2game58F90Env;
#define E g_d2game58F90Env

static uint32_t questHandler(int quest, void* p1, void* p2) {
    void* r = E.ordinal10920(p2);
    uint32_t u = rd32(r, 0x10 + rd8(p1, 0x6d) * 4);
    if ((E.ordinal10174(u, quest, 0) != 0 || E.ordinal10174(u, quest, 0xf) != 0) &&
        E.ordinal10174(u, quest, 0xd) == 0 && E.ordinal10174(u, quest, 0xe) == 0) return 0;
    E.sub354F0(p2, p1);
    return 0;
}

// 1.13c D2Game+0x58F90
uint32_t d2game_58F90(void* p1, void* p2) { return questHandler(0x27, p1, p2); }

// 1.13c D2Game+0x59180
void d2game_59180(int32_t* p1) {
    uint8_t buf[6] = { 0x5d, 0x21, 0x20, 0, 0, 0 };  // 0x20215d, then word 0
    if (p1 && *p1 == 0) E.ordinal10920(p1);
    E.subA3E0(buf, 6);
}

// 1.13c D2Game+0x591D0
uint32_t d2game_591D0(void* p1, void* p2) { return questHandler(0x25, p1, p2); }

// 1.13c D2Game+0x593F0
uint32_t d2game_593F0(void* p1, void* p2) { return questHandler(0x24, p1, p2); }

// 1.13c D2Game+0x59620
uint32_t d2game_59620(void* p1, void* p2) { return questHandler(0x23, p1, p2); }

static void* findNode(void* game, uint32_t type) {
    void* head = rdptr(game, 0x10f4);
    if (!head) return nullptr;
    for (void* n = rdptr(head, 0); n; n = rdptr(n, 0xf4))
        if (rd32(n, 0) == type) return n;
    return nullptr;
}

// 1.13c D2Game+0x59C50  (in_EAX = game)
void d2game_59C50(void* game) {
    void* n = findNode(game, 0x22);
    if (!n || rd8(n, 9) == 0 || rd8(n, 0xb) > 2) return;
    wr8(n, 0x14, 0); wr8(n, 0xb, 3);
    E.subDFCC0(n, 0);
}

// 1.13c D2Game+0x59F80  (in_EAX = game)
void d2game_59F80(void* game) {
    void* n = findNode(game, 0x21);
    if (!n || rd8(n, 9) == 0 || rd8(n, 0xb) > 1) return;
    wr8(n, 0x14, 0); wr8(n, 0xb, 2);
    E.subDFCC0(n, 0);
}

// 1.13c D2Game+0x5A160  (this = ecx, in_EAX = eax, param_1)
uint32_t d2game_5A160(void* self, void* inEax, void* param1) {
    void* u = inEax;
    if (!u && (rd8(self, 0x6a) != 3 || rd32(self, 0x88) == 0 || !(u = E.sub4AD10(0)))) u = param1;
    void* h = E.ordinal10331(u);
    if (h) {
        void* items[8] = {};
        uint32_t n = E.ordinal10383(h, items);
        for (uint32_t i = 0; i < n && i < 8; i++)
            for (void* p = rdptr(items[i], 0x74); p; p = rdptr(p, 0xe8))
                if (rd32(p, 0) == 1 && rd32(p, 4) == 0x1b2 && rd32(p, 0x10) == 0xc) return 1;
    }
    return 0;
}

// 1.13c D2Game+0x5A200  (in_EAX = game)
void d2game_5A200(void* game) {
    void* n = findNode(game, 0x20);
    if (!n || rd8(n, 9) == 0 || rd8(n, 0xb) == 5) return;
    if ((int32_t)rd32(rdptr(n, 0x18), 0xa8) < 5) {
        wr8(n, 0xb, 5); wr8(n, 0x14, 0);
        E.subDFCC0(n, 0);
    }
}

// 1.13c D2Game+0x5A290  (unaff_EDI = game)
uint32_t d2game_5A290(void* game, void** out) {
    void* n = findNode(game, 0x20);
    if (!n) return 0;
    uint8_t* a = (uint8_t*)rdptr(n, 0x18);
    int32_t i = E.sub58260(n);
    if (i < 0 || a[0xf8 + i] == 0) return 0;
    uint32_t id = rd32(a, 0xec + i * 4);
    for (void* p = rdptr(game, 0x1520 + (id & 0x7f) * 4); p; p = rdptr(p, 0xe4))
        if (rd32(p, 0xc) == id) { *out = p; return 1; }
    return 0;
}

// 1.13c D2Game+0x5A690  (in_EAX = game, param_2 = unit)
void d2game_5A690(void* game, void* unit) {
    void* n = findNode(game, 0x1f);
    if (!n || rd8(n, 9) == 0 || rd8(n, 0xb) >= 2 || !unit) return;
    void* d = rdptr(unit, 0x14);
    if (rd32(unit, 0) == 1 && d && rd16(d, 0x26) == 0x2a) {
        wr8(n, 0x14, 0); wr8(n, 0xb, 2);
        E.subDFCC0(n, 0);
    }
}

// 1.13c D2Game+0x5A9B0  (param_1 = node ecx, param_2 = event {+0 game,+0xc,+0x14,+0x18})
void d2game_5A9B0(void* node, void* ev) {
    uint32_t a = rd32(ev, 0x18), b = rd32(ev, 0x14);
    if (a == 0x6d) E.sub58A40((void*)(uintptr_t)rd32(ev, 0));
    if (b == 0x6d) {
        if (rd16(node, 0x9c) != 0) E.sub33830((uint8_t*)node + 0x1c);
        if (rd8(node, 0xc) == 2) {
            void* game = rdptr(ev, 0);
            void* r = E.ordinal10920(rdptr(ev, 0xc));
            uint32_t u = rd32(r, 0x10 + rd8(game, 0x6d) * 4);
            if (E.ordinal10174(u, rd32(node, 0xe0), 0) == 1) return;
            if (E.ordinal10174(u, rd32(node, 0xe0), 1) == 1) return;
            E.sub35EB0();
            if (rd8(node, 0xb) != 1) {
                wr8(node, 0x14, 0); wr8(node, 0xb, 1);
                E.subDFCC0((void*)E.sub58D40, 0);
            }
            E.subDFCC0(node, 0);
        }
    }
    if (a > 0x82) {
        if (rd8(node, 9) != 0 && rd8(node, 0xb) < 2) {
            wr8(node, 0x14, 0); wr8(node, 0xb, 2);
            E.subDFCC0(node, 0);
        }
        E.subDFCC0(node, 0);
    }
}
