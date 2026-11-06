// 1.13c D2Client+0x51500: free-cell search around a unit. EBX = unit, stack 0 = player data st (byte +6), bm = gdwBitMasks.
// The five-entry order table (0xD5EC8) rotates the 8-way offset index; dx @0xD5F04, dy @0xD5EE4. The retail loop resets the index to its
// initial value (the seed) after a miss.
#include <cstdint>
#include <cstring>
namespace d2client_51500 {
struct Env {
    int32_t (*pathX)(uint32_t); int32_t (*pathY)(uint32_t); uint32_t (*seed)(uint32_t); uint32_t (*resolve)(uint32_t);
    int16_t (*o10573)(uint32_t, int32_t, int32_t, uint32_t, uint32_t);
    uint32_t (*o10389)(uint32_t, int32_t, int32_t, uint32_t, uint32_t, uint32_t);
    void (*o11129)(uint32_t, uint32_t); uint32_t (*f1510)(uint32_t, uint32_t); uint32_t (*fa62a0)(uint32_t, uint32_t);
};
static uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static int32_t coord(const Env& e, uint32_t u, bool y) {
    int32_t t = (int32_t)rd(u); uint32_t p = rd(u + 0x2c);
    if (t == 2 || (t > 3 && t <= 5)) return (int32_t)rd(p + (y ? 0x10 : 0xc));
    return p ? (y ? e.pathY(p) : e.pathX(p)) : 0;
}
uint32_t sub_51500(const Env& e, uint32_t unit, const uint8_t* st, const uint8_t* bm) {
    static const int32_t order[5] = { 0, -1, 1, -2, 2 };
    static const int32_t dy[8] = { 3, 2, 0, -2, -3, -2, 0, 2 };
    static const int32_t dx[8] = { 0, -2, -3, -2, 0, 2, 3, 2 };
    uint32_t s0 = (e.seed(unit) & 0xff) >> 3;
    uint32_t room = e.resolve(unit);
    for (int i = 0; i < 5; i++) {
        uint32_t s = ((uint32_t)order[i] + s0) & 7;
        int32_t x = dx[s] + coord(e, unit, false);
        int32_t y = dy[s] + coord(e, unit, true);
        if (e.o10573(room, x, y, 1, 0x180) == 0) continue;
        uint32_t o = e.o10389(room, x, y, 0xB20511b0u, 0, 1);
        if (!o || !((rd(o + 0xc4) >> 2) & 1)) continue;
        if ((bm[0x10] & st[6]) == 0 || (rd(o) == 1 && e.f1510(rd(o + 4), 6)) || e.fa62a0(o, unit)) {
            e.o11129(unit, o);
            return o;
        }
    }
    return 0;
}
}  // namespace d2client_51500
