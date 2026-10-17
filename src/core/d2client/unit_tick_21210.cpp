// D2Client unit mode tick, derived from the 1.13c decompile.
#include <cstdint>
#include <cstring>
struct Client21210Callees {
    void (*shifted)(const uint8_t* unit, int32_t* frame, int32_t* count);   // D2Common #10569 via thunk +0xC5EC
    void (*c69610)(); void (*c6b300)(uint32_t eax, const uint8_t* unit);   // c6b300(eax=0x34|0x9c, unit, 5, 0,0,0,0)
    void (*c1630)(const uint8_t* unit); void (*c1660)(const uint8_t* unit); void (*c6eb0)();   // c6eb0(0xE3, 4)
};
static uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
// 1.13c D2Client+0x21210  thiscall(unit, ignored); pd = the unit's +0x14 pointee (null when the field is 0)
extern "C" void d2_client_21210(uint8_t* unit, uint8_t* pd, const Client21210Callees& c) {
    if (!unit || rd32(unit) != 1 || !pd) return;
    uint32_t mode = rd32(unit + 0x10);
    if (mode == 0) {
        int32_t frame = 0, count = 0;
        c.shifted(unit, &frame, &count);
        if (frame != 0xD) return;
        c.c69610(); c.c6b300(0x34, unit); c.c6b300(0x9C, unit);
        unit[0x4E] = 0;
    } else if (mode == 0xC) {
        uint64_t s = (uint64_t)rd32(unit + 0x20) * 0x6AC690C5u + rd32(unit + 0x24);   // 64-bit seed step, stored back
        memcpy(unit + 0x20, &s, 8);
        int32_t cnt; memcpy(&cnt, pd + 0x18, 4);
        if ((uint32_t)s % 100 != 0 || cnt >= 100) return;
        c.c1630(unit); c.c1660(unit); c.c6eb0();
        cnt += 4; memcpy(pd + 0x18, &cnt, 4);
    }
}
