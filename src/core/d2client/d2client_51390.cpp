// 1.13c D2Client+0x51390 (216 B). Retail: EDI = unit (nullable), EBX = &y, stack (ret 4) = &x.
// Unit position (x,y): no unit gives (*px,*py); unit types 2,4,5 read the path (+0x2C) at +0xC and +0x10; any other type with a path asks
// Ordinal 10867 / 10750, without a path gives 0. With the client room global set: Ordinal 10331(room) = r; r, Ordinal 10482(r,x,y,1) (a short)
// and Ordinal 10078(r,&{x,y},1,1,0,7) all nonzero: *px=x, *py=y, return 0. Otherwise return the unit.
#include <cstdint>
#include <cstring>
namespace d2client_51390 {
struct Env {
    int32_t (*pathX)(uint32_t path);                                         // Ordinal_10867 (thunk 0xC3BE)
    int32_t (*pathY)(uint32_t path);                                         // Ordinal_10750 (thunk 0xC2EC)
    uint32_t (*resolve)(uint32_t room);                                      // Ordinal_10331 (thunk 0xC1F0)
    int16_t (*o10482)(uint32_t r, int32_t x, int32_t y, uint32_t one);       // thunk 0xC4A8
    uint32_t (*o10078)(uint32_t r, const int32_t* rec, uint32_t a, uint32_t b, uint32_t c, uint32_t d);  // thunk 0xCD06
};
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
uint32_t sub_51390(const Env& e, uint32_t unit, int32_t* py, int32_t* px, uint32_t cbbfc) {
    int32_t x, y;
    if (!unit) { x = *px; y = *py; }
    else {
        int32_t t = (int32_t)rd(unit, 0); uint32_t f = rd(unit, 0x2C);
        if (t == 2 || (t > 3 && t < 6)) { x = (int32_t)rd(f, 0xC); y = (int32_t)rd(f, 0x10); }
        else { x = f ? e.pathX(f) : 0; y = f ? e.pathY(f) : 0; }
    }
    if (cbbfc) {
        uint32_t r = e.resolve(cbbfc);
        if (r && e.o10482(r, x, y, 1) != 0) {
            int32_t rec[2] = { x, y };
            if (e.o10078(r, rec, 1, 1, 0, 7)) { *px = x; *py = y; return 0; }
        }
    }
    return unit;
}
}
