// 1.13c D2Client+0xAF640 unit-update packet handler: ECX = packet (byte +1 kind, dword +2 id, words +6/+8, byte +0xA). Looks the unit up (0xA4E20), applies the
// position update (0xA7E20), stamps the unit's byte +0x18 and dword +0x1C, runs the type 0/1 side calls on its +0x2C record and, for the local player, extends the
// 0x11981C deadline by 3000 ticks. A null lookup is not an error; a null thunk 0xC1F0 result and a player 0xC55C failure are fatal (seams abort).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cAf640Env {
    uint8_t* (*f54e20)(uint32_t id7, uint32_t tblRva, uint32_t kind);   // EAX = id & 0x7f, EDX = 0x10A608 + (kind << 9)
    void (*f57e20)(uint8_t* unit, uint32_t x, uint32_t y, uint32_t b);
    uint32_t (*c1f0)(uint8_t* unit);
    uint32_t (*c1de)(uint32_t v);
    uint32_t (*c1ea)(uint32_t v);
    void (*c502)(uint8_t* unit);
    void (*c3ac)(uint32_t rec, uint32_t v);
    uint32_t (*c538)(uint8_t* unit);
    void (*c400)(uint32_t rec, uint32_t v);
    uint32_t (*c55c)(uint32_t v);
    uint32_t (*tick)();
    void (*fatal)(uint32_t line);
};
struct D2cAf640St { uint32_t player; uint32_t g3b8; uint32_t g81c; };   // 0x11BBFC, 0x11C3B8, 0x11981C
static_assert(sizeof(D2cAf640St) == 12, "state");
static uint32_t af_rd(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2c_af640(const D2cAf640Env* e, const uint8_t* pkt, D2cAf640St* g) {
    const uint32_t kind = pkt[1], id = af_rd(pkt, 2);
    const uint32_t x = (uint16_t)(pkt[6] | pkt[7] << 8), y = (uint16_t)(pkt[8] | pkt[9] << 8);
    uint8_t* u = e->f54e20(id & 0x7f, (kind << 9) + 0x10A608u, kind);
    if (!u) return;
    e->f57e20(u, x, y, pkt[0xa]);
    uint32_t t = e->c1f0(u);
    if (!t) { e->fatal(0x538); return; }
    u[0x18] = (uint8_t)e->c1ea(e->c1de(t));
    memcpy(u + 0x1c, &g->g3b8, 4);
    uint32_t rec = af_rd(u, 0x2c);
    if (rec) {
        uint32_t ty = af_rd(u, 0);
        if (ty == 0) { e->c502(u); e->c3ac(af_rd(u, 0x2c), 0x1c09); }
        else if (ty == 1 && e->c538(u) == 0) e->c400(af_rd(u, 0x2c), 0x100);
    }
    if ((uint32_t)(uintptr_t)u == g->player) {
        if (e->c55c(e->c1f0(u))) { e->fatal(0x81e); return; }
        uint32_t d = e->tick() + 0xbb8;
        if (d > g->g81c) g->g81c = d;
    }
}
}
