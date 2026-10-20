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

// 1.13c D2Game+0xB4520  (EAX = skill id, ESI = flags word)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B4520(u32 game, void* unit, i32 skillId, u32 esi, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !unit) return 0;
    ORD(11129, 0, 0, 0, 0);
    u32 flags = esi;
    u32 o = ORD(10653, 0, 0, 0, 0);
    // ponytail: piStack_10/piStack_c are the (game, unit) pair on the stack
    u32 r = SUB(0xDE5A0, game, o, esi, 0, 0, 0, 0) & 0xffff;
    if (r & 1) {
        flags = esi | rd32(s, 0x130);
        ORD(10786, 0, 0, 0, 0);
        SUB(0x9F210, 0, 0, 0, 0, 0, 0, 0);
    }
    SUB(0xDDDE0, 0x80, game, U((uintptr_t)unit), 0, 0, 0, 0);
    SUB(0xDED30, game, esi, U((uintptr_t)unit), 0, 0, 0, 0);
    i32 h = ORD(10511, 0, 0, 0, 0);
    if (h) ORD(10243, U(h), (flags & 1) ? 1 : 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB46B0  (__fastcall; param_2 = unit; ESI is the 3rd arg to 0xE19C0)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B46B0(u32 ecx, void* unit, u32 esi, const D2GameBatchB4520Env* env)
{
    u32 U_ = U((uintptr_t)unit);
    u32 room = rd32(unit, 0x2c);
    u32 v = ORD(10983, room, 0, 0, 0);
    ORD(10764, room, 0, 0, 0);
    i32 r = ORD(10331, U_, 0, 0, 0);
    i32 z = SUB(0x1280, 0, 0, 0, 0, 0, 0, 0);
    if (z == 0 && rd32(unit, 0) == 1 && rd32(unit, 0x14)) {
        u32 p = rd32((const void*)(uintptr_t)rd32(unit, 0x14), 0x28);
        if (p && rd32((const void*)(uintptr_t)p, 0x28)) {
            i32 q = SUB(0xE03A0, rd32((const void*)(uintptr_t)p, 0x28), rd32((const void*)(uintptr_t)p, 0x2c), 0, 0, 0, 0, 0);
            if (q) r = ORD(10331, U(q), 0, 0, 0);
        }
    }
    if (!SUB(0xE19C0, v, U(r), esi, U_, 0, 0, 0)) return 0;
    ORD(10158, U_, 0, 0, 0);
    wr32(unit, 0xc8, rd32(unit, 0xc8) | 0x10000);
    return 1;
}

// 1.13c D2Game+0xB4870  (ESI = unit)
extern "C" EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_B4870(void* unit, const D2GameBatchB4520Env* env)
{
    u32 room = rd32(unit, 0x2c);
    if (!room) return nullptr;
    if (!ORD(10203, room, 0, 0, 0)) return nullptr;
    ORD(10788, 0, 0, 0, 0);
    u32 b = U(ORD(10266, 0, 0, 0, 0)) & 0xff;
    // ponytail: the 0x10740 first arg is a stack-resident per-class byte table indexed by b
    ORD(10740, b, U((uintptr_t)&room), 0, 0);
    ORD(10630, 1, 0, 0, 0);
    ORD(10306, 0, 0, 0, 0);
    i32 t = (i32)rd32(unit, 0);
    if (t != 2 && (t < 4 || t > 5) && rd32(unit, 0x2c)) ORD(10867, rd32(unit, 0x2c), 0, 0, 0);
    t = (i32)rd32(unit, 0);
    if (t != 2 && (t < 4 || t > 5) && rd32(unit, 0x2c)) ORD(10750, rd32(unit, 0x2c), 0, 0, 0);
    ORD(10331, 0, 0, 0, 0);
    SUB(0x1280, 0, 0, 0, 0, 0, 0, 0);
    Desc d{}; d.w[0] = 0x21;
    return (void*)(uintptr_t)SUB(0x6F930, 0, U((uintptr_t)&d), 0, 0, 0, 0, 0);
}

// Missile direction step used by B4DE0: (dx,dy) -> unit step (sx,sy), in the decompile's sign ladder.
static void dirStep(i32 dx, i32 dy, i32* a, i32* b)
{
    i32 sa = 0, sb = 0;
    if (dx < 0 && dy < 0) { sa = 2; sb = -2; }
    else if (dx == 0) { sa = dy < 0 ? 2 : (dy > 0 ? -2 : 0); sb = -2; }
    else if (dx > 0) { sb = 2; sa = dy < 0 ? 2 : (dy == 0 ? 0 : -2); }
    else if (dy > 0) { sa = -2; sb = -2; }
    else if (dy != 0) { sa = 0; sb = 0; }
    else { sa = 0; sb = -2; }
    *a = sa; *b = sb;
}

// 1.13c D2Game+0xB4DE0  (__fastcall ECX = game; EBX/EDI = step words; param_4 = target)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B4DE0(void* game, void* unit, i32 skillId, u32 p4,
                                                       i32 ebx, i32 edi, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    i32 tgt[2];
    if (!SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)tgt), 0, 0, 0, 0, 0)) return 0;
    i32 ux = SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0), uy = SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    i32 dx = tgt[0] - ux, dy = tgt[1] - uy;
    if (!dx && !dy) return 0;
    i32 sa, sb; dirStep(dx, dy, &sa, &sb);   // ponytail: ladder reconstructed; sa/sb feed iStack_54 / iVar4
    i32 n = ORD(10786, U((uintptr_t)unit), rd32(s, 0x138), U(skillId), p4);
    dx -= (n / 2) * edi; dy -= (n / 2) * sb;
    Desc d{}; d.w[0] = 3;
    for (; n > 0; --n) {
        d.w[1] = U((uintptr_t)unit);
        d.w[2] = U(dx / 6 + ebx); d.w[3] = U(dx / 6 + sa);
        d.w[4] = U(dx); d.w[5] = U(dy);
        SUB(0x6F930, U(tgt[1]), U((uintptr_t)&d), 0, 0, 0, 0, 0);
        dx += edi; dy += sb;
    }
    return 1;
}

// 1.13c D2Game+0xB4FF0  (EBP = game, as in B4DE0)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B4FF0(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    i32 n = ORD(10786, U((uintptr_t)unit), rd32(s, 0x138), U(skillId), p4);
    Desc d{};
    SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    i32 tgt[4];
    if (!SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)tgt), 0, 0, 0, 0, 0)) return 0;
    d.w[0] = 3; d.w[0xF] = p4;   // ponytail: descriptor field slots from the stack layout
    for (i32 i = 0; i < n; ++i) {
        d.w[0x12] = U(i);
        SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    }
    return 1;
}

// 1.13c D2Game+0xB5110  (EDI = game)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B5110(void* game, void* unit, i32 skillId, u32 p4, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    if (!(rd32(env->dt, 0xb64) + (u32)mi * 0x1a4)) return 0;
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    Desc d{}; d.w[0] = 3; d.w[1] = d.w[2] = U((uintptr_t)unit); d.w[0xF] = U(mi);
    d.w[0x10] = U(SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    d.w[0x11] = U(SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    d.w[0x15] = U(skillId); d.w[0x16] = p4;
    d.w[0x13] = U(ORD(10786, U((uintptr_t)unit), rd32(s, 0x138), U(skillId), p4));
    SUB(0xE07A0, U((uintptr_t)game), 0, 0, 0, 0, 0, 0);
    u32 room = rd32(unit, 0x2c);
    void* t = (void*)(uintptr_t)ORD(10392, room, 0, 0, 0);
    if (t == unit || !t) {
        i32 v = ORD(10983, room, 0, 0, 0);
        d.w[1] = U(v - SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
        ORD(10764, room, 0, 0, 0);
    } else {
        u32 ty = U(SUB(0x1130, U((uintptr_t)t), 0, 0, 0, 0, 0, 0)), tx = U(SUB(0x1100, U((uintptr_t)t), 0, 0, 0, 0, 0, 0));
        ORD(10739, room, tx, ty, 0);
    }
    SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB5450  (ECX = game)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B5450(void* game, void* unit, i32 skillId, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 mi = rd16s(s, 0x48);
    if (unit) wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x40);
    SUB(0xE07A0, U((uintptr_t)game), 0, 0, 0, 0, 0, 0);
    u32 room = rd32(unit, 0x2c);
    void* t = (void*)(uintptr_t)ORD(10392, room, 0, 0, 0);
    if (t == unit || !t) return 0;
    i32 tx = SUB(0x1100, U((uintptr_t)t), 0, 0, 0, 0, 0, 0), ty = SUB(0x1130, U((uintptr_t)t), 0, 0, 0, 0, 0, 0);
    i32 ux = SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0), uy = SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    i32 ax = tx - ux; if (ax < 0) ax = -ax;
    i32 ay = ty - uy; if (ay < 0) ay = -ay;
    i32 px = ux, py = uy;
    if (ax < ay) { px = ux; for (int k = 0; k < 4; ++k) px += tx < px ? -1 : (px < tx ? 1 : 0); py = ty; }
    else { py = uy; for (int k = 0; k < 4; ++k) py += ty < py ? -1 : (py < ty ? 1 : 0); px = tx; }
    Desc d{}; d.w[0] = 3; d.w[1] = U((uintptr_t)unit);
    i32 ex = px - ux, ey = py - uy;
    d.w[3] = U(ex / 6 - 1 + ux); d.w[4] = U(ey / 6 - 1 + uy);
    d.w[5] = U(ey); d.w[6] = U(ex);   // ponytail: slot order from the stack layout
    d.w[0x15] = U(skillId);
    SUB(0x6F930, U(mi), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB56C0  (ECX = game)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B56C0(void* game, void* unit, i32 skillId, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    if (!s || !missileIdOk(env, s)) return 0;
    i32 h = ORD(10511, U((uintptr_t)unit), 0, 0, 0);
    if (!h) return 0;
    i32 a = ORD(10980, 0, 0, 0, 0), b = ORD(10649, 0, 0, 0, 0), pos[2];
    if (!a || !b) { if (!SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)pos), 0, 0, 0, 0, 0)) return 0; a = pos[0]; }
    u32 c1 = U(ORD(10192, 0, 0, 0, 0)) & 0xff;
    u32 cls = U(ORD(10266, 0, 0, 0, 0)) & 7;
    SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    i32 y = SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0);
    ORD(10740, env->tbl1B510[cls], 0, 0, 0);
    i32 idx = ORD(10569, U((uintptr_t)unit), U(a), 0, 0);
    if (idx < 1) idx = 0; else if (idx > 7) idx = 8;
    uint8_t v = env->tbl1B518[cls * 9 + idx];
    if (v != 'c') ORD(10740, v, c1, U(y + h), 0);
    Desc d{}; d.w[0] = 0x20;
    SUB(0x6F930, U(h), U((uintptr_t)&d), 0, 0, 0, 0, 0);
    return 1;
}

// 1.13c D2Game+0xB6720  (EAX = game ctx, EBX = unit): common tail of B6840/B6A60.
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6720(void* ctx, void* unit, const D2GameBatchB4520Env* env)
{
    u32 n = 0xb;
    if (unit) {
        if (rd32(unit, 0) == 1) {
            i32 p = SUB(0x1320, 0, 0, 0, 0, 0, 0, 0);
            if (p) n = *((const uint8_t*)(uintptr_t)p + 0x108);
        }
        for (u32 q = rd32(unit, 0xdc); q; ) {
            uint8_t c = *(const uint8_t*)(uintptr_t)q; q = rd32((const void*)(uintptr_t)q, 0x24);
            if (c == 0) SUB(0x8E6F0, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    SUB(0x8EA80, U((uintptr_t)ctx), U((uintptr_t)unit), 1, rd32(ctx, 0xa8) + n, 0, 0, 0);
    return 1;
}

static u32 b6840(void* game, void* unit, i32 skillId, void* ctx, const D2GameBatchB4520Env* env)
{
    const uint8_t* s = skillRec(env, skillId);
    i32 h;
    if (!s || !(h = ORD(10511, U((uintptr_t)unit), 0, 0, 0))) return 0;
    i32 mi = rd16s(s, 0x48);
    if (mi < 0 || mi >= (i32)rd32(env->dt, 0xb6c)) return 0;
    const uint8_t* r = (const uint8_t*)(uintptr_t)SUB(0x1250, 0, 0, 0, 0, 0, 0, 0);
    Desc d{}; d.w[0] = 0x25; d.w[1] = U((uintptr_t)unit);
    d.w[4] = U(SUB(0x1100, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    d.w[5] = U(SUB(0x1130, U((uintptr_t)unit), 0, 0, 0, 0, 0, 0));
    i32 m = (i32)(r[0x9b] * (u32)skillId);
    d.w[0x12 - 0x0] = 0;  // keep layout zero
    d.w[0x0e] = U(((m + ((m >> 31) & 7)) >> 3) + r[0x9a]);
    d.w[0x0c] = U(skillId); d.w[8] = U(mi);
    i32 pos[2];
    if (SUB(0x9FA30, U((uintptr_t)game), U((uintptr_t)pos), 0, 0, 0, 0, 0) &&
        SUB(0x6F930, U((uintptr_t)game), U((uintptr_t)&d), 0, 0, 0, 0, 0)) {
        SUB(0xB32E0, U((uintptr_t)unit), U(skillId), 0, 0, 0, 0, 0);
        SUB(0xB3270, rd32(s, 0), 0, 0, 0, 0, 0, 0);
        i32 now = ORD(10980, 0, 0, 0, 0);
        if ((i32)rd32(ctx, 0xa8) < now && ORD(10494, U((uintptr_t)unit), 0xc, 0, 0)) {
            i32 n = ORD(10786, U((uintptr_t)unit), rd32(s, 0x140), 0, U(skillId));
            if (n < 1) n = 1;
            if (unit) {
                for (int pass = 1; pass >= 0; --pass)
                    for (u32 q = rd32(unit, 0xdc); q; ) {
                        uint8_t c = *(const uint8_t*)(uintptr_t)q; q = rd32((const void*)(uintptr_t)q, 0x24);
                        if (c == pass) SUB(0x8E6F0, 0, 0, 0, 0, 0, 0, 0);
                    }
            }
            SUB(0x8EA80, U((uintptr_t)ctx), U((uintptr_t)unit), 0, rd32(ctx, 0xa8) + U(n), 0, 0, 0);
            return 1;
        }
        d2_fn_d2game_B6720(ctx, unit, env);
        return 1;
    }
    d2_fn_d2game_B6720(ctx, unit, env);
    return 0;
}
// 1.13c D2Game+0xB6840  (ESI = game ctx; the two bodies are byte-identical)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6840(void* game, void* unit, i32 skillId, void* ctx, const D2GameBatchB4520Env* env)
{ return b6840(game, unit, skillId, ctx, env); }
// 1.13c D2Game+0xB6A60
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6A60(void* game, void* unit, i32 skillId, void* ctx, const D2GameBatchB4520Env* env)
{ return b6840(game, unit, skillId, ctx, env); }

// 1.13c D2Game+0xB6C90  (ESI = unit)
extern "C" EMSCRIPTEN_KEEPALIVE u32 d2_fn_d2game_B6C90(void* unit, const D2GameBatchB4520Env* env)
{
    if (!unit) return 0;
    i32 k = ORD(10601, U((uintptr_t)unit), 0, 0, 0);
    if (k == 0x3b || k == 0x38) return 1;
    if (k == 0x33) return ORD(10306, U((uintptr_t)unit), 0, 0, 0) > 3;
    if (k == 0x1b) return ORD(10306, U((uintptr_t)unit), 0, 0, 0) > 7;
    return 0;
}
