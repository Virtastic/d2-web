// D2Client inventory cell resolution and hover drawing, transcribed from the 1.13c disassembly (D2Client.dll image base 0x6FAB0000).
// Every call out of the function sits behind a small table of function pointers so a probe can record it.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

namespace {
struct CellArea {   // the panel layout record EAX points at (0x18 bytes of it are read)
    uint8_t w, h, pad0[2];
    int32_t left, r8, top;
    uint8_t pad1[4];
    uint8_t cellW, cellH;
};
}

// 1.13c D2Client+0x94820  (ECX = item, EAX = layout area; stack x, y, &outX, &outY, ret 0x10)
// The cell the item's top-left lands on when its footprint is centred on (x, y).
struct D2c94820Env {
    void (*itemSize)(uint32_t item, uint8_t* w, uint8_t* h);        // Ordinal_10114 through thunk 0xC7F6 (item, &w, &h, source, line), ret 0x14
    void (*itemOffset)(uint32_t item, uint32_t* a, uint32_t* b);   // 0x6E930 (item, 0, 0, &a, &b), ret 0x14; may leave a, b untouched
    void (*fatal)();                                               // a null item: the error report then _exit(-1)
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_94820(const void* area, uint32_t item, int32_t x, int32_t y, uint32_t* ox, uint32_t* oy, const D2c94820Env* e) {
    const CellArea* A = (const CellArea*)area;
    if (!item) { e->fatal(); return; }
    uint32_t cx = (uint32_t)(x - A->left) / A->cellW;
    uint32_t cy = (uint32_t)(y - A->top) / A->cellH;
    uint8_t w, h; uint32_t a, b;
    e->itemSize(item, &w, &h);
    e->itemOffset(item, &a, &b);
    if (!(w & 1)) cx = (((a >> 2) - (uint32_t)A->left) + (uint32_t)x) / A->cellW;
    if (w == A->w) cx = (uint32_t)(w >> 1);
    if (!(h & 1)) cy = (((b >> 2) - (uint32_t)A->top) + (uint32_t)y) / A->cellH;
    if (h == A->h) cy = (uint32_t)(h >> 1);
    *ox = cx; *oy = cy;
    if (w > 1) { uint32_t t = *ox - (uint32_t)(w >> 1); *ox = (int32_t)t < 0 ? 0 : t; }
    if (h > 1) { uint32_t t = *oy - (uint32_t)(h >> 1); *oy = (int32_t)t < 0 ? 0 : t; }
}

// 1.13c D2Client+0x945D0  (EAX = layout area, EDX = owner; stack inv, item, x, y, flag, ret 0x14)
// Hover bookkeeping of an inventory-style panel. With an item in hand it stores the cell its footprint would cover (unless that runs off the
// panel); without one it looks up the item under (x, y) and, when it is a new one, stores that item's pixel rectangle.
struct D2c945d0G {   // module globals: 0x119854 read; 0xE0EA8..0xE0EBF, 0x11BC28, 0x11BC38 and 0x11BC3C written
    uint32_t src;                                  // 0x119854: the last argument of the owner-kind lookup
    int32_t rx0, ry0, rx1, ry1, cellX, cellY;      // 0xE0EA8 0xE0EAC 0xE0EB0 0xE0EB4 0xE0EB8 0xE0EBC
    uint32_t hoverFlag, hoverItem, hoverPrev;      // 0x11BC28 0x11BC38 0x11BC3C
};
struct D2c945d0Env {
    uint32_t (*ownerKind)(uint32_t owner, uint32_t flag, uint32_t src);                                                    // thunk 0xC8A4, ret 0xC
    uint32_t (*itemAtCell)(uint32_t inv, uint32_t cx, uint32_t cy, uint32_t* a, uint32_t* b, uint32_t kind, uint32_t flag);   // thunk 0xC95E, ret 0x1C
    void (*itemSize)(uint32_t item, uint8_t* w, uint8_t* h);                                                               // thunk 0xC7F6
    void (*itemOffset)(uint32_t item, uint32_t* a, uint32_t* b);                                                           // 0x6E930
    int32_t (*gridX)(uint32_t item);                                                                                       // 0x1630 (ECX = item)
    int32_t (*gridY)(uint32_t item);                                                                                       // 0x1660 (ECX = item)
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_945d0(const void* area, uint32_t owner, uint32_t inv, uint32_t item, int32_t x, int32_t y, uint32_t flag,
                                                     D2c945d0G* g, const D2c945d0Env* e) {
    const CellArea* A = (const CellArea*)area;
    const uint32_t kind = e->ownerKind(owner, flag, g->src);
    uint32_t cx = (uint32_t)(x - A->left) / A->cellW;
    uint32_t cy = (uint32_t)(y - A->top) / A->cellH;
    if (item) {
        uint8_t w, h; uint32_t a, b;
        e->itemSize(item, &w, &h);
        e->itemOffset(item, &a, &b);
        // retail reuses the argument slots for w, h: x is dead after the width step, so h stays in its place
        if (!(w & 1)) cx = (((a >> 2) - (uint32_t)A->left) + (uint32_t)x) / A->cellW;
        if (w == A->w) cx = (uint32_t)(A->w >> 1);
        if (!(h & 1)) cy = (((b >> 2) - (uint32_t)A->top) + (uint32_t)y) / A->cellH;
        if (h == A->h) cy = (uint32_t)(A->h >> 1);
        int32_t ix = (int32_t)cx, iy = (int32_t)cy;
        if (w > 1) { ix -= (int32_t)(w >> 1); if (ix < 0) ix = 0; if ((int32_t)w + ix > (int32_t)A->w) return; }
        if (h > 1) { iy -= (int32_t)(h >> 1); if (iy < 0) iy = 0; if ((int32_t)h + iy > (int32_t)A->h) return; }
        g->cellY = iy; g->cellX = ix; g->hoverFlag = 1; g->hoverItem = 0; g->hoverPrev = 0;
        return;
    }
    uint32_t a, b;
    const uint32_t hit = e->itemAtCell(inv, cx, cy, &a, &b, kind, flag);
    if (!hit) { g->hoverFlag = 0; g->hoverItem = 0; g->hoverPrev = 0; return; }
    const uint32_t prev = g->hoverPrev;
    g->hoverFlag = 1;
    if (hit == prev) return;
    uint8_t w, h;
    e->itemSize(hit, &w, &h);
    const int32_t gx = e->gridX(hit), gy = e->gridY(hit);
    const int32_t pw = (int32_t)w * A->cellW;
    const int32_t rx = (pw - (pw >> 31)) >> 1;   // retail halves toward zero
    g->rx0 = g->rx1 = rx + (int32_t)A->cellW * gx + A->left;
    const int32_t rowTop = (int32_t)A->cellH * gy + A->top;
    g->hoverItem = g->hoverPrev = hit;
    g->ry0 = rowTop;
    g->ry1 = ((int32_t)h + gy) * (int32_t)A->cellH + A->top;
}

// 1.13c D2Client+0x94970  (no arguments; everything is module state)
// Per-frame hover update of the inventory panel: when the send gate is open it refreshes the panel, applies the pending selection-table entry
// and, if the mouse is over the panel, runs the hover bookkeeping (0x945D0) for the unit the panel belongs to.
#pragma pack(push, 1)
struct D2c94970Row { uint16_t tag; uint32_t a; uint16_t pad; uint32_t b; uint8_t rest[8]; };   // the selection table at 0x10B240, 0x14 bytes a row
#pragma pack(pop)
struct D2c94970M {   // 0x11B824..0x11B867
    int32_t mouseY, mouseX; uint32_t p0[2];
    uint32_t selMode;            // 0x11B834
    uint32_t p1[7];
    uint32_t phase, selArg;      // 0x11B854 0x11B858
    uint32_t p2[2];
    uint32_t cursorMode;         // 0x11B864
};
struct D2c94970N {   // 0x11BC34..0x11BC8F
    uint32_t selByte; uint8_t p0[0x10];
    uint8_t flag;                // 0x11BC48
    uint8_t p1[0x3F];
    int32_t sel;                 // 0x11BC88
    uint32_t selUnit;            // 0x11BC8C
};
struct D2c94970E { int32_t rows; uint32_t panelOpen; };   // 0xE11C8 (rows in the table), 0xE0EA4 (cleared after a hover update)
struct D2c94970Id { uint32_t id; uint32_t on; };          // 0x11971D (unit id), 0x119721 (non-zero when the panel's unit is set)
struct D2c94970A { uint8_t w, h, p0[2]; int32_t left, right, top, bottom; uint8_t cellW, cellH; };   // the panel layout record at 0x10B3B0
struct D2c94970Env {
    uint32_t (*canSend)();                        // 0x8CF60
    void (*refresh)();                            // 0x8D300
    void (*selUpdate)(uint32_t unit);             // 0x8B900 (EAX = [0x11BC8C])
    void (*hover)(const void* area, uint32_t unit, uint32_t inv, uint32_t item, int32_t x, int32_t y, uint32_t flag);   // 0x945D0
    void (*fatal)();                              // the unit is not a player: error report, _exit(-1)
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_94970(D2c94970M* m, D2c94970N* n, D2c94970E* ex, const D2c94970Row* tab, const D2c94970Id* id,
                                                     const uint32_t* hash, const D2c94970A* area, const D2c94970Env* e) {
    if (!e->canSend()) return;
    e->refresh();
    const int32_t i = n->sel;
    if (i >= 0 && i < ex->rows && tab[i].tag != 0x276f) {
        e->selUpdate(n->selUnit);
        n->selByte = tab[i].a; m->selMode = 0; m->phase = 6; m->selArg = tab[i].b; m->cursorMode = 7;
    }
    if (!id->on) return;
    uint32_t u = hash[id->id & 0x7f];
    if (!u) return;
    for (;;) {
        uint32_t uid; memcpy(&uid, (const void*)(uintptr_t)(u + 0xC), 4);
        if (uid == id->id) break;
        memcpy(&u, (const void*)(uintptr_t)(u + 0xE4), 4);
        if (!u) return;
    }
    uint32_t type; memcpy(&type, (const void*)(uintptr_t)u, 4);
    if (type != 1) { e->fatal(); return; }
    if (m->mouseX < area->left || m->mouseX > area->right) return;
    if (m->mouseY < area->top || m->mouseY > area->bottom) return;
    uint32_t inv; memcpy(&inv, (const void*)(uintptr_t)(u + 0x60), 4);
    e->hover(area, u, inv, 0, m->mouseX, m->mouseY, n->flag);
    ex->panelOpen = 0;
}

// 1.13c D2Client+0x95140  (stdcall: unit, area, x, y)
// Draws the socket fill marks of a socketed item at (x, y) in the inventory panel: the item's footprint and socket count pick a row of 6 socket
// positions (x, y in half cells) from the table at 0xD0A98, and each socket in turn is marked and, if it holds an item, that item is drawn there.
struct D2c95140Env {
    uint32_t (*gate)(uint32_t unit);                                   // 0xC1A8 (unit, 0x10, 0x460, inv.cpp)
    uint32_t (*firstSocketed)(uint32_t inv);                           // 0xC4F0 (the unit's [+0x60])
    void (*itemSize)(uint32_t unit, uint8_t* w, uint8_t* h);           // 0xC7F6
    uint32_t (*socketCount)(uint32_t unit);                            // 0xC988 (the low byte)
    void (*mark)(const uint32_t* rec, int32_t x, int32_t y);           // 0xD096 (rec = 18 dwords, rec[13] = the global at 0x11A778)
    uint32_t (*socketItem)(uint32_t cur);                              // 0xC526
    void (*draw)(uint32_t item, int32_t x, int32_t y);                 // 0x6B5A0 (EBX = item)
    uint32_t (*socketNext)(uint32_t cur);                              // 0xC568
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_95140(uint32_t unit, const void* areaP, int32_t x, int32_t y, const uint8_t* table, uint32_t g, const D2c95140Env* e) {
    if (!e->gate(unit)) return;
    const CellArea* a = (const CellArea*)areaP;
    uint32_t inv; memcpy(&inv, (const void*)(uintptr_t)(unit + 0x60), 4);
    uint32_t cur = e->firstSocketed(inv);
    uint8_t w = 0, h = 0;
    e->itemSize(unit, &w, &h);
    const uint32_t s = e->socketCount(unit) & 0xFF;
    const uint32_t idx = (uint32_t)h + 4u * ((uint32_t)w + 2u * s) - 13u;
    const uint8_t* row = table + (int64_t)(int32_t)idx * 0x30;
    if (!(e->socketCount(unit) & 0xFF)) return;
    for (uint32_t i = 0;;) {
        int32_t sx, sy; memcpy(&sx, row + 8 * i, 4); memcpy(&sy, row + 8 * i + 4, 4);
        int32_t vx = (int32_t)((uint32_t)(sx - 1) * a->cellW), vy = (int32_t)((uint32_t)(sy + 1) * a->cellH);
        const int32_t px = ((vx - (vx >> 31)) >> 1) + x, py = ((vy - (vy >> 31)) >> 1) + y;
        uint32_t rec[18] = {}; rec[13] = g;
        e->mark(rec, px - 1, py);
        if (cur) {
            const uint32_t it = e->socketItem(cur);
            e->draw(it, px, py - (int32_t)a->cellH);
            cur = e->socketNext(cur);
        }
        ++i;
        if (!(i < (e->socketCount(unit) & 0xFF))) break;
    }
}
