// 1.13c D2Game+0x1F600: compress a monster unit into a 0x5C-byte inactive record and push it on the per-type
// list. EAX = unit, stack (ret 4) = pGame. Monster-only: retail dereferences null when the unit is not type 1 or has no pData (not modelled).
// The callees are seams (thunk RVAs): A478 #10331 (room of unit), A68E/A5D4/ABB6/ABAA the sgptDataTables+0x174 short-list pass, 8E6F0 (EAX = event,
// EDI = game), A2E6 Fog alloc (ECX = game+0x1C, EDX = 0x5C), 1EF50 (EDI = unit; game, rec), E3B00 (ECX = unit), 1E960 (EBX = out[0]; game, type, out[1], game).
#include <cstdint>
#include <cstring>
struct D2G1F600Seams {
    uint32_t (*fA478)(uint32_t unit);
    uint32_t (*fA68E)(uint32_t unit, uint32_t row);
    uint32_t (*fA5D4)(uint32_t unit, uint32_t row);
    void     (*fABB6)(uint32_t unit, uint32_t p);
    void     (*fABAA)(uint32_t p);
    void     (*f8E6F0)(uint32_t ev, uint32_t game);
    uint32_t (*fA2E6)(uint32_t pool, uint32_t size);
    uint32_t (*fA454)(uint32_t p);
    uint32_t (*fA3B8)(uint32_t p);
    void     (*f1EF50)(uint32_t unit, uint32_t game, uint32_t rec);
    uint32_t (*fA556)(uint32_t v);
    uint32_t (*fA56E)(uint32_t v);
    void     (*fE3B00)(uint32_t unit);
    uint32_t (*fA514)(uint32_t unit);
    uint32_t (*fA42A)(uint32_t unit, uint32_t a, uint32_t b);
    uint32_t (*fA3A0)(uint32_t unit);
    void     (*fA6D0)(uint32_t room, uint32_t* out);
    uint32_t (*f1E960)(uint32_t ebx, uint32_t game, uint32_t type, uint32_t out1, uint32_t game2);
};
static uint32_t r32(uint32_t p, uint32_t o = 0) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static uint32_t r16(uint32_t p, uint32_t o) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)(p + o), 2); return v; }
static uint32_t r8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }
static void w32(uint32_t p, uint32_t o, uint32_t v) { memcpy((void*)(uintptr_t)(p + o), &v, 4); }
static void w16(uint32_t p, uint32_t o, uint32_t v) { uint16_t x = (uint16_t)v; memcpy((void*)(uintptr_t)(p + o), &x, 2); }
extern "C" void d2game_1f600(uint32_t unit, uint32_t game, uint32_t tables, const D2G1F600Seams* s)
{
    const uint32_t room = s->fA478(unit);
    if (unit == 0 || room == 0) return;
    for (int32_t i = 0; i < (int16_t)r16(tables, 0x178); i++) {
        const uint32_t row = (uint32_t)(int32_t)(int16_t)r16(r32(tables, 0x174), 2 * i);
        if (s->fA68E(unit, row) != 0) {
            const uint32_t p = s->fA5D4(unit, row);
            if (p != 0) { s->fABB6(unit, p); s->fABAA(p); }
        }
    }
    for (uint32_t ev = r32(unit, 0xdc); ev != 0;) {
        const uint32_t next = r32(ev, 0x24);
        if (r8(ev, 0) == 2) s->f8E6F0(ev, game);
        ev = next;
    }
    const uint32_t rec = s->fA2E6(r32(game, 0x1c), 0x5c);
    for (uint32_t o = 0; o < 0x5c; o += 4) w32(rec, o, 0);
    w32(rec, 0x1c, 0xffffffff);
    const uint32_t t = r32(unit, 0);
    const uint32_t d = r32(unit, 0x14);
    const bool md = t == 1 && d != 0;
    const uint32_t u2c = r32(unit, 0x2c);
    const bool direct = t == 2 || (3 < (int32_t)t && (int32_t)t < 6);
    w32(rec, 0x00, direct ? r32(u2c, 0xc) : (u2c == 0 ? 0 : s->fA454(u2c)));
    w32(rec, 0x04, direct ? r32(u2c, 0x10) : (u2c == 0 ? 0 : s->fA3B8(u2c)));
    w32(rec, 0x08, r32(unit, 4));
    w32(rec, 0x54, r32(game, 0xa8));
    const uint32_t blk0 = md ? r32(d, 0x28) : 0;
    w32(rec, 0x28, blk0 ? r32(blk0, 0) : 0);
    w16(rec, 0x30, md ? r16(d, 0x14) : 0x1506);
    s->f1EF50(unit, game, rec);
    uint32_t fl = r32(rec, 0x18);
    if (md && (r8(d, 0x16) & 4)) fl |= 2;
    w32(rec, 0x18, fl);
    if (t == 1) {
        if (d != 0 && (r8(d, 0x16) & 2)) {
            w32(rec, 0x18, r32(rec, 0x18) | 0x800);
            w16(rec, 0x3c, md ? r16(d, 0x26) : 0xffff);
        }
        if (d != 0 && (r8(d, 0x16) & 1)) {
            w32(rec, 0x18, r32(rec, 0x18) | 1);
            const uint32_t ai = r32(unit, 0x74);
            if (ai != 0) {
                const uint32_t kind = s->fA56E(s->fA556(s->fA478(unit))) & 0xff;
                if (kind == 2) {
                    const uint32_t a0 = r32(ai, 0);
                    if (r32(a0, 0xe0) == 0x14 && r8(a0, 0xa) == 1) s->fE3B00(unit);
                }
            }
        }
    }
    if (md && (r8(d, 0x16) & 0x10)) w32(rec, 0x18, r32(rec, 0x18) | 0x20);
    if (md && (r8(d, 0x5c) & 2)) w32(rec, 0x18, r32(rec, 0x18) | 0x40);
    const uint32_t mode = s->fA514(unit);
    if (mode == 2) w32(rec, 0x18, r32(rec, 0x18) | 0x100);
    else if (mode == 0) w32(rec, 0x18, r32(rec, 0x18) | 0x200);
    else if (mode == 1) w32(rec, 0x18, r32(rec, 0x18) | 0x80);
    const int32_t v = (int32_t)r32(unit, 0xd0);
    if (v < 8) return;                                   // retail logs a fatal error and exits here (not modelled)
    if (v != 0xb) w32(rec, 0x18, r32(rec, 0x18) | 0x400);
    w32(rec, 0x40, s->fA42A(unit, 0xd, 0));
    w32(rec, 0x44, s->fA3A0(unit));
    w32(rec, 0x48, s->fA42A(unit, 6, 0));
    w32(rec, 0x2c, md ? r32(d, 0x58) : 0);
    if (md) for (uint32_t k = 0; k < 9; k++) *(uint8_t*)(uintptr_t)(rec + 0x32 + k) = (uint8_t)r8(d, 0x1c + k);
    const uint32_t ds = r32(unit, 0x10);
    if (ds == 0xc || ds == 0) w32(rec, 0x18, r32(rec, 0x18) | 4);
    if (t == 1) {
        const uint32_t blk = d ? r32(d, 0x28) : 0;
        if (r32(blk, 0x30) == 1) {
            const uint32_t b8 = r8(blk, 8), a = r32(blk, 0x2c), b = r32(blk, 0x34);
            w32(blk, 0x2c, 0xffffffff);
            w32(blk, 0x34, 0);
            if (a != 0xffffffff) {
                w32(rec, 0x1c, a);
                w32(rec, 0x20, b);
                if (b8 & 1) w32(rec, 0x18, r32(rec, 0x18) | 8);
                if (b8 >> 1 & 1) w32(rec, 0x18, r32(rec, 0x18) | 0x10);
            }
        }
    }
    w32(rec, 0x0c, r32(unit, 0xc));
    w32(rec, 0x10, r32(unit, 0xc4));
    w32(rec, 0x14, r32(unit, 0xc8));
    uint32_t out[2] = { 0, 0 };
    s->fA6D0(room, out);
    const uint32_t ty = s->fA56E(s->fA556(room)) & 0xff;
    const uint32_t list = s->f1E960(out[0], game, ty, out[1], 1);   // retail pushes the constant 1 (EBX is 1 on every monster path)
    w32(rec, 0x58, r32(list, 0xc));
    w32(list, 0xc, rec);
}
