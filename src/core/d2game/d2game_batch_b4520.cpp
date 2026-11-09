// src/core/d2game/d2game_batch_b4520.cpp -- twelve 1.13c D2Game.dll functions (0xB4520..0xB6C90) transcribed
// from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Callees are two env seams: ord(n,...) = D2Common ordinal n, sub(rva,...) = D2Game+rva, arguments in
// retail order (register args first). Register-only inputs (in_EAX/unaff_ESI/EBX/EDI/EBP) are explicit
// parameters. dt = *sgptDataTables: +0xB98/+0xBA0 skills base/count (0x23c stride), +0xB64/+0xB6C
// (0x1a4 stride). Stack-garbage reads in the decompile (uninitialised locals) are taken as the nearest
// live value and flagged "ponytail:"; the VECTORED pass settles them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef uint32_t u32; typedef int32_t i32;
static inline u32 rd32(const void* p, u32 o) { u32 v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, u32 o, u32 v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline int16_t rd16s(const void* p, u32 o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2GameBatchB4520Env {
    i32 (*ord)(u32 n, u32 a, u32 b, u32 c, u32 d);
    i32 (*sub)(u32 rva, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
    const uint8_t* dt;      // *sgptDataTables_exref
    const uint8_t* tbl1B510; // D2Game+... DAT_6fd1b510 (byte[8]) then DAT_6fd1b518 rows of 9
    const uint8_t* tbl1B518;
};
#define ORD(n, ...) env->ord(n, ##__VA_ARGS__)
// skill record by id, or null (shared prologue bounds test)
static const uint8_t* skillRec(const D2GameBatchB4520Env* e, i32 id) {
    if (id < 0 || id >= (i32)rd32(e->dt, 0xba0)) return nullptr;
    return (const uint8_t*)(uintptr_t)(rd32(e->dt, 0xb98) + (u32)id * 0x23c);
}
static bool missileIdOk(const D2GameBatchB4520Env* e, const uint8_t* s) {
    i32 m = rd16s(s, 0x48); return m >= 0 && m < (i32)rd32(e->dt, 0xb6c);
}
#define U(x) ((u32)(x))
#define SUB(r, ...) env->sub(r, ##__VA_ARGS__)
// zeroed 0x17-dword missile/event descriptor
struct Desc { u32 w[0x17]; };

// 1.13c D2Game+0xB4520  (EAX = skill id, ESI = flags word; stack: game, unit, a2). The descriptor is 0x70 zero bytes: w[0] flags, w[1] = the
// 0xDE5A0 result in its low word, w[2] = the 10786 count, w[0x18] = the row's +0x134 word when set. SUB(0x9F210/0xDDDE0/0xDE5A0) take their register inputs first.
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B4520(u32 game, void* unit, i32 skillId, u32 esi, u32 a2, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !unit) return 0;
    const u32 un = U((uintptr_t)unit);
    ORD(11129, esi, un, 0, 0);
    u32 d[0x1c]; std::memset(d, 0, sizeof d);
    u32 o = U(ORD(10653, esi, U(skillId), a2, 0));
    u32 ax = SUB(0xDE5A0, un, esi, game, o, 0, 0, 0) & 0xffff;   // EAX = unit, EDI = esi, stack game, o, 0
    d[1] = ax;
    if (ax & 1) {
        uint16_t w; std::memcpy(&w, s + 0x12e, 2);
        d[1] = (ax | w) & 0xffff;
        d[0] |= rd32(s, 0x130);
        if (rd32(s, 0x134)) d[0x18] = rd32(s, 0x134);
        d[2] = U(ORD(10786, esi, rd32(s, 0x13c), U(skillId), a2));
        SUB(0x9F210, U(skillId), esi, U((uintptr_t)d), a2, 0, 0, 0);   // EAX = skill id, EDI = esi, stack desc, a2
    }
    SUB(0xDDDE0, esi, U((uintptr_t)d), game, un, 0, 0, 0);   // EAX = esi, CL = 0x80, EDI = desc, stack game, unit
    SUB(0xDED30, game, esi, un, 0, 0, 0, 0);
    i32 h = ORD(10511, esi, 0, 0, 0);
    if (h) ORD(10243, U(h), (d[0] & 1) ? 1 : 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB46B0  (__fastcall ECX = game, EDX = unit; ret 8, the two stack words unused). #10983/#10764 read the unit's room; #10331(unit)
// feeds 0x1280 (EAX = that result, EBX = the #10983 result, EDI = the #10764 result), whose result is the room handed to 0xE19C0 (EAX = #10764
// result, ECX = #10983 result, EDX = room; stack game, unit, 0, 0). 0xE03A0 takes EAX = node+0x30, ECX = node+0x28, EDX = node+0x2c.
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B46B0(u32 game, void* unit, const D2GameBatchB4520Env* env)
{
    u32 U_ = U((uintptr_t)unit);
    u32 room = rd32(unit, 0x2c);
    u32 v = ORD(10983, room, 0, 0, 0);
    u32 w = ORD(10764, room, 0, 0, 0);
    u32 r = ORD(10331, U_, 0, 0, 0);
    u32 z = SUB(0x1280, r, v, w, 0, 0, 0, 0);
    if (z == 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14)) {
        u32 p = rd32((const void*)(uintptr_t)rd32(unit, 0x14), 0x28);
        if (p && rd32((const void*)(uintptr_t)p, 0x28)) {
            u32 q = SUB(0xE03A0, rd32((const void*)(uintptr_t)p, 0x30), rd32((const void*)(uintptr_t)p, 0x28), rd32((const void*)(uintptr_t)p, 0x2c), 0, 0, 0, 0);
            if (q) z = ORD(10331, q, 0, 0, 0);
        }
    }
    if (!SUB(0xE19C0, w, v, z, game, U_, 0, 0)) return 0;
    ORD(10158, U_, 0, 0, 0);
    wr32(unit, 0xc8, rd32(unit, 0xc8) | 0x10000);
    return 1;
}

// 1.13c D2Game+0xB4870  (ESI = unit; stack: game, ret 4). A room-less unit, or a room for which #10203 answers 0, returns 0. #10266(#10788(room)) picks a byte of the
// 8-entry table {a,8,16,14,12,10,e,c}; #10740 takes the dword of the stack table with that byte in place and writes the step pair (dx, dy). #10630(ECX = unit,
// EDX = 0xAD) is a plain-ret fastcall: the `push 1` before it stays on the stack and is the third word of #10306(unit, result, 1) (ret 0xC). Types 2, 4 and 5 read the
// room's +0xC / +0x10; the others ask #10867 / #10750. 0x1280 (EAX = #10331(unit), EBX = x, EDI = y) has its result dropped. The 0x17-dword request goes to
// 0x6F930 (ECX = game, EDX = request): {0x21, unit, 0, 0x8F, x, y, 0, x + 2 dx, y + 2 dy, 0, 3, 0xAD, #10306 result}.
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_B4870(void* unit, u32 game, const D2GameBatchB4520Env* env)
{
    u32 room = rd32(unit, 0x2c);
    if (!room) return nullptr;
    if (!ORD(10203, room, 0, 0, 0)) return nullptr;
    const u32 un = U((uintptr_t)unit);
    u32 k = U(ORD(10788, room, 0, 0, 0));
    u32 b = U(ORD(10266, k, 0, 0, 0)) & 0xff;
    static const uint8_t tab[8] = { 0xa, 8, 0x16, 0x14, 0x12, 0x10, 0xe, 0xc };
    // ponytail: an index past the table reads stack residue in retail; the vectors keep it below 8
    u32 w0 = (b < 8 ? tab[b] : 0u) | 0x08u << 8 | 0x16u << 16 | 0x14u << 24;
    i32 dxy[2] = { 0, 0 };
    ORD(10740, w0, U((uintptr_t)&dxy[0]), U((uintptr_t)&dxy[1]), 0);
    u32 r6 = U(ORD(10630, un, 0xad, 0, 0));
    u32 r3 = U(ORD(10306, un, r6, 1, 0));
    i32 t = (i32)rd32(unit, 0);
    u32 ex, ey;
    if (t == 2 || (t >= 4 && t <= 5)) { u32 rm = rd32(unit, 0x2c); ex = rd32((const void*)(uintptr_t)rm, 0xc); ey = rd32((const void*)(uintptr_t)rm, 0x10); }
    else {
        u32 rm = rd32(unit, 0x2c);
        ex = rm ? U(ORD(10867, rm, 0, 0, 0)) : 0;
        ey = rm ? U(ORD(10750, rm, 0, 0, 0)) : 0;
    }
    u32 x = ex + U(dxy[0]), y = ey + U(dxy[1]);
    u32 uc = U(ORD(10331, un, 0, 0, 0));
    SUB(0x1280, uc, x, y, 0, 0, 0, 0);
    Desc d{};
    d.w[0] = 0x21; d.w[1] = un; d.w[3] = 0x8f; d.w[4] = x; d.w[5] = y;
    d.w[7] = x + 2 * U(dxy[0]); d.w[8] = y + 2 * U(dxy[1]);
    d.w[10] = 3; d.w[11] = 0xad; d.w[12] = r3;
    return (void*)(uintptr_t)SUB(0x6F930, game, U((uintptr_t)&d), 0, 0, 0, 0, 0);
}

// Missile direction step used by B4DE0: (dx,dy) -> (x step, y step), the ladder of the 1.13c body.
static void dirStep(i32 dx, i32 dy, i32* a, i32* b)
{
    i32 sa = 0, sb = 0;
    if (dx < 0) { sb = -2; sa = dy < 0 ? 2 : (dy > 0 ? -2 : 0); }
    else if (dx == 0) { sa = dy < 0 ? 2 : (dy > 0 ? -2 : 0); }
    else { sb = 2; sa = dy < 0 ? 2 : (dy > 0 ? -2 : 0); }
    *a = sa; *b = sb;
}

// 1.13c D2Game+0xB4DE0  (__fastcall ECX = game, EDX = unit; stack: skill id, p4)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B4DE0(void* game, void* unit, i32 skillId, u32 p4,
                                                       const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    i32 tx, ty;   // 9FA30: ECX = game, EAX = unit, EBX = &tx, stack = &ty
    if (!SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)unit), U((uintptr_t)&tx), U((uintptr_t)&ty), 0, 0, 0)) return 0;
    i32 ux = SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0), uy = SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    i32 dx = tx - ux, dy = ty - uy;
    if (!dx && !dy) return 0;
    i32 xs, ys; dirStep(dx, dy, &xs, &ys);
    i32 n = ORD(10786, U((uintptr_t)unit), rd32(s, 0x138), U(skillId), p4);
    dx -= (n / 2) * xs; dy -= (n / 2) * ys;
    Desc d{}; d.w[0] = 3;
    for (; n > 0; --n) {
        d.w[1] = U((uintptr_t)unit);
        d.w[4] = U(mi);
        d.w[5] = U(dx / 6 + ux); d.w[6] = U(dx / 6 + uy);
        d.w[7] = U(dx); d.w[8] = U(dy);
        d.w[0xC] = p4;
        SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
        dx += xs; dy += ys;
    }
    return 1;
}

// 1.13c D2Game+0xB4FF0  (ECX = game, EDX = unit; stack: skill id, p4 = skill level). The descriptor is a
// D2MissileStrc-shaped block: +0x10 missile, +0x14/+0x18 position, +0x1C/+0x20 target (written by 9FA30),
// +0x30 level, +0x54 callback, +0x58 shot index.
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B4FF0(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    i32 n = ORD(10786, U((uintptr_t)unit), rd32(s, 0x138), U(skillId), p4);
    Desc d{};
    d.w[0] = 0x21; d.w[1] = U((uintptr_t)unit);
    d.w[5] = U(SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    d.w[6] = U(SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    d.w[0xC] = p4; d.w[4] = U(mi);
    d.w[0x15] = 0xB20B31D0u;   // retail callback (RVA 0xB31D0); the capture logs a code pointer as 0xB2000000 | RVA
    // 9FA30: ECX = game, EAX = unit, EBX = &w[7], stack = &w[8]
    if (!SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)unit), U((uintptr_t)&d.w[7]), U((uintptr_t)&d.w[8]), 0, 0, 0)) return 0;
    for (i32 i = 0; i < n; ++i) {
        d.w[0x16] = U(i);
        SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    }
    return 1;
}

// 1.13c D2Game+0xB5110  (ECX = game, EDX = unit; stack: skill id, p4 = skill level)
// Retail call order: 1100(unit), 1130(unit), 10786, E07A0, 10392; the end vector re-reads the unit position (1100/1130) after the
// room lookups, it does not reuse the first read.
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B5110(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    if (!(rd32(env->dt, 0xb64) + (u32)mi * 0x1a4)) return 0;
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    const u32 un = U((uintptr_t)unit);
    Desc d{}; d.w[0] = 3; d.w[1] = d.w[2] = un; d.w[4] = U(mi);
    d.w[5] = U(SUB(0x1100, un, 0, 0, 0, 0, 0, 0));
    d.w[6] = U(SUB(0x1130, un, 0, 0, 0, 0, 0, 0));
    d.w[0xB] = U(skillId); d.w[0xC] = p4;
    d.w[0xD] = U(ORD(10786, un, rd32(s, 0x138), U(skillId), p4));
    SUB(0xE07A0, U((uintptr_t)game), un, 0, 0, 0, 0, 0);
    u32 room = rd32(unit, 0x2c);
    u32 t = U(ORD(10392, room, 0, 0, 0));
    i32 ex, ey;
    if (t == un || !t) {
        i32 v = ORD(10983, room, 0, 0, 0);
        ex = v - SUB(0x1100, un, 0, 0, 0, 0, 0, 0);
        i32 w = ORD(10764, room, 0, 0, 0);
        ey = w - SUB(0x1130, un, 0, 0, 0, 0, 0, 0);
    } else {
        i32 ty = SUB(0x1130, t, 0, 0, 0, 0, 0, 0);
        i32 tx = SUB(0x1100, t, 0, 0, 0, 0, 0, 0);
        ORD(10739, room, U(tx), U(ty), 0);
        i32 tx2 = SUB(0x1100, t, 0, 0, 0, 0, 0, 0);
        i32 ux2 = SUB(0x1100, un, 0, 0, 0, 0, 0, 0);
        ex = tx2 - ux2;
        i32 ty2 = SUB(0x1130, t, 0, 0, 0, 0, 0, 0);
        i32 uy2 = SUB(0x1130, un, 0, 0, 0, 0, 0, 0);
        ey = ty2 - uy2;
    }
    d.w[7] = U(ex); d.w[8] = U(ey);
    SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB5450  (ECX = game, EDX = unit; stack: skill id, p4 = skill level)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B5450(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    SUB(0xE07A0, U((uintptr_t)game), U((uintptr_t)unit), 0, 0, 0, 0, 0);
    u32 room = rd32(unit, 0x2c);
    void* t = (void*)(uintptr_t)ORD(10392, room, 0, 0, 0);
    if (t == unit || !t) return 0;
    i32 tx = SUB(0x1100, U((uintptr_t)t), 0, 0, 0, 0, 0, 0), ty = SUB(0x1130, U((uintptr_t)t), 0, 0, 0, 0, 0, 0);
    i32 ux = SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0), uy = SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    i32 ax = tx - ux; if (ax < 0) ax = -ax;
    i32 ay = ty - uy; if (ay < 0) ay = -ay;
    i32 px, py;
    if (ax < ay) { px = ux; for (int k = 0; k < 4; ++k) px += tx < px ? -1 : (px < tx ? 1 : 0); py = ty; }
    else { py = uy; for (int k = 0; k < 4; ++k) py += ty < py ? -1 : (py < ty ? 1 : 0); px = tx; }
    Desc d{};
    i32 ex = px - ux, ey = py - uy;
    d.w[0] = 3; d.w[1] = U((uintptr_t)unit); d.w[4] = U(mi);
    d.w[5] = U(ex / 6 - 1 + ux); d.w[6] = U(ey / 6 - 1 + uy);
    d.w[7] = U(ex); d.w[8] = U(ey);
    d.w[0xB] = U(skillId); d.w[0xC] = p4;
    SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB56C0  (ECX = game, EDX = unit; stack: skill id, p4). The per-class offset tables are the module bytes at
// D2Game+0xFB510 (8 bytes, one per class) and +0xFB518 (8 rows of 9).
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B56C0(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    const u32 un = U((uintptr_t)unit);
    i32 h = ORD(10511, un, 0, 0, 0);
    if (!h) return 0;
    i32 a = ORD(10980, U(h), 0, 0, 0);
    i32 b = ORD(10649, U(h), 0, 0, 0);
    if (!a || !b) {   // 9FA30: ECX = game, EAX = unit, EBX = &a, stack = &b
        if (!SUB(0x9FA30, U((uintptr_t)game), un, U((uintptr_t)&a), U((uintptr_t)&b), 0, 0, 0)) return 0;
    }
    u32 w = (U(a) & ~0xffu) | (U(ORD(10192, un, U(a), U(b), 0)) & 0xff);
    u32 cls = U(ORD(10266, w, 0, 0, 0)) & 7;
    i32 ux = SUB(0x1100, un, 0, 0, 0, 0, 0, 0);
    i32 uy = SUB(0x1130, un, 0, 0, 0, 0, 0, 0);
    i32 o1 = 0, o2 = 0, junk = 0;
    ORD(10740, env->tbl1B510[cls], U((uintptr_t)&o1), U((uintptr_t)&o2), 0);
    ux += o1; uy += o2;
    i32 i = uy;   // the frame slot 10569 fills holds the spilled uy until it writes
    ORD(10569, un, U((uintptr_t)&i), U((uintptr_t)&junk), 0);
    i32 idx = i - 4;
    if (idx <= 0) idx = 0; else if (idx >= 8) idx = 8;
    uint8_t v = env->tbl1B518[cls * 9 + (u32)idx];
    i32 w8 = b;   // retail leaves the 10649 slot as the y word when the class has no second offset ('c')
    if (v != 'c') {
        w = (w & ~0xffu) | v;
        ORD(10740, w, U((uintptr_t)&o1), U((uintptr_t)&o2), 0);
        ux += o1; uy += o2;
        w8 = uy;
    }
    Desc d{}; d.w[0] = 0x20; d.w[1] = d.w[2] = un; d.w[4] = U(mi);
    d.w[7] = U(ux); d.w[8] = U(w8); d.w[0xC] = p4;
    SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB6720  (EAX = game, EBX = unit): common tail of B6840/B6A60. ae6f0 takes EAX = node, EDI = game.
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6720(void* game, void* unit, const D2GameBatchB4520Env* env)
{
    u32 n = 0xb;
    if (unit) {
        if (rd32(unit, 0) == 1) {
            i32 p = SUB(0x1320, rd32(unit, 4), 0, 0, 0, 0, 0, 0);
            if (p) n = *((const uint8_t*)(uintptr_t)p + 0x108);
        }
        for (u32 q = rd32(unit, 0xdc); q; ) {
            u32 cur = q;
            uint8_t c = *(const uint8_t*)(uintptr_t)q; q = rd32((const void*)(uintptr_t)q, 0x24);
            if (c == 0) SUB(0x8E6F0, cur, U((uintptr_t)game), 0, 0, 0, 0, 0);
        }
    }
    SUB(0x8EA80, U((uintptr_t)game), U((uintptr_t)unit), 1, rd32(game, 0xa8) + n, 0, 0, 0);
    return 1;
}

// ECX = game, EDX = unit; stack: skill id, p4 = skill level. The game pointer doubles as the context (+0xA8 tick).
static u32 b6840(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    i32 h;
    if (!s || !(h = ORD(10511, U((uintptr_t)unit), 0, 0, 0))) return 0;
    i32 mi = rd16s(s, 0x48);
    if (mi < 0 || mi >= (i32)rd32(env->dt, 0xb6c)) return 0;
    const uint8_t* r = (const uint8_t*)(uintptr_t)SUB(0x1250, U(mi), 0, 0, 0, 0, 0, 0);
    Desc d{}; d.w[0] = 0x25; d.w[1] = U((uintptr_t)unit);
    d.w[5] = U(SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    d.w[6] = U(SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    i32 m = (i32)(r[0x9b] * p4);
    d.w[0xA] = U(((m + ((m >> 31) & 7)) >> 3) + r[0x9a]);
    d.w[0xC] = p4; d.w[4] = U(mi);
    // 9FA30: ECX = game, EAX = unit, EBX = &w[7], stack = &w[8]
    u32 mon = 0;
    if (SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)unit), U((uintptr_t)&d.w[7]), U((uintptr_t)&d.w[8]), 0, 0, 0) &&
        (mon = U(SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0)))) {
        SUB(0xB32E0, U(skillId), mon, U((uintptr_t)unit), p4, 0, 0, 0);   // EAX = skill id, EBX = the new missile, stack = unit, level
        SUB(0xB3270, U((uintptr_t)unit), U((uintptr_t)s), 0, 0, 0, 0, 0);   // ESI = unit, stack = the skill record
        i32 now = ORD(10980, U(h), 0, 0, 0);
        if ((i32)rd32(game, 0xa8) < now && ORD(10494, U((uintptr_t)unit), 0xc, 0, 0)) {
            i32 n = ORD(10786, U((uintptr_t)unit), rd32(s, 0x140), U(skillId), p4);
            if (n < 1) n = 1;
            if (unit) {
                for (int pass = 1; pass >= 0; --pass)
                    for (u32 q = rd32(unit, 0xdc); q; ) {
                        u32 cur = q;
                        uint8_t c = *(const uint8_t*)(uintptr_t)q; q = rd32((const void*)(uintptr_t)q, 0x24);
                        if (c == pass) SUB(0x8E6F0, cur, U((uintptr_t)game), 0, 0, 0, 0, 0);
                    }
            }
            SUB(0x8EA80, U((uintptr_t)game), U((uintptr_t)unit), 0, rd32(game, 0xa8) + U(n), 0, 0, 0);
            return 1;
        }
        d2_fn_d2game_B6720(game, unit, env);
        return 1;
    }
    d2_fn_d2game_B6720(game, unit, env);
    return 0;
}
// 1.13c D2Game+0xB6840  (the two bodies are byte-identical)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6840(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{ return b6840(game, unit, skillId, p4, env); }
// 1.13c D2Game+0xB6A60
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6A60(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{ return b6840(game, unit, skillId, p4, env); }

// 1.13c D2Game+0xB6C90  (ESI = unit, EDI = the first argument of #10306)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6C90(void* unit, u32 edi, const D2GameBatchB4520Env* env)
{
    if (!unit) return 0;
    i32 k = ORD(10601, U((uintptr_t)unit), 0x6fd1b434u, 0x131, 0);
    if (k == 0x3b || k == 0x38) return 1;
    if (k == 0x33) return (i32)ORD(10306, edi, U((uintptr_t)unit), 1, 0) > 3;
    if (k == 0x1b) return (i32)ORD(10306, edi, U((uintptr_t)unit), 1, 0) > 7;
    return 0;
}
