// src/core/d2game/d2game_batch_a6b70.cpp -- twelve 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Skill-use handlers (0xA6B70..0xA9610, 0xAAC30), the mercenary/item-use helpers (0xACDA0..0xAD0F0) and the
// two small list walkers (0xAC9E0, 0xACF10). Every callee (D2Common ordinal or unmapped D2Game sub) is a seam
// on D2GameBatchA6B70Env: one generic signature, arguments in retail order (ECX, EDX, stack...), unused tail
// zero. Game pointers are 32-bit handles resolved through env->p(); field reads use the retail offsets.
// Register-only values Ghidra shows as unaff_* / in_EAX are explicit parameters.
// ponytail: the stack-built "fn95e10" request records (0xA7290 / 0xA8270 / 0xA8350) are recovered only
// partially by the decompiler; they are modelled as 8 dwords with the known fields, see each function.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

typedef int64_t (*A6B70Seam)(int64_t, int64_t, int64_t, int64_t, int64_t, int64_t);
struct D2GameBatchA6B70Env {
    void*    (*p)(uint32_t handle);          // handle -> host address (0 -> address of offset 0)
    uint32_t dataTables;                     // *sgptDataTables_exref
    uint32_t bitMasks;                       // gdwBitMasks_exref (array base)
    uint32_t strDat19ff9;                    // &DAT_6fd19ff9
    uint32_t handlerTable;                   // &DAT_6fd27238 (8-byte entries: [0]=pre, [4]=post)
    uint32_t handlerCount;                   // DAT_6fd27330
    uint32_t cb4180, cb3e10;                 // 0x6fcc4180 / 0x6fcc3e10 callbacks
    // ordinals
    A6B70Seam o10024, o10057, o10186, o10202, o10210, o10265, o10306, o10331, o10460, o10464, o10494,
              o10522, o10551, o10621, o10630, o10689, o10695, o10744, o10786, o10804, o10867, o10750,
              o10887, o10910, o10920, o10945, o10973, o10977, o11000, o11028, o11065, o11103, o11138,
              o11147;
    // D2Game subs by RVA
    A6B70Seam s1100, s1130, s21100, s21130, s214d0, s21180, s21260, s21290, s21320, s21560, s215d0, s3e140, s3cae0,
              s41260, s41290, s572c0, s57440, s95e10, sa0000, sae6f0, saea80, saa3e0, sac3b0, sbd230,
              sc4610, sc8e80, sd003a0, sd0a20, sd0e90, sd07b0, sd17b0, sd1bc0, sd1c90, sd11b10, sd11d90,
              sd12220, sd122c0, sd12380, sd12540, sd126b0, sd12870, sd12c10, sd12d80, sd120c0, sccf10;
};

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

namespace {
typedef D2GameBatchA6B70Env Env;
#define R(h, o)   rd32(E->p(h), (o))
#define W(h, o, v) wr32(E->p(h), (o), (v))
#define R16(h, o) rd16(E->p(h), (o))
#define R8(h, o)  (*((const uint8_t*)E->p(h) + (o)))
template <class... A> inline int32_t C(A6B70Seam f, A... a) {
    int64_t v[6] = {0, 0, 0, 0, 0, 0}; int64_t t[] = {(int64_t)a..., 0}; for (size_t i = 0; i < sizeof...(A); i++) v[i] = t[i];
    return (int32_t)f(v[0], v[1], v[2], v[3], v[4], v[5]);
}
inline int64_t A(const void* p) { return (int64_t)(intptr_t)p; }
[[noreturn]] void fatal(const Env* E, uint32_t code) { C(E->o10024, E->strDat19ff9, C(E->o10265, code)); __builtin_trap(); }
// unit->type==1 and unit->player(+0x14) set: that player's +0x2c block, else 0.
inline uint32_t plr(const Env* E, uint32_t u, uint32_t off) {
    if (u && R(u, 0) == 1 && R(u, 0x14)) return R(R(u, 0x14), off);
    return 0;
}
inline uint32_t skillRec(const Env* E, int32_t i) {   // sgptDataTables skills.txt row (0x23c each) or 0
    if (i < 0 || (int32_t)R(E->dataTables, 0xba0) <= i) return 0;
    return i * 0x23c + R(E->dataTables, 0xb98);
}
void walkStates(const Env* E, uint32_t unit) {        // for each state node (+0xdc, next +0x24) of type 2
    for (uint32_t n = R(unit, 0xdc); n;) { uint8_t t = R8(n, 0); n = R(n, 0x24); if (t == 2) C(E->sae6f0); }
}
void failRequest(const Env* E, uint32_t game, uint32_t unit) {   // shared "cannot use" tail of 0xA6B70
    C(E->o11103, unit, 0);
    uint32_t q[8] = {0, unit, 0, 0, 0, 0x6400, 0, 0};
    C(E->s95e10, game, A(q));
}
}  // namespace

// Skill use with a cast delay: validates the skill row, applies the cast-delay start (ordinal #10786), the
// mana/charge consume tail, then queues two room moves (0xD12540) for the two-step motion.
// 1.13c D2Game+0xA6B70
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A6B70(uint32_t game, uint32_t unit, uint32_t cmd, const Env* E)
{
    int32_t  sk = (int16_t)R16(R(cmd, 0x1c), 0x170);
    uint32_t st = R(cmd, 0);
    uint32_t rec = skillRec(E, sk);
    if (rec == 0) { failRequest(E, game, unit); return; }
    int32_t o = C(E->o10630);
    if (o == 0) { failRequest(E, game, unit); return; }
    int32_t sk6 = C(E->o10306, unit, o, 1);
    if ((int32_t)R(st, 0x14) < 0) { int32_t d = C(E->o10786, unit, R(rec, 0x144), sk, sk6); W(st, 0x14, d + R(game, 0xa8)); }
    if ((int32_t)R(game, 0xa8) > (int32_t)R(st, 0x14)) { failRequest(E, game, unit); return; }
    if (R(st, 0x1c) == 0 && R16(rec, 0x48) > -1 && R16(rec, 0x48) < (int32_t)R(E->dataTables, 0xb6c)) {
        uint32_t tgt = C(E->s41260, unit); if (!tgt) tgt = unit;
        C(E->s1130, unit); C(E->s1100, unit);
        uint32_t u4 = C(E->sa0000, game);
        if (u4) {
            C(E->o10210, u4, unit);
            int32_t v = C(E->o10621, tgt); C(E->o10551, unit, 0x13, v, 0);
            int32_t a = C(E->o10973, tgt, 0x77, 0);
            uint32_t pu = C(E->s572c0, tgt);
            if (pu) a += C(E->o10804, tgt, pu, 0, 0);
            C(E->o10551, unit, 0x77, a, 0);
        }
        W(st, 0x1c, 1);
    }
    uint32_t e;
    if (unit && R(unit, 0) == 1 && R(unit, 0x14) && R(R(unit, 0x14), 0x28) && (e = R(R(R(unit, 0x14), 0x28), 0x20))) {
        uint32_t pb = R(R(unit, 0x14), 0x2c);
        W(pb, 0x18, 0xf); *(uint8_t*)E->p(pb + 0x20) = 0x14;
        uint32_t x, y;
        if (R(st, 0x18) == 0) { if (C(E->sd12540, game, unit, 0, 2, R(e, 0xc), (uint8_t)R(e, 0x10), 1)) return;
            W(st, 0x18, R(st, 0x18) == 0); y = (uint8_t)R(e, 0x18); x = R(e, 0x14); }
        else { if (C(E->sd12540, game, unit, 0, 2, R(e, 0x14), (uint8_t)R(e, 0x18), 1)) return;
            W(st, 0x18, R(st, 0x18) == 0); y = (uint8_t)R(e, 0x10); x = R(e, 0xc); }
        if (C(E->sd12540, game, unit, 0, 2, x, y, 1) == 0) {
            uint32_t t = C(E->s41260, unit);
            if (C(E->sd12870, t, game, unit) == 0) C(E->sd122c0, 5);
        }
        return;
    }
    C(E->sd122c0, 3);
}

// Timed skill step (cast-frame check): state 0x38 clear -> reset and queue a delayed event; otherwise the
// speed-percentage (cmd+7 skill row +0x32/+0x34) is folded into the unit's mode block and the move is queued.
// EBP is the game, EBX a caller flag; ECX is the (unused) first argument.
// 1.13c D2Game+0xA6FC0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A6FC0(uint32_t ecx, uint32_t unit, uint32_t cmd, uint32_t game,
                                                         uint32_t ebx, const Env* E)
{
    (void)ecx;
    uint32_t st = R(cmd, 0);
    int32_t  cur = R(cmd, 0x14), tgt = R(cmd, 0x18);
    int32_t  limit; int32_t pct;
    if (C(E->o10494, unit, 0x38) == 0) {
        C(E->sbd230, game, unit);
        if (unit) walkStates(E, unit);
        C(E->saea80, game, unit, 2, R(game, 0xa8) + 1, 0, 0, 0);
        return;
    }
    limit = R(st, 0x14); if (limit == 0) limit = 0x1e;
    if (limit < (int32_t)R(cmd, 0x14)) { C(E->sd122c0, 10); return; }
    int32_t s2 = R16(R(cmd, 0x1c), 0x32), ecxv = s2;
    pct = 0;
    if (s2 > 0) { ecxv = s2; int32_t q = (R16(R(cmd, 0x1c), 0x34) * 100) / s2;
        if (q > 99) { pct = q - 100; if (pct > 0x78) pct = 0x78; } }
    (void)cur;
    auto queue = [&](bool useRet) -> int32_t {
        uint32_t pb = plr(E, unit, 0x2c);
        W(pb, 0x18, 2); if (pct) W(pb, 0x1c, pct);
        if (C(E->s21560, ecxv, 0xf) == 0) return C(E->sd12d80, game, unit, 0x1e);
        (void)useRet; return C(E->sd12c10, game, 0x1e);
    };
    if (R(st, 0x1c) == 0) {
        W(st, 0x1c, 1);
        if (unit) { if (R(unit, 0) == 1 && R(unit, 0x14)) { W(R(unit, 0x14), 0x38, 0); W(R(unit, 0x14), 0x34, 0); } walkStates(E, unit); }
        if (pct > -0x7f && pct < 0x7f) { queue(false); return; }
    } else {
        if (ebx && C(E->s215d0, ecxv, 9) == 0 && C(E->s21560, ecxv, 4) != 0) { C(E->sd12220, game, tgt); return; }
        if (pct > -0x7f && pct < 0x7f) { if (queue(true) == 0) C(E->sd11b10, game, unit); return; }
    }
    fatal(E, 0x110);
}

// Ground-target skill use: with both cmd+8 and cmd+0x18 set, answer the 0xF95e10 failure record (value 100,
// 0x65 when the target class bit 0x10 of byte +0x104 is set); otherwise resolve a target via 0xD17B0 and move
// to it, or fall back to the skill's range entry.
// 1.13c D2Game+0xA7290
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A7290(uint32_t game, uint32_t unit, uint32_t cmd, const Env* E)
{
    uint32_t rec = R(cmd, 0x1c); int32_t cur = R(cmd, 0x14);
    if (R(cmd, 8) && R(cmd, 0x18)) {
        C(E->o11103, unit, 0);
        uint32_t q[8] = {0, unit, 0, 0, 0, 0, 0, 100};   // ponytail: field order of the stack request unrecovered
        if (unit) { uint32_t t = C(E->s21320); if (t && (R8(t, 0x104) & R(E->bitMasks, 0x10))) q[7] = 0x65; }
        C(E->s95e10, game, A(q));
        return;
    }
    uint32_t loc[2] = {0, 0x19};
    uint32_t t = C(E->sd17b0, A(loc), game, unit, E->cb4180);
    uint32_t cls = R8(game, 0x6d);
    if (t == 0) {
        if (cur < R16(rec, 0x56 + cls * 2)) { C(E->sd12d80, game, unit, R8(rec, 0x5c + cls * 2)); return; }
        C(E->sd122c0, 0x19); return;
    }
    if (C(E->o11138, unit, t, 0)) { C(E->sd12380, unit); return; }
    C(E->sd12540, game, unit, t, 2, 0, 0, 1);
}

static void rangeTail(const Env* E, uint32_t game, uint32_t unit, uint32_t cmd)
{
    int32_t v = R(cmd, 0x14);
    if (C(E->sd1bc0, game, A(&v)) && v < 0x19) {
        C(E->o11103, unit, 0);
        if (unit) C(E->s21320);
        uint32_t q[8] = {0, unit, 0, 0, 0, 0, 0, (uint32_t)v};   // ponytail: stack request layout unrecovered
        C(E->s95e10, game, A(q));
        return;
    }
    C(E->sd122c0, 0x32);
}

// The two bodies are byte-identical in 1.13c (distinct skill ids in the dispatch table).
// 1.13c D2Game+0xA8270
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A8270(uint32_t game, uint32_t unit, uint32_t cmd, const Env* E)
{ rangeTail(E, game, unit, cmd); }
// 1.13c D2Game+0xA8350
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A8350(uint32_t game, uint32_t unit, uint32_t cmd, const Env* E)
{ rangeTail(E, game, unit, cmd); }

// Skill-use dispatcher on the result of 0xC8E80 (target classification); each case either hands off to a
// bound skill (cmd+7 row shorts at 0x170..0x17e), queues a move, or fails with an error code (0xD122C0).
// Always clears state+0x14 on exit except the two paths noted. ECX = game.
// 1.13c D2Game+0xA9610
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_A9610(uint32_t game, uint32_t unit, uint32_t cmd, const Env* E)
{
    uint32_t rowp = R(cmd, 0x1c), st = R(cmd, 0);
    uint32_t node = 0;
    auto findNode = [&]() -> uint32_t {
        if (!(unit && R(unit, 0) == 1 && R(unit, 0x14))) return 0;
        uint32_t ex = R(R(unit, 0x14), 0x28); if (!ex) return 0;
        uint32_t list = R(ex, 0x20); if (!list) return 0;
        uint32_t n = R(list, 0);
        while (R(n, 8) != 10 && n != list) n = R(n, 0);
        return R(n, 8) == 10 ? n : 0;
    };
    node = findNode();
    uint8_t scratch[20] = {};   // 0x66b..: filled but only read by 0xC4610 through its frame
    if (!node) {
        wr32(scratch, 8, 10);
        int32_t t = R(unit, 0);
        wr32(scratch, 12, (t == 2 || (t > 3 && t < 6)) ? R(R(unit, 0x2c), 0xc) : (R(unit, 0x2c) ? C(E->o10867, R(unit, 0x2c)) : 0));
        wr32(scratch, 16, (t == 2 || (t > 3 && t < 6)) ? R(R(unit, 0x2c), 0x10) : (R(unit, 0x2c) ? C(E->o10750, R(unit, 0x2c)) : 0));
        C(E->s41290, unit, A(scratch));
    }
    int32_t out20 = 0; uint8_t buf4[4] = {};
    uint32_t tgt = C(E->sc4610, game, unit, A(&out20), A(buf4), node, E->cb3e10);
    int32_t kind = C(E->sc8e80, unit, tgt, out20);
    if (kind != 5) {
        int32_t s = R16(rowp, 0x17e);
        if (s > -1) { uint32_t r = skillRec(E, s); if (r && R16(r, 0x80) > -1 && C(E->o10494, unit, R16(r, 0x80)) == 0 && C(E->sd12380, unit)) { W(st, 0x14, 0); return; } }
    }
    int32_t err = 2;
    switch (kind) {
    case 1: { uint32_t pb = plr(E, unit, 0x2c); W(pb, 0x1c, 0x14);
        uint32_t y = C(E->s21130, tgt), x = C(E->s21100, tgt); C(E->sd12540, game, unit, 0, 2, x, (uint8_t)y, 1);
        W(st, 0x14, 0); return; }
    case 2: case 3: case 4: C(E->sd12220, game, tgt); W(st, 0x14, 0); return;
    case 5:
        if (R16(rowp, 0x170) < 0) break;
        if (C(E->o10494, unit, 0xc) == 0) { C(E->sd12380, unit); W(st, 0x14, 0); return; }   // retail stores a stack word via EBX
        C(E->o10945, unit, 0xc, 0); err = 2; goto fail;
    case 6:  if (R16(rowp, 0x174) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; } break;
    case 7:  if (R16(rowp, 0x172) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; } break;
    case 8:  if (R16(rowp, 0x176) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; } break;
    case 9:  if (R16(rowp, 0x17c) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; } break;
    case 10: if (R16(rowp, 0x178) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; } break;
    case 13: if (R16(rowp, 0x17a) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; } break;
    case 12: C(E->sd126b0, unit, game, tgt, 4); W(st, 0x14, 0); return;
    case 14: {
        uint32_t pb = plr(E, unit, 0x2c); W(pb, 0x1c, 0x32); *(uint8_t*)E->p(pb + 0x20) = 0x4d;
        if (C(E->sd12540, game, unit, 0, 2, R(node, 0xc), (uint8_t)R(node, 0x10), 1)) goto done;
        walkStates(E, unit);
        int32_t ux = C(E->s21100, unit), uy = C(E->s21130, unit), nx = R(node, 0xc), ny = R(node, 0x10);
        int32_t h = C(E->sd07b0);
        int32_t sx = 0; int8_t sy = 0;
        if (ux < nx) sx = -1; if (nx < ux) sx = 1; if (uy < ny) sy = -1; if (ny < uy) sy = 1;
        pb = plr(E, unit, 0x2c); W(pb, 0x1c, 0x32); *(uint8_t*)E->p(pb + 0x20) = 0x4d;
        if (C(E->sd12540, game, unit, 0, 2, nx + (h >> 1) * sx, (int8_t)ny + (int8_t)(h >> 1) * sy, 1)) goto done;
        walkStates(E, unit);
        err = 2; goto fail; }
    case 15:
        if (tgt && R(tgt, 0) == 0) {
            uint32_t id = C(E->s57440, tgt);
            if (!game) fatal(E, 0x2d3);
            for (uint32_t n = R(game, 0x1520 + (id & 0x7f) * 4); n; n = R(n, 0xe4))
                if (R(n, 0xc) == id) {
                    if (R16(rowp, 0x17c) > -1) { C(E->sd12380, unit); W(st, 0x14, 0); return; }
                    err = 2; goto fail;
                }
            break;
        }
        err = 3; goto fail;
    case 16: W(plr(E, unit, 0x2c), 0x1c, 0x14); C(E->sd11b10, game, unit); W(st, 0x14, 0); return;
    default: err = 0xc; if (R8(game, 0x6d) == 1) err = 8; else if (R8(game, 0x6d) == 2) err = 4; goto fail;
    }
    err = 2;
fail:
    C(E->sd122c0, err);
done:
    W(st, 0x14, 0);
}

// Targeted skill (unit/monster) pre-check and queue: resolves the target in the game unit hash, checks the
// distance (#10522) against the per-class limits in the skill row (+0x56/+0x5c/+0x62/+0x68/+0x6e), then
// queues the move (0xD12540 mode 0xF) or fails with 0xF. EAX = cmd, ECX = unit, stack = game; EBP is the
// 0xD1C90 helper's extra argument.
// 1.13c D2Game+0xAAC30
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_AAC30(uint32_t unit, uint32_t game, uint32_t cmd, uint32_t ebp, const Env* E)
{
    uint32_t row = R(cmd, 0x1c), st = R(cmd, 0);
    uint32_t cls = game ? R8(game, 0x6d) : R8(0, 0x6d);
    uint32_t tg = 0, ex;
    if (unit && R(unit, 0) == 1 && R(unit, 0x14) && (ex = R(R(unit, 0x14), 0x28)) && R(ex, 0x28))
        tg = C(E->sd003a0, R(ex, 0x28), R(ex, 0x2c));
    if (!tg) {
        uint32_t id = R(st, 0x1c);
        if (!id) { C(E->sd122c0, 10); return; }
        if (!game) fatal(E, 0x2d3);
        for (uint32_t n = R(game, 0x1120 + (id & 0x7f) * 4); n; n = R(n, 0xe4))
            if (R(n, 0xc) == id) {
                if (R16(row, 0x170) > -1) {
                    C(E->s21130, n); C(E->s21100, n); C(E->sd12380, unit);
                    C(E->saea80, game, unit, 0, R(game, 0xa8) + 6, 0, 0, 0); C(E->sd120c0, 8); return;
                }
                break;
            }
        C(E->sd122c0, 10); return;
    }
    W(st, 0x1c, R(tg, 0xc));
    if (C(E->o10057, C(E->o10331, unit)) != 0) {
        if (C(E->s3e140, game, unit, 0, 0)) return;
        C(E->sd122c0, 0x21); return;
    }
    int32_t lim = R16(row, 0x68 + cls * 2);
    int32_t out[2] = {(int32_t)row, 0};
    int32_t pStack4 = 0;
    uint32_t p5 = C(E->sd1c90, unit, ebp, A(&pStack4));
    uint32_t cand = p5;
    if (!p5 || lim < pStack4) cand = 0;
    uint32_t q5 = C(E->sd1c90, tg, ebp, A(out));
    if (cand == 0) {
        if (q5 && C(E->sd0a20, q5) < lim && C(E->sd0e90, unit) == 0) cand = q5;
    } else if (C(E->sd0e90, unit) == 0) {
        cand = 0;
        if (q5 && C(E->sd0a20, q5) < lim && C(E->sd0e90, unit) == 0) cand = q5;
    }
    int32_t dist = C(E->o10522, tg, unit);
    if (R16(row, 0x170) > -1 && dist > 0x32) {
        C(E->s21130, tg); C(E->s21100, tg); C(E->sd12380, unit);
        C(E->saea80, game, unit, 0, R(game, 0xa8) + 4, 0, 0, 0); C(E->sd120c0, 10); return;
    }
    uint32_t cls2 = game ? cls : 0;
    int32_t kind = R(tg, 0x10);
    bool near = dist <= R16(row, 0x6e + cls2 * 2) || C(E->s3cae0, game, tg, 0) == 0;
    bool far  = near && (dist <= R16(row, 0x62 + cls2 * 2) ||
                ((((kind != 2 && kind != 6) || C(E->s3cae0, game, tg, 0) == 0)) && (kind != 3 || C(E->s3cae0, game, tg, 0) == 0)));
    if (!(near && far)) return;
    if (C(E->s3e140, game, unit, game, cand) != 0) return;
    if (game == 0) {
        if ((int32_t)C(E->s21180, A((uint8_t*)E->p(unit) + 0x20)) < R16(out[0], 0x5c + R8(0, 0x6d) * 2)) { C(E->sd11b10, 0, unit); return; }
    } else {
        if (cand) { C(E->sd12220, game, game); C(E->sd120c0, R16(out[0], 0x56 + R8(game, 0x6d) * 2)); return; }
        if (C(E->o10522, tg) < out[1]) {
            if (lim > -0x7f && lim < 0x7f) {
                uint32_t pb = (R(unit, 0) == 1 && R(unit, 0x14)) ? R(R(unit, 0x14), 0x2c) : 0;
                if (lim) W(pb, 0x1c, lim);
                C(E->sd12540, game, unit, game, 0xf, 0, 0, 1); return;
            }
            fatal(E, 0x110);
        }
        if (C(E->o10522, tg) > 10) { C(E->sd11d90, tg, game, unit, 2, 6); return; }
    }
    C(E->sd122c0, 0xf);
}

// Walk the unit's inventory-like list (+0x60) with #10460/#10464; true when an entry's #11147 equals EDI.
// EAX = holder, EDI = key.
// 1.13c D2Game+0xAC9E0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AC9E0(uint32_t holder, uint32_t key, const Env* E)
{
    if (R(holder, 0x60) == 0) return 0;
    for (int32_t it = C(E->o10460, R(holder, 0x60)); it; it = C(E->o10464, it))
        if ((uint32_t)C(E->o11147, it) == key) return 1;
    return 0;
}

// Heal-by-percent: for a player unit (type 0) and an item record with +0x4c > 0, #10910 must be positive;
// adds item%*max... through #10973/#10887 (stat 8, capped at #10186) and finishes with #10689(unit, 0x98).
// param_4 is a packed word whose high half is the 0x4c-record handle.
// 1.13c D2Game+0xACDA0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_ACDA0(uint32_t unit, uint32_t unused, uint32_t item, uint32_t packed, const Env* E)
{
    (void)unused;
    if (!unit || R(unit, 0) != 0 || !item || (int32_t)R(item, 0x4c) < 1) return 0;
    uint32_t hi = packed >> 16;
    if (C(E->o10910, unit, hi, packed) < 1) return 0;
    int32_t mx = C(E->o10186, unit);
    int32_t cur = C(E->o10973, unit, 8, 0);
    if (mx <= cur) return 0;
    int32_t add = C(E->s214d0, 100, R(hi, 0x4c));
    uint32_t v = (uint32_t)(add + cur) & ((add + cur < 1) - 1);
    if (mx <= (int32_t)v) v = mx;
    C(E->o10887, unit, 8, v, 0);
    C(E->o10689, unit, 0x98, 0);
    return 1;
}

// For each entry of the +0x60 list whose unit carries stat-state 4 (#10202(unit,4,0x78)): clear it
// (#10977), then queue the 8-byte 0x3F update packet (0xAA3E0) with the unit id. param_1 is the player.
// 1.13c D2Game+0xACF10
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ACF10(uint32_t player, const Env* E)
{
    if (R(player, 0x60) == 0) return;
    for (int32_t it = C(E->o10460, R(player, 0x60)); it; it = C(E->o10464, it)) {
        uint32_t u = C(E->o11147, it);
        if (C(E->o10202, u, 4, 0x78) == 0) continue;
        C(E->o10977, u, 4, 0);
        if (R(player, 0) == 0) C(E->o10920, player);
        uint8_t pkt[8] = {0x3f, 0xff, 0, 0, 0, 0, 0xff, 0xff};
        if (u) memcpy(pkt + 2, (const uint8_t*)E->p(u) + 0xc, 4);
        C(E->saa3e0, A(pkt), 8);
    }
}

// Clear stat-state 4 on item EDI-owner param_3, and when #11000 answers a slot >= 0 queue the 0x3F packet
// (slot byte, item id). Always answers 1. param_2 is the player.
// 1.13c D2Game+0xACFD0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_ACFD0(uint32_t ecx, uint32_t player, uint32_t item, const Env* E)
{
    (void)ecx;
    C(E->o10977, item, 4);
    int32_t slot = C(E->o11000, item);
    if (slot > -1) {
        if (player && R(player, 0) == 0) C(E->o10920, player);
        uint8_t pkt[8] = {0x3f, (uint8_t)slot, 0, 0, 0, 0, 0, 0};
        if (item) memcpy(pkt + 2, (const uint8_t*)E->p(item) + 0xc, 4);
        memcpy(pkt + 6, &item, 2);
        C(E->saa3e0, A(pkt), 8);
    }
    return 1;
}

// Item-use dispatcher: EDI = player unit, ESI = use packet (type 4, +4 item id, +0xc, +0). Resolves the
// item's handler index (#10744 / #11028+#11065 / #10695+0x94), then runs the pre handler
// (table[idx*8], unless #10202 says it is skipped) or the post handler (table[idx*8+4]) and the 0xACF10
// refresh plus 0xAC3B0 acknowledge. Answers 0 or the post handler's result.
// 1.13c D2Game+0xAD0F0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_AD0F0(uint32_t player, uint32_t pkt, const Env* E)
{
    if (!player || (R(player, 0) != 0 && R(player, 0) != 1) || !pkt || R(pkt, 0) != 4) return 0;
    int32_t idx = -1;
    if (C(E->o10744) != 0 || C(E->o10744) != 0) {
        uint16_t t = (uint16_t)C(E->o11028);
        uint32_t r = C(E->o11065, t);
        if (r && (idx = (int32_t)R(r, 4)) > 0) goto have;
    }
    {
        uint32_t r = C(E->o10695, R(pkt, 4));
        if (r) idx = R(r, 0x94);
        if (idx < 1) return 0;
    }
have:
    if (idx >= (int32_t)E->handlerCount) return 0;
    {
        uint32_t pre = R(E->handlerTable, idx * 8), post = R(E->handlerTable, idx * 8 + 4);
        if (pre && C(E->o10202) == 0) {
            if (C((A6B70Seam)(intptr_t)pre) != 0) return 0;
            C(E->sccf10, player);
            if (R(player, 0) == 0) C(E->o10920);
            C(E->sac3b0, R(pkt, 0), R(pkt, 0xc));
            return 0;
        }
        if (post) {
            C(E->o10977, 0, 4, 0);
            int32_t r = C((A6B70Seam)(intptr_t)post);
            if (r == 0) {
                C(E->sccf10, player);
                if (R(player, 0) == 0) C(E->o10920);
                C(E->sac3b0, R(pkt, 0), R(pkt, 0xc));
            }
            return r;
        }
    }
    return 0;
}
