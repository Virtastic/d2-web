// D2Client rva 0x9EC10: walk a list of timed/lit cel overlays and draw each.
// EAX = list owner (head at +0x20), stack param_1 = view. Retail 32-bit layout: pointers are uint32_t
// guest addresses, resolved by the injected env. Callees and tables are injected.
#include <cstdint>
#include <cstddef>

struct D2Client9EC10Overlay {   // node->+0xc target, dword indexed (piVar1)
    int32_t x, y;               // +0x00 +0x04
    uint32_t pad2[3];           // +0x08..+0x10
    uint32_t flags;             // +0x14 (piVar1[5])
    uint32_t cel;               // +0x18 guest ptr; first dword = kind index
    uint32_t pad7[2];           // +0x1C +0x20
    uint32_t state;             // +0x24 (piVar1[9])
    uint32_t alpha;             // +0x28 (piVar1[10]) low byte
    uint32_t expiry;            // +0x2C (piVar1[0xb]) tick
};
struct D2Client9EC10Node { uint32_t pad0; uint32_t sx, sy; uint32_t item; uint32_t next; }; // +4 sx = guest ptr to the light-sample base when used by 0xA8230, else screen x (mode!=0); +8 sy
static_assert(offsetof(D2Client9EC10Overlay, flags) == 0x14 && offsetof(D2Client9EC10Overlay, expiry) == 0x2C, "ov");
static_assert(offsetof(D2Client9EC10Node, next) == 0x10 && offsetof(D2Client9EC10Node, item) == 0xC, "node");

struct D2Client9EC10Env {
    uint32_t light;                                   // DAT +0x11C414
    uint32_t (*ord10013)();                           // mode switch (nonzero: use node coords)
    uint32_t (*tick)();                               // GetTickCount
    const uint32_t* kindCount;                        // +0xD0A70 [kind]
    const uint32_t* idxA;                             // +0xDED18, 0x30 bytes/kind, 8 bytes/entry, dword used
    const uint32_t* idxB;                             // +0xDEEF8, same layout (state&1)
    uint32_t (*celKind)(uint32_t cel);                // *(int*)cel
    uint32_t (*listHead)(uint32_t owner);             // *(owner+0x20)
    const D2Client9EC10Overlay* (*item)(uint32_t g);
    D2Client9EC10Overlay* (*itemW)(uint32_t g);
    const D2Client9EC10Node* (*node)(uint32_t g);
    void (*lightSample)(uint32_t p, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a); // 0xA8230 (p,&40,&3d,&3e,&3f)
    uint32_t (*draw1)(uint32_t cel, int32_t x, int32_t y, const uint32_t* col, uint32_t light);        // ord 10001
    uint32_t (*draw58)(uint32_t cel, int32_t x, int32_t y, const uint32_t* col, uint32_t light, uint32_t a); // ord 10058
};

// 1.13c D2Client+0x9EC10
extern "C" void d2_client_9ec10(const D2Client9EC10Env& e, uint32_t owner, const int32_t* view /* +4,+8,+0x24,+0x28 as v[1],v[2],v[9],v[10] */) {
    uint32_t n = e.listHead(owner);
    if (!n) return;
    const uint32_t light = e.light;
    const uint32_t mode = e.ord10013();
    const uint32_t now = e.tick();
    for (; n; n = e.node(n)->next) {
        const D2Client9EC10Node* nd = e.node(n);
        D2Client9EC10Overlay* o = e.itemW(nd->item);
        if ((o->state & 4) && o->expiry <= now) { o->state &= ~4u; o->flags |= 8; }
        if ((o->flags & 0x408) != 0 || (o->flags & 0x1c000) == 0) continue;
        const uint32_t kind = e.celKind(o->cel);
        const uint32_t cnt = e.kindCount[kind];
        const uint32_t* tab = (o->state & 1) ? e.idxB : e.idxA;
        uint32_t col[8];
        for (uint32_t i = 0; i < cnt; i++) {
            uint8_t b0, b1, b2, b3;
            e.lightSample(nd->sx + tab[kind * 12 + i * 2] * 8, &b0, &b1, &b2, &b3);
            col[i] = b0 | (uint32_t)b1 << 8 | (uint32_t)b2 << 16 | (uint32_t)b3 << 24;
        }
        int32_t x, y;
        if (mode == 0) { x = (view[1] - view[9]) + o->x; y = (o->y - view[10]) + view[2]; }
        else { x = (int32_t)nd->sx; y = (int32_t)nd->sy; }
        const uint8_t a = (uint8_t)o->alpha;
        const uint32_t r = (a == 0xFF) ? e.draw1(o->cel, x, y, col, light) : e.draw58(o->cel, x, y, col, light, a);
        if (r) o->flags |= 0x20000;
    }
}
