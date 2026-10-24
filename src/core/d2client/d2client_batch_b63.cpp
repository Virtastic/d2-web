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
// 1.13c D2Client+0x512E0  (esi = other-unit/point block; param_1 = point ptr value)
// Ordinal_10141 fills a point for DAT_6fbcbbfc; the decompile's else-branch compares stack addresses
// (decompiler artefact), so both branches compare coordinates here.
EMSCRIPTEN_KEEPALIVE i32 d2c_512E0(const i32* esi, i32 param_1, i32 player, i32 (*getpt)(i32 unit, i32* out),
    i32 (*ord10296)(), i32 (*flagbyte)(i32 base)) {
    if (esi && esi[4]) return 0;
    i32 pt[2] = {0, 0};
    if (player) getpt(player, pt);
    i32 base = ord10296();
    if (flagbyte(base + 0x13c) == 1) {
        if (pt[0] < param_1 && param_1 < (i32)(intptr_t)esi) return 0;
        if (param_1 < pt[0] && (i32)(intptr_t)esi < param_1) return 0;
    }
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
    i32 (*ord10331)(i32 p, i32 q); void (*ord10742)(i32); void (*ord10880)(i32); i32 (*ord10331b)();
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
            if (a && e->f1260()) {
                i32 b = e->f1260();
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
        e->ord10880(e->ord10331b()); *g->flags |= 1;
    }
}

struct D2cE51AE0 {
    i32 (*ord10858)(); i32 (*f55440)(); i32* (*f2c5c0)(); i32 (*f55b40)(i32 id, i32 type);
    i32 (*ord10331)(i32 p, i32 q); void (*ord10742)(i32); i32 (*f1630)(i32*); i32 (*f1660)(i32*);
    i32 (*pathx)(i32 path); i32 (*pathy)(i32 path); void (*f50b30)(i32*);
    i32 (*ord10143)(i32*); i32 (*ord10787)(i32*, i32*);
    i32* (*f1650)(i32*, i32*, const i32*); i32* (*f1500)(); i32* (*f1390)(const i32*);
};
struct D2cG51AE0 { i32 g2f8, g3a68, g2f4, g964c, g9638, g985c, g97f8; uint8_t* g3afa; i32* flags; i32* player; };
static i32 unit_pos(const D2cE51AE0* e, const i32* u, int off) {
    i32 t = u[0];
    if (t == 2 || (t > 3 && t < 6)) return rd((const void*)(intptr_t)u[0xb], off);
    return u[0xb] == 0 ? 0 : (off == 0xc ? e->pathx(u[0xb]) : e->pathy(u[0xb]));
}
// 1.13c D2Client+0x51AE0  (eax = in_EAX target unit; retaddr = unaff_retaddr; g->player = DAT_6fbcbbfc)
EMSCRIPTEN_KEEPALIVE i32* d2c_51AE0(const D2cE51AE0* e, D2cG51AE0* g, const i32* bitmask, i32* in_EAX, i32* param_1,
    i32* param_2, i32* param_3, uint8_t param_4, const i32* retaddr) {
    const uint8_t* st = (const uint8_t*)(intptr_t)e->ord10858();
    i32* piVar6 = g->player;
    bool skip = false;
    if (in_EAX) {
        if (in_EAX[0] == 0 && ((in_EAX[0x31] & 0x10000) || in_EAX[4] == 0 || in_EAX[4] == 0x11)) return in_EAX;
        if (((uint8_t)bitmask[0] & st[7]) || !e->f55440()) skip = true;
        else {
            if (g->g2f8 == 0) {
                if (!g->g3a68 || e->f2c5c0()) {
                    *g->g3afa = 0;
                    if (g->g2f4 && g->g964c == 5) {
                        i32 p = e->f55b40(g->g9638, 5);
                        if (p) { e->ord10742(e->ord10331(p, rd((void*)(intptr_t)p, 4))); *g->flags |= 1; }
                    }
                    g->g2f4 = 0;
                } else *g->g3afa = 0;
            }
            *param_2 = e->f1630(in_EAX);
            *param_3 = e->f1660(in_EAX);
            in_EAX = nullptr;
        }
    }
    if (!skip && *param_2 == unit_pos(e, piVar6, 0xc) && *param_3 == unit_pos(e, piVar6, 0x10)) e->f50b30(param_2);
    if ((!((param_4 & 8) && (param_4 & 1) && !(param_4 & 0x20)) || !e->ord10143(piVar6)) &&
        !e->ord10787(piVar6, param_1) && (piVar6 != g->player || g->g985c <= g->g97f8)) {
        u32 b6 = st[6];
        if ((u32)bitmask[6] & b6) return e->f1650(param_1, in_EAX, retaddr);
        if (!in_EAX && ((u32)bitmask[5] & b6) && param_3) return e->f1500();
        if ((u32)bitmask[7] & b6) return e->f1390(retaddr);
    }
    return in_EAX;
}

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

// 1.13c D2Client+0x5D7A0  (param_1 = effect request block)
// Builds a light/effect object for a visual slot; all Ordinal_100xx/FUN seams are injected.
struct D2cE5D7A0 {
    i32 (*f0ee70)(i32 id); i32* (*f0cc10)(i32 id); void (*f0ebc0)(i32 id, i32); u32* (*f0ce70)(u32* rq, u32 flags);
    i32 (*ord10059)(); void (*ord10005)(); void (*ord10038)(); void (*ord10050)(); i32 (*ord10069)(i32);
    void (*ord10026)(u32); void (*f0d1d0)(u32* obj); void (*f0db30)(i32 id, char* buf); i32 (*ord10037)();
    i32 (*ord10039)(i32, uint8_t);
};
EMSCRIPTEN_KEEPALIVE u32* d2c_5D7A0(const D2cE5D7A0* e, uint8_t* tbl, i32 tblCnt, u32* rq) {
    i32 id = (i32)rq[1];
    uint8_t* rec = (tbl && id >= 0 && id < tblCnt) ? tbl + id * 0x8e : nullptr;
    if (!(rq[0xe] & 1)) { id = e->f0ee70(id); rq[1] = (u32)id; }
    i32* p = e->f0cc10((i32)rq[1]);
    if (p) {
        uint8_t* q = (uint8_t*)(intptr_t)p[2];
        if (rd(q, 0x3d) == 0 && (u32)(rq[0xd] - rd(q, 0x34)) <= 1 && (uint8_t)q[0x3c] >= (uint8_t)rq[0xf]) return nullptr;
    }
    i32 id2 = (i32)rq[1];
    uint8_t* r2 = (tbl && id2 >= 0 && id2 < tblCnt) ? tbl + id2 * 0x8e : nullptr;
    wr(rec, 0x68, rd(rec, 100)); wr(rec, 100, id2);
    if (!(r2[0x4f] || rd(r2, 0x86) == 2)) {
        e->f0ebc0(id2, 0);
        if (rd(r2, 0x86) != 2) return nullptr;
    }
    if (r2[0x81]) return nullptr;
    u32 f = r2[0x50] != 0;
    if (!f) { if (e->ord10059() == 1) f = 2; if (e->ord10059() == 2) f |= 6; }
    u32* o = e->f0ce70(rq, f);
    if (!o) return nullptr;
    e->ord10005();
    if (!r2[0x4f]) {
        e->ord10038();
        if (r2[0x80] == 1) e->ord10050();
        if (!e->ord10069(rd(r2, 0x70))) return nullptr;
    }
    o[2] = (u32)(uintptr_t)rq;
    if (f & 4) e->ord10026(r2[0x4b] ? 0x3f800000u : 0u);
    o[6] = 0xffffffffu; o[7] = 0; o[3] = o[4] = o[5] = 0;
    e->f0d1d0(o);
    if (!r2[0x4f]) { if (!e->ord10037()) { o[2] = 0; return nullptr; } }
    else {
        char buf[80]; e->f0db30(id, buf);
        if (!e->ord10039(rd(rq, 0x24), r2[0x3e])) { o[2] = 0; return nullptr; }
    }
    return o;
}

// 1.13c D2Client+0x63620 and 1.13c D2Client+0x636A0 (identical bodies: CRT stack-probe wrapper)
EMSCRIPTEN_KEEPALIVE uint64_t d2c_63620(i32 g3710, void (*f9ff7)(), uint64_t (*facc4)()) { if (g3710) f9ff7(); return facc4(); }
EMSCRIPTEN_KEEPALIVE uint64_t d2c_636A0(i32 g3710, void (*f9ff7)(), uint64_t (*facc4)()) { if (g3710) f9ff7(); return facc4(); }
}

extern "C" {
// 1.13c D2Client+0x51230  (esi = other-unit/point block; param_1 = second point's x, from the 2nd Ordinal_10141)
// Twin of 0x512E0: same gate and range test; the else-branch compares stack addresses (decompiler artefact).
EMSCRIPTEN_KEEPALIVE i32 d2c_51230(const i32* esi, i32 param_1, i32 player, i32 (*getpt)(i32 unit, i32* out),
    i32 (*ord10296)(), i32 (*flagbyte)(i32 base)) {
    if (esi && esi[4]) return 0;
    i32 pt[2] = {0, 0};
    if (player) getpt(player, pt);
    i32 base = ord10296();
    if (flagbyte(base + 0x13c) == 1) {
        if (pt[0] < param_1 && param_1 < (i32)(intptr_t)esi) return 0;
        if (param_1 < pt[0] && (i32)(intptr_t)esi < param_1) return 0;
    }
    return 1;
}
}
