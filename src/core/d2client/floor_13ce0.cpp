// 1.13c D2Client+0x13CE0: per-floor-tile submit. Builds the light block for the tile
// and hands it to D2Gfx 10076; may spawn the tile's ambient effects; widens the frame bounds (0x13560).
#include <cstddef>
#include <cstdint>
#include <emscripten.h>
#include <initializer_list>

struct FloorCtx {            // EBX of the retail call; 32-bit fields (pointers are the low 32 bits in wasm)
    int32_t a0, a1, spawnA, spawnB, spawnLimit, f5, f6; uint32_t lcg; int32_t a8; uint32_t rect;
};
static_assert(offsetof(FloorCtx, lcg) == 0x1C && offsetof(FloorCtx, rect) == 0x24, "ctx");
struct FloorTile { uint8_t p0[0x14]; uint32_t flags; uint32_t sub; };
static_assert(offsetof(FloorTile, sub) == 0x18, "tile");
struct FloorSub { uint8_t p0[6]; uint8_t b6, b7; uint8_t p1[0x14 - 8]; int32_t gate; };
static_assert(offsetof(FloorSub, b7) == 7 && offsetof(FloorSub, gate) == 0x14, "sub");
struct FloorRect { uint8_t p0[0x14]; int32_t left, top, right, bottom; };
static_assert(offsetof(FloorRect, bottom) == 0x20, "rect");

extern "C" void d2_client_13560(void*, int32_t, void*, int32_t);

// The callees are other D2Client functions (0x13390.. 0x138C0, 0x80940/0x80A70) and D2Gfx 10076; `call` is their one
// door: rva, eax/ecx/edx, the stack words in push order (top first), the word count and the stack block's address.
typedef void (*FloorCallee)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, const uint32_t* st, uint32_t n, uint32_t blk);
static FloorCallee g_floor_callee;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_13ce0_set_callee(FloorCallee f) { g_floor_callee = f; }

// 1.13c D2Client+0x13CE0; `quality` is the global at D2Client+0xF17C8 (signed compare with 4)
extern "C" void d2_client_13ce0(FloorCtx* c, FloorTile* t, uint32_t p2, int32_t x, int32_t y, uint32_t p5, uint32_t p6, uint32_t quality) {
    if (t->flags & 0x408) return;
    FloorRect* r = (FloorRect*)(uintptr_t)c->rect;   // ctx[9] + 0x14 is never null in a running game (else the retail fatal path)
    FloorSub* s = (FloorSub*)(uintptr_t)t->sub;
    if (!(r->left <= x && x < r->right && r->top <= y && y < r->bottom && s->gate == 0)) return;
    static uint8_t blk[736];
    uint32_t b = (uint32_t)(uintptr_t)blk, sub = t->sub, bit = s->b7 & 1;
    auto C = [&](uint32_t rva, uint32_t a, uint32_t cx, uint32_t d, std::initializer_list<uint32_t> w) {
        if (g_floor_callee) g_floor_callee(rva, a, cx, d, w.begin(), (uint32_t)w.size(), b);
    };
    if ((int32_t)quality < 4) {
        if (!bit) C(0x138C0, 0, b, 0, { p5, p6 }); else C(0x134E0, b, 0, 0, {});
    } else if (c->a1 == 0) {
        if (!bit) C(0x137A0, (uint32_t)x, 0, 0, { b, (uint32_t)y, p5, p6, 0, (uint32_t)c->a8, (uint32_t)c->a0 });
        else C(0x13450, (uint32_t)c->a8, b, (uint32_t)x, { (uint32_t)c->a0 });
    } else if (!bit) C(0x135E0, 0, 0, 0, { b, p5, p6, 0, (uint32_t)c->a8, (uint32_t)c->a0 });
    else C(0x13390, (uint32_t)c->a8, b, 0, { p5, p6, 0, (uint32_t)c->a0 });
    C(0xD1C8, 0, 0, 0, { sub, b, (uint32_t)x, (uint32_t)y, p5, p6, 0xFF, (uint32_t)c->a8, p2 });   // D2Gfx 10076
    if (s->b6 & 2) {
        uint64_t* st = (uint64_t*)(uintptr_t)c->lcg;
        uint64_t n = (uint64_t)(uint32_t)*st * 0x6AC690C5ull + (uint32_t)(*st >> 32);
        *st = n;
        int32_t rr = (int32_t)((uint32_t)n % 1000);
        if (c->spawnA && rr < c->spawnLimit) C(0x80A70, 0, 0, 0, { (uint32_t)x, (uint32_t)y, p5 << 16, p6 << 16 });
        if (c->spawnB && rr < 100) C(0x80940, 0, 0, 0, { (uint32_t)x, (uint32_t)y, p5 << 16, p6 << 16 });
    }
    d2_client_13560(c, (int32_t)p5, t, (int32_t)p6);
}
