// src/core/d2game/d2game_batch_b64.cpp -- twelve UNMAPPED 1.13c D2Game.dll rows reimplemented from the
// cached decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order:
//   0xBD040 0xBE2A0 0xC39B0 0xC8FD0 0xCB1F0 0xCB690 0xCBA20 0xCBF50 0xCE040 0xCE0D0 0xCE120 0xCE150
// Base 0x6FC20000. Batch lane: implementation + marker only, no spec, no vector.
//
// Register-only inputs (EAX/EBX/EBP/ESI/EDI, caller-frame words) are explicit leading/extra params, commented
// per function. Callees not implemented here are seams on B64Env, in retail argument order; Ordinal_NNNNN
// are 1.13c D2Common/Fog import numbers. Addresses are 32-bit retail pointers.
#include <cstdint>
#include <cstring>
#include <initializer_list>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 2); return v; }
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static inline void wr16(uint32_t a, uint32_t o, uint16_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 2); }
static inline void wr8 (uint32_t a, uint32_t o, uint8_t v)  { *(uint8_t*)(uintptr_t)(a + o) = v; }

struct B64Env {
    uint32_t (*o10695)(uint32_t id);                       // monster/item record by id
    uint32_t (*o10450)(uint32_t code, uint32_t* out);      // code lookup
    uint32_t (*rand211D0)(uint32_t seedPtr);               // 0x211D0 seed roll
    uint32_t (*o10041)();                                  // table getter (BE2A0)
    uint32_t (*o10237)(uint32_t v);
    uint32_t (*o10045)(const char* file, uint32_t line);   // alloc
    uint32_t (*o10387)(uint32_t a, uint32_t b);
    uint32_t (*o10973)(uint32_t game, uint32_t a, uint32_t b);
    uint32_t (*spawn31980)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h, uint32_t i, uint32_t j);
    void     (*o10012)(uint32_t unit, uint32_t v);
    uint32_t (*o10087)(uint32_t unit);
    uint32_t (*o10711)(uint32_t unit);
    uint32_t (*o10048)(uint32_t unit);
    uint32_t (*o10463)(uint32_t unit);
    void     (*o10887)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c);
    void     (*f2380)(uint32_t a, uint32_t unit);
    uint32_t (*o10202)(uint32_t unit, uint32_t a, uint32_t b, const char* file);
    uint32_t (*o11116)(uint32_t unit);
    void     (*f5690)(uint32_t p);
    void     (*o10977)(uint32_t unit, uint32_t a, uint32_t b);
    uint32_t (*o10750)(uint32_t path);                     // path Y
    uint32_t (*o10867)(uint32_t path);                     // path X
    uint32_t (*f76a0)(uint32_t p, uint32_t room, uint32_t id);
    void     (*f2ee70)();
    uint32_t (*fE660)(uint32_t code);
    uint32_t (*fC1B60)();
    uint32_t (*f1280)();
    void     (*f54130)(uint32_t room, uint32_t pos, uint32_t a, uint32_t b);
    uint32_t (*fEF10)(void* q);
    uint32_t (*fF650)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g, uint32_t h);
    void     (*f68410)(uint32_t game, uint32_t unit, uint32_t z);
    void     (*f49820)();
    void     (*fE20A0)(uint32_t game, uint32_t unit);
    void     (*o10024)(const char* s, uint32_t a);
    uint32_t (*o10265)(uint32_t n);
    uint32_t (*o10601)(uint32_t p, const char* file, uint32_t line);
    uint32_t (*o11139)(uint32_t inv, uint32_t slot);
    uint32_t (*o11006)(uint32_t item);
    uint32_t (*o11088)(uint32_t item);
    uint32_t (*o10744)(uint32_t item, uint32_t stat);
    uint32_t (*o10631)(uint32_t inv);
    uint32_t (*o10708)(uint32_t v);
    void     (*o11031)(uint32_t inv, uint32_t slot);
    uint16_t (*o10764)(uint32_t path);
    uint16_t (*o10983)(uint32_t path);
    void     (*fAA3E0)(uint32_t ecx, const uint8_t* pkt, uint32_t len);
    uint32_t (*o10920)(uint32_t unit);
    uint32_t (*f1100)(uint32_t unit);
    uint32_t (*f1130)(uint32_t unit);
    void     (*fBF80)(uint32_t ecx, uint32_t a, uint32_t b, int16_t c, uint16_t d, uint8_t e);
    void     (*f98550)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g);
    void     (*f98430)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
    void     (*o10329)();
    uint32_t (*fC230)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint8_t e, uint32_t f, int16_t g, uint16_t h);
    uint32_t (*o10392)(uint32_t v);
    uint32_t (*o10331)(uint32_t unit);
    uint32_t (*f003A0)(uint32_t a, uint32_t b);
    void     (*f007A0)(uint32_t p);
    uint32_t (*tab_ce4)(uint32_t idx);                     // byte at DAT_6fd22ce4 + idx*12
    uint32_t (*tab_ce8)(uint32_t idx);                     // dword at DAT_6fd22ce8 + idx*12
    uint32_t datF80, datF7C;                               // DAT_6fd31f80 / DAT_6fd31f7c (cached codes)
};
static const B64Env* g_b;
extern "C" void d2_b64_setenv(const B64Env* e) { g_b = e; }

static inline bool unitHasPath(int32_t t) { return t == 2 || (t > 3 && t < 6); }
static uint32_t unitX(uint32_t u) { uint32_t p = rd32(u, 0x2c); return unitHasPath(rd32(u, 0)) ? rd32(p, 0xc) : p ? g_b->o10867(p) : 0; }
static uint32_t unitY(uint32_t u) { uint32_t p = rd32(u, 0x2c); return unitHasPath(rd32(u, 0)) ? rd32(p, 0x10) : p ? g_b->o10750(p) : 0; }

// 1.13c D2Game+0xBD040 (thiscall this=game; EAX=level, EBP=seed ptr, EDI=fallback, stack words factorA/factorB;
// stackWord8 is the uninitialised-in-decompile fall-out value)
uint32_t d2game_bd040(uint32_t game, uint32_t classId, int32_t level, uint32_t seedPtr, uint32_t fallback,
                      int32_t factorA, int32_t factorB, uint32_t stackWord8) {
    if (rd32(game, 0x70) == 0) return classId;
    uint32_t rec = g_b->o10695(classId);
    if (rec == 0) return classId;
    uint32_t seed = rd32(game, 0x1d24);
    uint32_t out10 = rd32(rec, 0x8c);
    uint32_t c = rd32(rec, 0x88);
    if (c == 0x20202020 || c == 0) return classId;
    uint32_t r = g_b->o10450(c, &out10);
    int32_t v;
    if (r == 0 || (v = (level - rd8(r, 0xfd)) * factorA + 1) <= 0) return classId;
    if ((int32_t)g_b->rand211D0(seedPtr) < v) return fallback;
    c = rd32(rec, 0x8c);
    if (c != 0x20202020 && c != 0) {
        r = g_b->o10450(seed + 8, &out10);
        if (r != 0 && (v = (level - rd8(r, 0xfd)) * factorB + 1) > 0 && (int32_t)g_b->rand211D0(seedPtr) < v)
            return out10;
    }
    return stackWord8;
}

// 1.13c D2Game+0xBE2A0 (thiscall this=room/owner; param_1=game(arg); EAX=unit list head, EBX=drop ratio record,
// EBP=replacement param, piStack1c=room arg of the spawn call)
void d2game_be2a0(uint32_t self, uint32_t param1, uint32_t listHead, uint32_t ratioRec, uint32_t ebp, uint32_t roomArg) {
    uint32_t tbl = g_b->o10041();
    if (!tbl || !g_b->o10237(rd8(param1, 0x6d))) return;
    uint32_t codeA = g_b->datF80, codeB = g_b->datF7C;
    if (!codeA) g_b->o10450(0x206e6972, &codeA);
    if (!codeB) g_b->o10450(0x20756d61, &codeB);
    int32_t chanceBase = 0;
    uint32_t blk = g_b->o10045("..\\Source\\D2Game\\UNIT\\SUNITNPC.CPP", 0xd94);
    if (!blk) return;
    wr32(blk, 0, g_b->o10387(rd32(param1, 0x1c), 0));
    wr32(blk, 4, self ? rd32(self, 0xc) : 0xffffffffu);
    wr32(blk, 8, rd32(listHead, 8));
    wr32(listHead, 8, blk);
    int32_t lvl = (int32_t)g_b->o10973(self, 0xc, 0);
    uint32_t seed = rd32(param1, 0x1d24);
    uint64_t* sp = (uint64_t*)(uintptr_t)(seed + 8);
    int32_t n = 0;
    for (;;) {
        uint64_t s = (uint64_t)(uint32_t)*sp * 0x6ac690c5ull + rd32(seed, 0xc);
        *sp = s;
        int32_t idx = lvl - 5 + (int32_t)((s & 0xffffffff) % 10);
        if (idx < 6) idx = 5; else if (idx > 0x62) idx = 99;
        uint32_t cnt = rd32(tbl, 8 + idx * 4), pick;
        if ((int32_t)cnt < 1) pick = 0;
        else if ((cnt & (cnt - 1)) == 0) { s = (s & 0xffffffff) * 0x6ac690c5ull + (s >> 32); *sp = s; pick = (cnt - 1) & (uint32_t)s; }
        else { s = (s & 0xffffffff) * 0x6ac690c5ull + (s >> 32); *sp = s; pick = (uint32_t)((s & 0xffffffff) % cnt); }
        uint32_t id = rd32(rd32(tbl, 4), pick * 4);
        if (rd32(param1, 0x70) == 0) {
            uint32_t rec = g_b->o10695(id);
            if (!rec) return;
            if (rd16(rec, 0xf6) >= 100) goto next;
        }
        {
            if (n < 2) id = n ? codeB : codeA;
            uint32_t cls = d2game_bd040(param1, id, 0, 0, 0, 0, 0, 0);   // retail forwards the live register state; callers pin it
            int32_t q = 4;
            uint32_t pa = ebp;
            if (chanceBase > 0) {
                s = (uint64_t)(uint32_t)*sp * 0x6ac690c5ull + rd32(seed, 0xc);
                *sp = s;
                int32_t roll = (int32_t)((s & 0xffffffff) % 100000);
                pa = ebp;
                if (roll < chanceBase) q = roll < (int32_t)rd32(ratioRec, 0x4c) ? 7 : ((int32_t)(rd32(ratioRec, 0x48) + rd32(ratioRec, 0x4c)) <= roll) + 5;
            }
            n++;
            uint32_t u = g_b->spawn31980(1, 0, roomArg, cls, pa, 4, q, 0, 0, 0);
            if (u) {
                g_b->o10012(u, 0);
                if (g_b->o10087(u)) {
                    if (g_b->o10711(u) && g_b->o10048(u)) g_b->o10887(u, 0x46, g_b->o10463(u), 0);
                    g_b->f2380(0, u);
                    if (!g_b->o10202(u, 0x100, 0xba, "..\\Source\\D2Game\\UNIT\\SUNITNPC.CPP")) {
                        int32_t v = (int32_t)g_b->o11116(u);
                        if (v > 0) g_b->o10887(u, 0x48, v, 0);
                    } else g_b->f5690(pa);
                }
                g_b->o10977(u, 0x10, 0);
                int32_t t = rd32(u, 0);
                uint32_t p = rd32(u, 0x2c);
                if (t != 2 && (t < 4 || t > 5) && p) { g_b->o10750(p); g_b->o10867(p); }
                if (g_b->f76a0(pa, roomArg, rd32(u, 0xc)) == 0) { g_b->f2ee70(); return; }
            }
        }
    next:
        if (n > 0xd) return;
    }
}

// 1.13c D2Game+0xC39B0 (fastcall ecx=param_1)
void d2game_c39b0(uint32_t p) {
    uint32_t d = rd32(p, 0x18);
    if (g_b->fE660(0x2066736d)) wr32(d, 0x18, rd32(d, 0x18) + 1);
    if (g_b->fE660(0x20706976)) wr32(d, 0x24, rd32(d, 0x24) + 1);
    if (g_b->fE660(0x20786f62)) wr32(d, 0x20, rd32(d, 0x20) + 1);
    if (g_b->fE660(0x20747368)) wr32(d, 0x1c, rd32(d, 0x1c) + 1);
    if (rd8(d, 0x2b) != 0 &&
        ((int32_t)rd32(d, 0x1c) > 0 || (rd32(d, 0x20) != 0 && rd32(d, 0x24) != 0 && rd32(d, 0x18) != 0))) {
        uint32_t inv = rd32(rd32(p, 4), 0x10f4);
        if (!inv) return;
        uint32_t it = rd32(inv, 0);
        if (!it) return;
        while (rd32(it, 0) != 0xd) { it = rd32(it, 0xf4); if (!it) return; }
        if (rd8(it, 9) != 0 && rd8(rd32(it, 0x18), 0xe) != 1) wr8(rd32(it, 0x18), 0x10, 0);
    }
}

// 1.13c D2Game+0xC8FD0 (cdecl; ESI=pos pointer {x,y} as hidden input)
void d2game_c8fd0(uint32_t unit, uint32_t room, int32_t* pos) {
    uint32_t d = rd32(unit, 0x18);
    if (rd8(d, 0xc) == 1) {
        uint32_t game = rd32(unit, 4);
        if (!game) { g_b->o10024("", g_b->o10265(0x2d3)); return; }   // retail _exit(-1)
        for (uint32_t n = rd32(game, 0x1320 + (rd32(d, 0x3c) & 0x7f) * 4); n; n = rd32(n, 0xe4)) {
            if (rd32(n, 0xc) != rd32(d, 0x3c)) continue;
            uint32_t list = 0;
            if (rd32(n, 0) == 1 && rd32(n, 0x14)) list = rd32(rd32(n, 0x14), 0x30);
            for (uint32_t e = rd32(list, 0); e; e = rd32(e, 8))
                if (rd32(e, 0) != 0) { g_b->f68410(game, n, 0); return; }
            g_b->f49820();
            g_b->fE20A0(game, n);
            wr8(d, 0xc, 0);
            break;
        }
    }
    if (rd8(d, 0xd) == 1) return;
    pos[0] += (g_b->fC1B60() & 0xff) ? 0xf : -10;
    pos[1] -= 3;
    uint32_t c = g_b->f1280();
    g_b->f54130(room, (uint32_t)(uintptr_t)pos, 3, 0x100);
    struct { uint32_t game, a, b, c, d, e, f, g; uint16_t h; } q = {rd32(unit, 4), c, 0, 0xc9, 1, (uint32_t)pos[0], (uint32_t)pos[1], 0xffffffffu, 0};
    uint32_t u = g_b->fEF10(&q);
    if (!u) u = g_b->fF650(0xc9, 1, rd32(unit, 4), c, pos[0], pos[1], 2, 0);
    if (u) {
        wr32(u, 0xc4, rd32(u, 0xc4) | 0x3000000);
        d = rd32(unit, 0x18);
        wr8(d, 0xd, 1);
        if (rd8(d, 0x15) == 0) { wr8(d, 0x15, 1); wr32(d, 0x28, pos[0]); wr32(d, 0x2c, pos[1]); }
    }
}

// 1.13c D2Game+0xCB1F0 (cdecl param_1; EBX=player unit as hidden input)
uint32_t d2game_cb1f0(uint32_t param1, uint32_t ebxUnit) {
    if (g_b->o10601(param1, "..\\Source\\D2Game\\PLAYER\\PlrMsg.cpp", 0x6d5)) return param1;
    uint32_t inv = rd32(ebxUnit, 0x60);
    uint32_t a = g_b->o11139(inv, 4), b;
    if (a && (!g_b->o11006(a) || g_b->o11088(a) == 0x26)) a = 0;
    b = g_b->o11139(inv, 5);
    if (b && (!g_b->o11006(b) || g_b->o11088(b) == 0x26)) b = 0;
    if (a && b && g_b->o10744(a, 0x2d) && g_b->o10744(b, 0x2d)) {
        if (!g_b->o10631(inv)) {
            uint32_t r = g_b->o10708(0xffffffffu);
            if (r) { g_b->o11031(inv, 5); return r; }
            g_b->o10024("", g_b->o10265(0x6ee));   // retail _exit(-1)
            return 0;
        }
        g_b->o11031(inv, 0);
    }
    return param1;
}

// 1.13c D2Game+0xCB690 (EAX=unit, EBX=range; returns 1 when the point is farther than range on either axis)
uint32_t d2game_cb690(uint32_t unit, int32_t range, uint16_t px, uint16_t py) {
    int32_t dx = (int32_t)unitX(unit) - px; if (dx < 0) dx = -dx;
    int32_t dy = (int32_t)unitY(unit) - py; if (dy < 0) dy = -dy;
    return (dx <= range && dy <= range) ? 0 : 1;
}

// 1.13c D2Game+0xCBA20 (fastcall ecx=param_1, edx=unit; retaddr word=table index; EBP high byte)
void d2game_cba20(uint32_t ecx, uint32_t unit, uint32_t tblIdx, uint32_t ebp) {
    uint32_t path = rd32(unit, 0x2c);
    uint8_t pk[16] = {0};
    pk[0] = 0xf;
    uint32_t id = rd32(unit, 0xc);
    uint16_t w10764 = g_b->o10764(path), w10983 = g_b->o10983(path);
    std::memcpy(pk + 2, &id, 4);
    pk[6] = (uint8_t)g_b->tab_ce4(tblIdx);
    std::memcpy(pk + 7, &w10983, 2);
    std::memcpy(pk + 9, &w10764, 2);
    pk[11] = (uint8_t)(ebp >> 24);
    uint16_t y = (uint16_t)unitX(unit);   // retail's last stored word is the path X getter result
    std::memcpy(pk + 12, &y, 2);
    g_b->fAA3E0(ecx, pk, 0x10);
}

// 1.13c D2Game+0xCBF50 (EAX=packet ptr, ESI=unit, retaddr word=game, slotAddr5=the write target ghidra shows as
// absolute address 5)
int32_t d2game_cbf50(uint32_t pkt, uint32_t unit, uint32_t game, int32_t param2, uint16_t* out1, uint16_t* out2,
                     uint32_t slotAddr5) {
    *out1 = 0; *out2 = 0;
    if (param2 != 5) return 3;
    uint32_t p = g_b->o10920(unit);
    if (!p) return 2;
    uint16_t y = rd16(pkt, 3), x = rd16(pkt, 1);
    uint32_t far_ = d2game_cb690(unit, 0, x, y);   // retail passes EBX = range pinned by caller
    if (far_) {
        if (0x19 < (int32_t)(rd32(game, 0xa8) - rd32(p, 0x168)) && unit && rd32(unit, 0) == 0) {
            uint32_t q = rd32(g_b->o10920(unit), 0x9c);
            if (q) {
                uint32_t id = rd32(unit, 0xc);
                uint16_t a = (uint16_t)g_b->f1130(unit), b = (uint16_t)g_b->f1100(unit);
                g_b->fBF80(0, id, q, (int16_t)b, a, 1);
            }
        }
        return (int32_t)far_;
    }
    wr32(p, 0x168, rd32(game, 0xa8));
    if (slotAddr5) wr16(slotAddr5, 0, x);
    *out1 = y;
    return 0;
}

// 1.13c D2Game+0xCE040 (fastcall ecx=param_1, edx=param_2; stack param_3, param_4; hidden regs of 0xCBF50 forwarded)
int32_t d2game_ce040(uint32_t p1, uint32_t p2, uint16_t p3, uint32_t p4, uint32_t pkt, uint32_t unit, uint32_t game, uint32_t slot5) {
    uint16_t o2; int32_t r = d2game_cbf50(pkt, unit, game, (int32_t)p4, &p3, &o2, slot5);
    if (r == 0) { g_b->f98550(p1, p2, 0, 3, p3, o2, 0); r = 0; }
    return r;
}

// 1.13c D2Game+0xCE0D0 (same shape as 0xCE040 with mode 2)
int32_t d2game_ce0d0(uint32_t p1, uint32_t p2, uint16_t p3, uint32_t p4, uint32_t pkt, uint32_t unit, uint32_t game, uint32_t slot5) {
    uint16_t o2; int32_t r = d2game_cbf50(pkt, unit, game, (int32_t)p4, &p3, &o2, slot5);
    if (r == 0) { g_b->f98550(p1, p2, 0, 2, p3, o2, 0); r = 0; }
    return r;
}

// 1.13c D2Game+0xCE120 (EBX, ESI hidden inputs)
void d2game_ce120(uint32_t ebx, uint32_t esi) {
    g_b->f98430(0, 3, ebx, esi, 0);
    g_b->o10329();
}

// 1.13c D2Game+0xCE150 (fastcall ecx=param_1, edx=param_2=unit; param_3=skill record ptr; tblIdx = caller-frame
// index into the 12-byte table; retaddr = param passed on to 0xC230)
void d2game_ce150(uint32_t p1, uint32_t unit, uint32_t rec, uint32_t tblIdx, uint32_t retWord) {
    uint16_t y = 0;
    if (!rec) { g_b->o10024("", g_b->o10265(0xfb)); return; }   // retail _exit(-1)
    uint32_t cur;
    if ((rd8(rec, 0x3d4) & 1) == 0) {
        if (rd32(rec, 0x174) == 0) cur = 0;
        else { cur = g_b->f003A0(rd32(rec, 0x1a8), rd32(rec, 0x170)); if (!cur) wr32(rec, 0x174, 0); }
    } else cur = rd32(rec, 0x174);
    if (unit == cur) return;
    g_b->f007A0(p1);
    uint32_t other = g_b->o10392(rd32(unit, 0x2c));
    if (other != unit && other) {
        uint32_t o = g_b->o10331(other);
        if (o) {
            uint32_t cnt = rd32(o, 0x78);
            for (uint32_t i = 0; i < cnt; i++)
                if (rd32(rd32(o, 0x48), i * 4) == rec) {
                    g_b->fC230(0, rd32(unit, 0xc), retWord, g_b->tab_ce8(tblIdx), (uint8_t)rd32(other, 0), rd32(other, 0xc),
                               (int16_t)g_b->f1100(unit), (uint16_t)g_b->f1130(unit));
                    return;
                }
        }
    }
    uint32_t path = rd32(unit, 0x2c);
    uint16_t x = unitHasPath(rd32(unit, 0)) || path ? (uint16_t)unitX(unit) : 0;
    uint16_t w1 = g_b->o10764(path), w2 = g_b->o10983(path);
    uint8_t pk[16] = {0};
    pk[0] = 0xf;
    uint32_t id = rd32(unit, 0xc);
    std::memcpy(pk + 2, &id, 4);
    pk[6] = (uint8_t)g_b->tab_ce4(tblIdx);
    std::memcpy(pk + 7, &w2, 2);
    std::memcpy(pk + 9, &w1, 2);
    pk[11] = 0;
    y = x;
    std::memcpy(pk + 12, &y, 2);
    g_b->fAA3E0(p1, pk, 0x10);
}
