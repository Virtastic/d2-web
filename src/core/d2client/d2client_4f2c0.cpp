// 1.13c D2Client+0x4F2C0: per-frame prepass for a monster unit; the monster-data record (stride 0x23C in sgptDataTables+0xB98) selects which
// frame clamp runs (record type 0x5F, 0x7E or 0x98). EAX = EDI = unit; plain ret.
// Seams: D2Common 10511 / 10397 (stdcall p), 10601 (stdcall p, file string, line 0x3B4), 10137 (p, flags), 10494 (unit, 0xC), 0x2A700 (EAX),
// 0xA3000 (EDI), 0x1260 (EAX = id), 0x1330 (EAX = 4, ECX = &seed). `tab` is sgptDataTables as the module global resolves it.
#include <cstdint>
#include <cstring>
namespace d2c4f2c0 {
struct Env {
    void* c;
    uint32_t (*w10511)(void*, uint32_t unit);
    int32_t (*w10601)(void*, uint32_t s, uint32_t line);
    uint32_t (*w10397)(void*, uint32_t s);
    void (*w10137)(void*, uint32_t s, uint32_t fl);
    uint32_t (*w10494)(void*, uint32_t unit, uint32_t v);
    uint32_t (*f2a700)(void*, uint32_t unit);
    void (*a3000)(void*, uint32_t unit);
    uint32_t (*f1260)(void*, uint32_t id);
    uint32_t (*f1330)(void*, uint32_t n, uint32_t seedp);
};
static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static uint32_t rd8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }
static uint32_t rd16(uint32_t p, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 2); return v; }
static void wr(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
// 1.13c D2Client+0x4F2C0
void sub_4F2C0(const Env& e, uint32_t unit, uint32_t tab) {
    if (!unit || rd(unit, 0) != 1) return;
    const uint32_t s = e.w10511(e.c, unit);
    if (!s) return;
    const int32_t mi = e.w10601(e.c, s, 0x3B4);
    if (mi < 0 || mi >= (int32_t)rd(tab, 0xBA0)) return;
    const uint32_t rec = rd(tab, 0xB98) + (uint32_t)mi * 0x23C;
    if (!rec) return;
    uint32_t fl = e.w10397(e.c, s);
    bool b3 = true;
    if ((fl & 1) && e.f2a700(e.c, unit)) {
        if (rd16(rec, 0x2E) != 0x43) e.w10137(e.c, s, fl | 2);
        e.a3000(e.c, unit);
        b3 = false;
    }
    const uint32_t cl = rd8(unit, 0x4E);
    if (cl == 4) { e.a3000(e.c, unit); return; }
    if (b3 && !((rd(unit, 0xC4) >> 6) & 1) && (cl == 1 || cl == 2 || cl == 3)) { e.a3000(e.c, unit); return; }
    switch ((int16_t)rd16(rec, 0x2E)) {
    case 0x5F: {
        const uint32_t r2 = e.f1260(e.c, rd(unit, 4));
        if (!r2) return;
        e.a3000(e.c, unit);
        if ((int32_t)rd(unit, 0x44) >> 8 > (int32_t)rd8(r2, 0x109)) wr(unit, 0x44, rd8(r2, 0x10A) << 8);
        return;
    }
    case 0x7E:
        e.a3000(e.c, unit);
        if ((int32_t)(rd(unit, 0x44) & 0xFFFFFF00u) >= 0xC00) wr(unit, 0x44, 0x800);
        return;
    case 0x98: {
        const uint32_t r2 = e.f1260(e.c, rd(unit, 4));
        if (!r2) return;
        if (!e.w10494(e.c, unit, 0xC)) return;
        const int32_t ebx = (int32_t)rd(unit, 0x44) >> 8;
        if (ebx >= (int32_t)rd8(r2, 0x109)) e.a3000(e.c, unit);
        if (ebx < (int32_t)rd8(r2, 0x10A)) return;
        const uint32_t v = e.f1330(e.c, 4, unit + 0x20);
        wr(unit, 0x44, (v + rd8(r2, 0x10A) - 4) << 8);
        return;
    }
    default: return;
    }
}
}
