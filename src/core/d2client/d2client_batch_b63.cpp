// D2Client batch b63-cli-r: env-seam ports (callees/globals injected, no retail data).
// Hidden register inputs are explicit leading parameters, noted per function.
// Skipped: 0x5D1D0 (x87 float10), 0x66DA0/0x66EB0 (SEH ctor chain).
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;
static inline i32 rd(const void* p, u32 off) { i32 v; memcpy(&v, (const char*)p + off, 4); return v; }
static inline void wr(void* p, u32 off, i32 v) { memcpy((char*)p + off, &v, 4); }
// Unit-position accessor shared by 51650/51AE0: types 2,4,5 keep x/y at +0xC/+0x10 of the path ptr (+0x2C).
static i32 unit_xy(const i32* u, int off, i32 (*ord)(const i32*), i32 (*pathrd)(i32 path, int off)) {
    i32 t = u[0];
    if (t == 2 || (t > 3 && t < 6)) return pathrd(u[0xb], off);
    return u[0xb] == 0 ? 0 : ord(u);
}

extern "C" {
// 1.13c D2Client+0x512E0  (esi = unit under test, param_1 = other unit; disassembly: three Ordinal_10141 points
// A=player(DAT_6fbcbbfc, (0,0) if null) B=param_1 C=esi; flag byte +0x13c==1 picks y else x; unsigned compares)
// Returns 0 when esi[4]!=0 or when C lies strictly between A and B on the chosen axis, else 1.
EMSCRIPTEN_KEEPALIVE i32 d2c_512E0(const i32* esi, i32 param_1, i32 player, i32 (*getpt)(i32 unit, i32* out),
    i32 (*ord10296)(), i32 (*flagbyte)(i32 base)) {
    if (esi && esi[4]) return 0;
    uint32_t A[2] = {0, 0}, B[2], C[2];
    if (player) getpt(player, (i32*)A);
    getpt(param_1, (i32*)B);
    getpt((i32)(intptr_t)esi, (i32*)C);
    i32 base = ord10296();
    int k = flagbyte(base + 0x13c) == 1 ? 1 : 0;
    uint32_t a = A[k], b = B[k], c = C[k];
    if (c > b && c < a) return 0;
    if (c < b && c > a) return 0;
    return 1;
}

struct D2cE51650 {
    i32 (*pathx)(i32 unit); i32 (*pathy)(i32 unit);          // Ordinal_10867 / 10750 fallbacks
    i32 (*ord10601)(); i32 (*ord11064)(i32 unit);
    void (*ord10591)(i32 zero, const i32* req, i32 ctx); void (*ord10052)(i32* req);
    i32 (*ord10080)(i32 u, i32 x, i32 y); i32 (*f568e0)(i32 u, i32 a, i32 b, i32 c);
    i32 (*ord10331)(i32 unit); void (*ord10344)(const i32* req);
};
// 1.13c D2Client+0x51650  (eax = in_EAX attacker-state ptr, stack retaddr slot = unaff_retaddr)
// Picks the nearest valid target unit from a room scan; req layout [0x00]=flags [0x0C]=z [0x20]=player.
EMSCRIPTEN_KEEPALIVE i32* d2c_51650(const D2cE51650* e, const uint8_t* in_EAX, const i32* bitmask, i32 player,
    i32* param_1, i32* param_2, i32* param_3, i32* param_4, const i32* retaddr, i32 cb11d0) {
    if (param_2) {
        if ((*bitmask & in_EAX[7]) && !(param_2[0x31] & 0x10000)) {
            if (param_2[0] == 0 && (param_2[4] == 0 || param_2[4] == 0x11)) return param_2;
            if (param_2[0] == 1 && (param_2[4] == 0 || param_2[4] == 0xc)) return param_2;
        } else if ((*bitmask & in_EAX[7])) return param_2;
        i32 t = param_2[0];
        *param_3 = (t == 2 || (t > 3 && t < 6)) ? rd((const void*)(intptr_t)param_2[0xb], 0xc) : (param_2[0xb] ? e->pathx(param_2[0xb]) : 0);
        *param_4 = (t == 2 || (t > 3 && t < 6)) ? rd((const void*)(intptr_t)param_2[0xb], 0x10) : (param_2[0xb] ? e->pathy(param_2[0xb]) : 0);
    }
    i32 cnt = ((in_EAX[7] & *bitmask & 0xff) ? 0xfff : 0) + 0x2003;
    i32 cb = 0;
    if (e->ord10601() == 0x5a) {
        if (param_2 && param_2[0] == 4 && e->ord11064(0)) return param_2;
        cnt = 0x20; cb = cb11d0;
    }
    // frame is contiguous in the original: f[0]=*retaddr, f[1..4]={*param_1, 9, callback, &req}
    i32 req[14] = {0};
    req[0] = cnt; req[2] = player; req[3] = *retaddr; req[4] = *param_1; req[5] = 9;
    i32 f[5] = {*retaddr, *param_1, 9, cb, (i32)(intptr_t)req};
    e->ord10591(0, f + 1, player ? e->ord10331(player) : 0);
    e->ord10052(f + 1);
    i32 best = 10;
    for (i32 i = 0; i < cnt; i++) {
        i32* u = (i32*)(intptr_t)((const i32*)(intptr_t)f[1])[i];
        i32 d = e->ord10080((i32)(intptr_t)u, *param_3, *param_4);
        if (e->f568e0((i32)(intptr_t)u, 0, 0, 0) && d < best) { param_2 = u; best = d; }
    }
    e->ord10344(f);
    return param_2;
}

struct D2cE51860 {
    i32* (*f2c5c0)(); i32 (*f568e0)(i32* u, i32 a, i32 b, i32 c); i32 (*f55b40)(i32 id, i32 type);
    i32 (*ord10331)(i32 p, i32 q); void (*ord10742)(i32); void (*ord10880)(i32);
    i32 (*f1230)(); i32 (*f1260)(); i32 (*ord11020)(); u32 (*tick)(); void (*f4940)(); void (*fatal)(i32 code);
};
struct D2cG51860 { i32 busy, g3a68, *cur, lastType, lastId, hover[2]; uint8_t* g3afa; i32* g3afb; i32* flags; };
// 1.13c D2Client+0x51860  (eax = in_EAX hover target unit or null)
EMSCRIPTEN_KEEPALIVE void d2c_51860(const D2cE51860* e, D2cG51860* g, i32* in_EAX, const i32* bitmask) {
    if (g->busy) return;
    if (g->g3a68 && e->f2c5c0() == in_EAX) {
        if (in_EAX) { *g->g3afb = in_EAX[3]; *g->g3afa = 1; } else *g->g3afa = 0;
        return;
    }
    bool same = false;
    *g->g3afa = 0;
    if (in_EAX) {
        if (!e->f568e0(in_EAX, 0, 0, 0)) { e->fatal(0x4f); return; }
        if (!g->cur) goto tail;
        if (*in_EAX == g->lastType && in_EAX[3] == g->lastId) { same = true; goto tail; }
    }
    if (g->cur && g->lastType == 5) {
        i32 p = e->f55b40(g->lastId, 5);
        if (p) { e->ord10742(e->ord10331(p, rd((void*)(intptr_t)p, 4))); *g->flags |= 1; }
    }
tail:
    if (!in_EAX) { g->cur = nullptr; return; }
    g->lastType = *in_EAX; g->lastId = in_EAX[3]; g->cur = (i32*)1;
    if (g->lastType == 1) {
        if (!same) {
            i32 id = in_EAX[1], a = e->f1230();
            i32 b = a ? e->f1260() : 0;  // retail calls 0x1260 once, only when 0x1230 returned nonzero
            if (a && b) {
                if (!(bitmask[0] & rd((void*)(intptr_t)a, 0xd) & 0xff) && (rd((void*)(intptr_t)a, 0x4c) & 0xff) != 1 &&
                    (rd((void*)(intptr_t)b, 5) & bitmask[1] & 0xff)) {
                    g->hover[0] = id;
                    if (!(rd((void*)(intptr_t)b, 6) & bitmask[1] & 0xff)) g->hover[1] = id;
                }
            }
        }
    } else if (g->lastType == 2) {
        if (e->ord11020() && in_EAX[4] != 4) {
            u32 t = e->tick();
            if (t - (u32)in_EAX[0x35] > 500) { in_EAX[0x35] = (i32)t; e->f4940(); }
        }
    } else if (g->lastType == 5 && !same) {
        e->ord10880(e->ord10331((i32)(intptr_t)in_EAX, in_EAX[1])); *g->flags |= 1;
    }
}

// The click-target resolver (D2Client 51AE0) lives in d2client_51ae0.cpp (faithful port with injectable seams).

// 1.13c D2Client+0x51E80  (no register inputs; every global and callee is an injected seam)
// Hover resolver: direct pick via 0x51A80/0xA68E0, else walk the rooms around the head unit and keep the
// best-priority selectable unit (priority table DAT_6fba2190, indexed by unit type), then commit via 0x51860.
struct D2cE51E80 {
    const i32* prio;                 // DAT_6fba2190
    i32 g2f8, g2f0; i32* g347c; i32* g3480; i32* head;   // DAT_6fbcc2f8/2f0, 6fbd347c, 6fbd3480, 6fbcbbfc
    i32 (*f51d10)(i32* px, i32* py);                     // ECX=px out, EAX=py out; nonzero = no pick
    i32* (*f51a80)();
    i32 (*fa68e0)(i32* u, i32 px, i32 py, i32 hit);
    void (*f51860)(i32* best);                           // EAX=best
    i32 (*ord10331)(i32* head);                          // head -> room
    i32 (*f1630)(i32* head); i32 (*f1660)(i32* head);
    void (*ord11087)(i32* x, i32* y);
    void (*ord10383)(i32 room, i32*** rooms, u32* count);
    i32 (*f51470)(i32* u, i32 x, i32* flag, i32* head, i32 y, i32 py, i32 px);
    i32 (*f55440)(i32* u);
    i32 (*ord10648)(i32* u); i32 (*ord10494)(i32* u, i32 v); i32 (*ord11020)(i32* u);
    i32 (*f51230)(i32* esi_u, i32* best); i32 (*f512e0)(i32* esi_best, i32* u);
    i32 (*f510b0)(i32* head, i32* u);
    void (*fatal)();                                     // >30000 chain: D2Win error print + _exit(-1)
};
EMSCRIPTEN_KEEPALIVE void d2c_51E80(const D2cE51E80* e) {
    if (e->g2f8) return;
    i32 px = 0, py = 0;
    if (e->f51d10(&px, &py)) return;
    i32* p = e->f51a80();
    if (p && e->fa68e0(p, px, py, 1) && (*p != 4 || !e->g2f0)) { e->f51860(p); return; }
    i32* head = e->head;
    if (!head) return;
    i32 room = e->ord10331(head);
    if (!room) return;
    if (*e->g347c != room) *e->g3480 = 1;
    *e->g347c = room;
    i32 x = e->f1630(head), y = e->f1660(head);
    e->ord11087(&x, &y);
    i32** rooms = nullptr; u32 cnt = 0;
    e->ord10383(room, &rooms, &cnt);
    i32* best = nullptr;
    for (u32 r = 0; r < cnt; r++) {
        i32* last = nullptr; i32 n = 0;
        for (i32* u = (i32*)(intptr_t)rooms[r][0x74 / 4]; u; u = (i32*)(intptr_t)u[0x3a]) {
            i32 tu = *u;
            if (tu == 3) continue;
            if (last == u) break;
            last = u;
            if (n++ > 30000) { e->fatal(); return; }
            if (u == head) continue;
            if (e->f51470(u, x, e->g3480, head, y, py, px)) continue;
            if (tu == 4 && e->g2f0) continue;
            if (!e->fa68e0(u, px, py, 1)) continue;
            if (p == u && !e->f55440(u)) { best = u; break; }
            if (best) {
                i32 pu = e->prio[tu], pb = e->prio[*best];
                if (tu == 0) { if (e->f55440(u)) pu = 1; }
                else if (tu == 1) { if (e->ord10648(u)) pu = -8; if (e->ord10494(u, 0x8f)) continue; }
                if (*best == 1 && e->ord10648(best)) pb = -8;
                bool take;
                if (tu == 2 && *best != 2) take = !e->ord11020(u) ? pb < pu : e->f51230(u, best) != 0;
                else if (tu != 2 && *best == 2) take = !e->ord11020(best) ? pb < pu : e->f512e0(best, u) != 0;
                else if (pb != pu) take = pb < pu;
                else take = e->f510b0(head, u) < e->f510b0(head, best);
                if (!take) continue;
            }
            best = u;
        }
    }
    e->f51860(best);
}

// 1.13c D2Client+0x5D7A0: moved to d2client_5d7a0.cpp (stdcall rq; the 0x5DB30 id is rq+4 after the remap, the D2Sound calls take o[0]).

// 1.13c D2Client+0x63620 and 1.13c D2Client+0x636A0 (identical bodies: CRT stack-probe wrapper)
EMSCRIPTEN_KEEPALIVE uint64_t d2c_63620(i32 g3710, void (*f9ff7)(), uint64_t (*facc4)()) { if (g3710) f9ff7(); return facc4(); }
EMSCRIPTEN_KEEPALIVE uint64_t d2c_636A0(i32 g3710, void (*f9ff7)(), uint64_t (*facc4)()) { if (g3710) f9ff7(); return facc4(); }
}

extern "C" {
// 1.13c D2Client+0x51230  (esi = unit P under test, param_1 = other unit Q; disassembly: A=player (0,0 if null), Q, P points;
// 10296(esi) flag byte +0x13c==1 picks y else x; unsigned). Returns 0 when esi[4]!=0 or P is not strictly between Q and A, else 1.
EMSCRIPTEN_KEEPALIVE i32 d2c_51230(const i32* esi, i32 param_1, i32 player, i32 (*getpt)(i32 unit, i32* out),
    i32 (*ord10296)(), i32 (*flagbyte)(i32 base)) {
    if (esi && esi[4]) return 0;
    i32 base = ord10296();
    uint32_t A[2] = {0, 0}, P[2], Q[2];
    if (player) getpt(player, (i32*)A);
    getpt((i32)(intptr_t)esi, (i32*)P);
    getpt(param_1, (i32*)Q);
    int k = flagbyte(base + 0x13c) == 1 ? 1 : 0;
    uint32_t a = A[k], p = P[k], q = Q[k];
    if (q < p && p < a) return 1;
    if (q > p && p > a) return 1;
    return 0;
}
}
