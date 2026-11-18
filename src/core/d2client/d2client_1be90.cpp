#include <cstdint>
// 1.13c D2Client+0x1BE90 -- refresh a unit's room state (EDI = unit). Reads the unit's room; with none it returns. Otherwise the
// unit's flags are (flagsA(unit) | flagsB(roomType(room))), applied to the unit; a second room lookup feeds a table lookup keyed by
// the dword at unit+0x2C whose answer is stored against the [0x11C3B8] global. The room's in-town answer t becomes the new value of
// the [0x11B7FC] global; if that changed and t is 0 the leave-town routine runs on the unit. [0x11BF74] takes the room value, then the
// room is finished.
struct D2c1be90Env {
    const uint8_t* (*mem)(uint32_t);
    uint32_t cc3b8; uint32_t* cb7fc; uint32_t* cbf74;
    uint32_t (*room)(uint32_t); uint32_t (*roomType)(uint32_t); uint32_t (*flagsA)(uint32_t); uint32_t (*flagsB)(uint32_t);
    void (*apply)(uint32_t, uint32_t); uint32_t (*lookup)(uint32_t, uint32_t); void (*store)(uint32_t, uint32_t);
    uint32_t (*town)(uint32_t); void (*leaveTown)(uint32_t); uint32_t (*roomValue)(uint32_t); void (*finish)(uint32_t);
};
extern "C" void d2_client_1be90(uint32_t unit, const D2c1be90Env* e) {
    uint32_t r = e->room(unit);
    if (!r) return;
    uint32_t t = e->roomType(r);
    uint32_t a = e->flagsA(unit);
    uint32_t b = e->flagsB(t);
    e->apply(unit, a | b);
    uint32_t r2 = e->room(unit);
    uint32_t key; const uint8_t* p = e->mem(unit + 0x2c);
    key = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    e->store(e->cc3b8, e->lookup(key, r2));
    uint32_t in = e->town(r) != 0;
    if (*e->cb7fc != in && in == 0) e->leaveTown(unit);
    *e->cb7fc = in;
    *e->cbf74 = e->roomValue(r);
    e->finish(r);
}
