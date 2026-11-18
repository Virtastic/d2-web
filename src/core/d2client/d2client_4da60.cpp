// 1.13c D2Client+0x4DA60: per-unit hook run after a mode change -- picks, from the unit type and the monster id, whether the unit's
// animation is reset (0x2A6C0 with a mode in EDI) and the client is told (D2Common 10595), or left alone.
// Register convention: EAX = unit; ESI keeps the unit for the callees that take it there. Addresses are 32-bit words in the probe's data model.
// Seams: 0x1700 (EAX), D2Common 10770 / 10559 / 10498 (stdcall 1, 2, 1), 10595 (stdcall 1), 0x2A6C0 (ESI = unit, EDI = mode), 0xA5F70 (ESI),
// 0xA5FB0 (EAX, stack 0), 0x1230 (EAX = id), 0x4DA20 (EAX), 0x4CDD0 (ESI). Global: sgptDataTables+0xA80 (monstats count).
#include <cstdint>
#include <cstring>
namespace d2c4da60 {
struct Env {
    void* c;
    int32_t (*idx)(void*, uint32_t unit);                       // 0x1700
    uint32_t (*row)(void*, int32_t id);                         // 0x1230
    void (*o10770)(void*, uint32_t p);
    void (*o10559)(void*, uint32_t p, uint32_t z);
    void (*o10498)(void*, uint32_t p);
    void (*o10595)(void*, uint32_t unit);
    void (*reset)(void*, uint32_t unit, uint32_t mode);         // 0x2A6C0
    uint32_t (*cur)(void*, uint32_t unit);                      // 0xA5F70
    void (*cur2)(void*, uint32_t unit);                         // 0xA5FB0 (stack 0)
    void (*f4da20)(void*, uint32_t unit);
    void (*f4cdd0)(void*, uint32_t unit);
};
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Client+0x4DA60
void sub_4DA60(const Env& e, uint32_t unit, uint32_t dtCount) {
    if (!unit || rd(unit, 0) != 1) return;
    const uint32_t type = rd(unit, 0x10);
    if (type >= 0x10) return;
    int32_t id = e.idx(e.c, unit);
    if (id < 0 || id >= (int32_t)dtCount) id = -1;
    if (type == 0xD && id != 0x4E) {
        const uint32_t p = rd(unit, 0x2C);
        if (!p) return;
        e.o10770(e.c, p); e.o10559(e.c, p, 0); e.o10498(e.c, p);
        e.reset(e.c, unit, 3);
        return;
    }
    if (e.cur(e.c, unit)) e.cur2(e.c, unit);
    const uint32_t r = e.row(e.c, id);
    if (!r) return;
    if (*(const uint8_t*)(uintptr_t)(r + 0x1A7)) {
        bool hit = false;   // reset + notify, then return
        switch (rd(unit, 0x10)) {
        case 2: if (id == 0xE7 || id == 0x76) return; break;
        case 4:
            if (id == 0x193 || id == 0x194) hit = true;
            else if (id == 0x1F1) { e.f4da20(e.c, unit); e.f4cdd0(e.c, unit); }
            break;
        case 8: if (id == 0x6E) return; /* fallthrough */
        case 5: case 9: if (id == 0x193 || id == 0x194) hit = true; break;
        case 10:
            if (id == 0x88) { e.reset(e.c, unit, 0xB); e.o10595(e.c, unit); return; }
            break;
        case 11: if (id == 0x88) return; break;
        case 12: if (id == 0x11C) return; break;
        case 14: if (id == 0xF7) return; break;
        default: break;
        }
        if (hit) { e.reset(e.c, unit, 8); e.o10595(e.c, unit); return; }
    }
    if (((rd(unit, 0xC4) >> 0x15) & 1) != 0 || ((rd(unit, 0xC8) >> 0xD) & 1) == 0) {
        e.reset(e.c, unit, 1); e.o10595(e.c, unit);
    }
}
}
