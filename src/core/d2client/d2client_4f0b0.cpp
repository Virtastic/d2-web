// 1.13c D2Client+0x4F0B0: build the 23-word unit-event request for a player unit's animation/skill index and hand it to the sender 0xB30F0, then
// 0x4C7C0 (EAX = unit, stack idx, q[7], q[8]). EAX = ESI = unit; plain ret.
// The branch "unit[0] is neither 0 nor 1 -> 0xA60D0" is unreachable (unit[0] == 1 is the entry guard); the local1c = unit it guards is kept.
// Seams: 10362 (D2Common stdcall unit,1), 0xA5F70 (ESI = unit), 10983 / 10764 (stdcall p), 0x1630 / 0x1660 (ECX), 0xA1340 (EBX = unit, stack &y, EDI = &x),
// 0xB30F0 (ECX = block), 0x4DA20 (EAX), 0x4CDD0 (ESI), 0x4C7C0. Addresses are 32-bit words in the probe's data model.
#include <cstdint>
#include <cstring>
namespace d2c4f0b0 {
struct Env {
    void* c;
    int32_t (*idx)(void*, uint32_t unit);                    // 10362(unit, 1)
    uint32_t (*cur)(void*, uint32_t unit);                   // 0xA5F70
    uint32_t (*w10983)(void*, uint32_t p);
    uint32_t (*w10764)(void*, uint32_t p);
    uint32_t (*c1630)(void*, uint32_t u);
    uint32_t (*c1660)(void*, uint32_t u);
    uint32_t (*a1340)(void*, uint32_t unit, uint32_t* px, uint32_t* py);   // x through EDI, y through stack word 0
    void (*b30f0)(void*, const uint32_t* blk);
    void (*f4da20)(void*, uint32_t unit);
    void (*f4cdd0)(void*, uint32_t unit);
    void (*f4c7c0)(void*, uint32_t unit, int32_t idx, uint32_t q7, uint32_t q8);
};
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Client+0x4F0B0
void sub_4F0B0(const Env& e, uint32_t unit) {
    if (!unit || rd(unit, 0) != 1) return;
    const int32_t idx = e.idx(e.c, unit);
    if (idx == -1) return;
    uint32_t q[23] = {};
    q[0] = 0x20;
    if (idx >= 0x3F && idx <= 0x42) { q[0] = 0x34; q[10] = 2; }
    q[1] = unit; q[2] = unit; q[4] = (uint32_t)idx; q[12] = 1;
    const uint32_t cur = e.cur(e.c, unit);
    if (!cur) {
        const uint32_t p = rd(unit, 0x2C);
        if (!p) return;
        q[7] = e.w10983(e.c, p);
        q[8] = e.w10764(e.c, p);
    } else {
        q[7] = e.c1630(e.c, cur);
        q[8] = e.c1660(e.c, cur);
    }
    if (idx == 0xEE || idx == 0xEF) {
        if (cur) {
            uint32_t x = 0, y = 0;
            const uint32_t u1c = unit;
            if (e.a1340(e.c, unit, &x, &y) && x && y) {
                uint32_t r[23] = {};
                r[0] = 0x521; r[1] = u1c; r[4] = (uint32_t)idx;
                r[5] = e.c1630(e.c, unit); r[6] = e.c1660(e.c, unit);
                r[7] = x; r[8] = y; r[9] = 0x1E; r[11] = 0; r[12] = 1;
                e.b30f0(e.c, r);
            }
        }
    } else if (idx == 0x196) {
        e.f4da20(e.c, unit);
        e.f4cdd0(e.c, unit);
    } else {
        e.b30f0(e.c, q);
    }
    e.f4c7c0(e.c, unit, idx, q[7], q[8]);
}
}
