// D2Client rva 0x9E970: per-frame walk of a linked list of timed/faded
// draw entries; updates fade alpha / expiry and draws the live ones through the D2Gfx table.
// Retail 32-bit layout; all pointers are retail 32-bit addresses resolved through the env seam.
#include <cstddef>
#include <cstdint>

struct D2Client9E970Entry {   // piVar1 (dword-indexed in the decompile)
    uint32_t x, y;            // +0x00,+0x04
    uint32_t pad08[3];        // +0x08..+0x13
    uint32_t flags;           // +0x14 [5]
    uint32_t cel;             // +0x18 [6] -> dword, first dword is the table index
    uint32_t guard;           // +0x1C [7] ; 0xF is fatal
    uint32_t pad20;           // +0x20
    uint32_t state;           // +0x24 [9]: 1 alt table, 2 fading, 4 expiring
    uint8_t  alpha;           // +0x28 [10]
    uint8_t  alphaMin;        // +0x29
    uint8_t  alphaMax;        // +0x2A
    uint8_t  pad2b;
    uint32_t endTick;         // +0x2C [0xb]
};
static_assert(offsetof(D2Client9E970Entry, flags) == 0x14 && offsetof(D2Client9E970Entry, guard) == 0x1C &&
              offsetof(D2Client9E970Entry, state) == 0x24 && offsetof(D2Client9E970Entry, alphaMin) == 0x29 &&
              offsetof(D2Client9E970Entry, endTick) == 0x2C, "9E970 entry layout");

struct D2Client9E970Node {    // iStack_40
    uint32_t pad0;
    uint32_t base;            // +4  (iStack_2c source / draw x)
    uint32_t y;               // +8
    uint32_t entry;           // +0xC retail ptr to D2Client9E970Entry
    uint32_t next;            // +0x10 retail ptr
};
static_assert(offsetof(D2Client9E970Node, next) == 0x10, "9E970 node layout");

struct D2Client9E970Env {
    uint32_t colorMask;       // DAT_6fbcc414
    uint32_t tickGate;        // DAT_6fba17c8 (fade is time-based when > 3)
    const uint32_t* perCelCount;   // DAT_6fb80a70 [cel index]
    const uint32_t* tabNormal;     // DAT_6fb8ed18, stride 0x30 bytes per cel, 8 bytes per slot (first dword = index)
    const uint32_t* tabAlt;        // DAT_6fb8eef8
    uint32_t ctxX, ctxY;      // param_3+4/+8
    uint32_t ctxOffX, ctxOffY;// param_3+0x24/+0x28
    uint32_t (*ord10013)();   // selects absolute vs ctx-relative position
    uint32_t (*getTick)();    // GetTickCount
    void (*fatal)(uint32_t code);                 // Ordinal_10265(code)+10024+_exit(-1)
    uint32_t (*loadColor)(uint32_t src, uint8_t out[4]);   // 0xA8230: out[0..3] = first, 41, 42, 43 order
    uint32_t (*draw)(uint32_t cel, int32_t x, int32_t y, const uint32_t* cols, uint32_t mask);               // ord 10001
    uint32_t (*drawAlpha)(uint32_t cel, int32_t x, int32_t y, const uint32_t* cols, uint32_t mask, uint32_t a); // ord 10058
    D2Client9E970Entry* (*entry)(uint32_t p);     // retail ptr -> host
    uint32_t (*celIndex)(uint32_t p);             // *(int*)p
};

// 1.13c D2Client+0x9E970
extern "C" void d2_client_9e970(D2Client9E970Env& e, D2Client9E970Node* node, uint32_t mode,
                                D2Client9E970Node* (*nextNode)(uint32_t)) {
    const uint32_t tick = e.getTick();
    const bool timed = (int32_t)e.tickGate > 3;
    const uint32_t abs = e.ord10013();
    if (mode > 1) { e.fatal(0xf3); return; }
    for (; node; node = node->next ? nextNode(node->next) : nullptr) {
        D2Client9E970Entry* p = e.entry(node->entry);
        if (p->guard == 0xf) { e.fatal(0xf9); return; }
        if (p->state & 2) {
            if (!timed || p->endTick <= tick) { p->alpha = p->alphaMax; p->state &= ~2u; }
            else {
                const int32_t v = ((int32_t)p->alphaMax - (int32_t)p->alphaMin) * (int32_t)(tick - p->endTick + 500);
                p->alpha = (uint8_t)(v / 500 + p->alphaMin);
            }
            p->flags = p->alpha == 0 ? (p->flags | 0x400) : (p->flags & 0xfffffbffu);
        }
        if ((p->state & 4) && p->endTick <= tick) { p->state &= ~4u; p->flags |= 8; }
        if ((p->flags & 0x408) == 0 && mode != ((p->flags & 0x1c000) != 0)) {
            const uint32_t idx = e.celIndex(p->cel);
            const uint32_t n = e.perCelCount[idx];
            const uint32_t* tab = (p->state & 1) ? e.tabAlt : e.tabNormal;
            uint32_t cols[8] = {};
            for (uint32_t i = 0; i < n; i++) {
                uint8_t c[4];
                e.loadColor(node->base + tab[idx * 12 + i * 2] * 8, c);   // 0x30/4 dwords per cel, 2 dwords per slot
                cols[i] = (uint32_t)c[0] | (uint32_t)c[1] << 8 | (uint32_t)c[2] << 16 | (uint32_t)c[3] << 24;
            }
            int32_t x, y;
            if (abs == 0) { x = (int32_t)(e.ctxX - e.ctxOffX + p->x); y = (int32_t)(p->y - e.ctxOffY + e.ctxY); }
            else { x = (int32_t)node->base; y = (int32_t)node->y; }
            const uint32_t r = p->alpha == 0xff ? e.draw(p->cel, x, y, cols, e.colorMask)
                                                : e.drawAlpha(p->cel, x, y, cols, e.colorMask, p->alpha);
            if (r) p->flags |= 0x20000;
        }
    }
}
