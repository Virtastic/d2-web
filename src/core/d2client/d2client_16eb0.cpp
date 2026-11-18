// 1.13c D2Client+0x16EB0 scatter-spawn. Env carries the host callees.
#include <cstdint>
#include <cstring>
struct D2c16eb0Env { uint8_t* (*mem)(uint32_t); uint32_t tblSlot; uint32_t (*unitRoom)(uint32_t); void (*unitPos)(uint32_t, int32_t*); uint32_t (*roomAt)(uint32_t, uint32_t, uint32_t);
    uint32_t (*collide)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t); void (*spawn)(uint32_t, uint32_t, uint32_t); };
static uint32_t rd32(const D2c16eb0Env* e, uint32_t a) { uint32_t v; std::memcpy(&v, e->mem(a), 4); return v; }
extern "C" void d2_client_16eb0(uint32_t unit, int32_t cls, int32_t count, const D2c16eb0Env* e) {
    uint32_t room = e->unitRoom(unit);
    uint32_t tbl = rd32(e, e->tblSlot);
    if (cls < 0 || cls >= (int32_t)rd32(e, tbl + 0xA80)) return;
    int32_t idx = (int16_t)(rd32(e, rd32(e, tbl + 0xA78) + 0x18 + (uint32_t)cls * 0x1A8) & 0xFFFF);
    if (idx < 0 || idx >= (int32_t)rd32(e, tbl + 0xA98)) return;
    uint32_t rec = (uint32_t)idx * 0x134 + rd32(e, tbl + 0xA90);
    if (!rec) return;
    int32_t pos[2];
    e->unitPos(unit, pos);
    int32_t size = (int8_t)e->mem(rec + 8)[0];
    for (int i = 0;;) {
        uint32_t lo = rd32(e, unit + 0x20), hi = rd32(e, unit + 0x24);
        uint64_t s1 = (uint64_t)lo * 0x6AC690C5u + hi;
        uint32_t x = (uint32_t)((uint32_t)s1 % 5u) - 2u + (uint32_t)pos[0];
        uint64_t s2 = (uint64_t)(uint32_t)s1 * 0x6AC690C5u + (s1 >> 32);
        std::memcpy(e->mem(unit + 0x20), &s2, 8);
        uint32_t y = (uint32_t)((uint32_t)s2 % 5u) - 2u + (uint32_t)pos[1];
        room = e->roomAt(room, x, y);
        if ((e->collide(room, x, y, (uint32_t)size, 0x3F11) & 0xFFFF) == 0) { e->spawn((uint32_t)cls, x, y); --count; }
        if (!(count > 0) || ++i >= 10) break;
    }
}
