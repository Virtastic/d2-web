// 1.13c D2Client+0xC9030: start-skill state update on a unit. EAX = a (unit handle or 0), ESI = unit, stack id, p1, p2, p3, p4 (ret 0x14).
// Saves three values read through the unit's +0x2C record (thunks 0xC2DA, 0xC31C, 0xC340), applies a set-or-restore pair (0xC62E when a != 0, else 0xC3EE with p2 p3),
// and when the skill row's flag byte +8 hits the global bit mask byte +8 runs the 0x51CF0 / 0xC30A gate; then the +0x122 and +0x124 row shorts select 0x88A70 and 0x6B300,
// 0xA2C90 produces the result, the saved values are restored and unit +0xC4 bit 6 is re-stamped from its entry value. A bad id returns 0 with nothing touched.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cC9030Env {
    void (*da0b0)(uint8_t* unit);
    uint32_t (*c2da)(uint32_t rec); uint32_t (*c31c)(uint32_t rec); uint32_t (*c340)(uint32_t rec);
    void (*c62e)(uint8_t* unit, uint32_t v);
    void (*c3ee)(uint32_t rec, uint32_t a, uint32_t b);
    uint32_t (*f55440)(uint32_t a);
    uint32_t (*c1fc)(uint32_t a, uint32_t k);
    void (*c30a)(uint32_t a, uint32_t k, uint32_t v);
    uint32_t (*f51cf0)(uint8_t* unit, uint32_t id, uint32_t m1, uint32_t p1);
    void (*f88a70)(uint8_t* unit, int32_t ebx);
    void (*f6b300)(uint8_t* unit);
    uint32_t (*f52c90)(uint8_t* unit, uint32_t id, uint32_t p1, uint32_t p4w);
    void (*fatal)(uint32_t line);
};
static uint32_t c9_rd(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
extern "C" {
// dt = *sgptDataTables, bm = gdwBitMasks
EMSCRIPTEN_KEEPALIVE uint32_t d2c_c9030(const D2cC9030Env* e, const uint8_t* dt, const uint8_t* bm, uint8_t* unit, uint32_t a, int32_t id, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4) {
    if (id < 0 || id >= (int32_t)c9_rd(dt, 0xba0)) return 0;
    const uint8_t* row = (const uint8_t*)(uintptr_t)c9_rd(dt, 0xb98) + (size_t)(uint32_t)id * 0x23c;
    if (!row) return 0;
    e->da0b0(unit);
    uint32_t rec = c9_rd(unit, 0x2c);
    if (!rec) { e->fatal(0x946); return 0; }
    const uint32_t v1 = e->c2da(rec), v2 = e->c31c(rec), v3 = e->c340(rec);
    const uint32_t flag = c9_rd(unit, 0xc4) >> 6 & 1;
    if (a) e->c62e(unit, a); else e->c3ee(c9_rd(unit, 0x2c), p2, p3);
    uint32_t res = 0;
    if (row[8] & bm[8]) {
        uint32_t held = 0;
        if (a && e->f55440(a) && e->c1fc(a, 0x76)) { e->c30a(a, 0x76, 0); held = 1; }
        if (!e->f51cf0(unit, (uint32_t)id, 0xffffffffu, p1)) goto tail;
        if (held) e->c30a(a, 0x76, 1);
    }
    {
        int16_t w; memcpy(&w, row + 0x122, 2);
        if (w >= 0) e->f88a70(unit, (int32_t)w);
        int16_t b; memcpy(&b, row + 0x124, 2);
        if (b >= 0 && (int32_t)b < (int32_t)c9_rd(dt, 0xbc0)) e->f6b300(unit);
        res = e->f52c90(unit, (uint32_t)id, p1, p4 & 0xffff);
    }
tail:
    if (v1) e->c62e(unit, v1); else e->c3ee(c9_rd(unit, 0x2c), v2, v3);
    uint32_t f = c9_rd(unit, 0xc4);
    f = flag ? (f | 0x40) : (f & ~0x40u);
    memcpy(unit + 0xc4, &f, 4);
    return res;
}
}
