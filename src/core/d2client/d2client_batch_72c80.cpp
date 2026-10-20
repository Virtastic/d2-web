// src/core/d2client/d2client_batch_72c80.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x72C80..0x73DF0)
// from the cached decompiles. Alpha batch lane: implementation + marker, no spec/vector. Base 0x6FAB0000.
// Every callee is one host seam call(code,a..e): code is a D2Client RVA, or 0x80000000|ordinal for an
// ordinal import (the batch convention). Data-table reads go through tbl(off): the dword at
// sgptDataTables+off (counts, or base pointers widened to wasm64 pointers).
// FLAGS: Ghidra shows unaffected registers (unaff_ESI/EBX/EDI/retaddr, extraout_ECX) in 72C80, 734D0,
// 73BC0: those are non-fastcall register args, modelled here as explicit trailing parameters (guess).
// Ordinal arg lists are as decompiled; Ghidra dropped arguments on several (e.g. 10680 called with 2/0/3
// args), so those calls pass the visible args only (guess). 73000's sentinels (0x163 as a pointer) are
// decompiler artefacts of register reuse; reproduced literally.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

typedef uintptr_t W;
struct D2cB72Env {
    W (*call)(uint32_t code, W a, W b, W c, W d, W e);
    W (*tbl)(uint32_t off);                       // dword/pointer at sgptDataTables+off
    uint32_t (*bitmask)(uint32_t idx);            // gdwBitMasks[idx]
};
static inline const uint8_t* rec(const D2cB72Env* e, uint32_t cntOff, uint32_t baseOff, uint32_t stride, int32_t i) {
    if (i < 0 || i >= (int32_t)e->tbl(cntOff)) return nullptr;
    return (const uint8_t*)(e->tbl(baseOff) + (W)i * stride);
}
static inline const uint8_t* skillRec(const D2cB72Env* e, int32_t i) { return rec(e, 0xc4, 0xbc, 0x3c, i); }
static inline const uint8_t* ordRec(const D2cB72Env* e, int32_t i)   { return rec(e, 0xba0, 0xb98, 0x23c, i); }
static inline int16_t s16(const uint8_t* p, int o) { return *(const int16_t*)(p + o); }
static const uint32_t ORD = 0x80000000u;
#define CALL(e, c, a, b, cc, d, ee) ((e)->call((c), (W)(a), (W)(b), (W)(cc), (W)(d), (W)(ee)))

extern "C" {

// 1.13c D2Client+0x72C80
EMSCRIPTEN_KEEPALIVE void d2c_72c80(const D2cB72Env* e, int32_t skill, W unit /*EBX*/, int32_t esi, int32_t retaddr) {
    const uint8_t* r = skillRec(e, skill);
    if (!r || (e->bitmask(0xc) & r[0x14])) return;
    int32_t a = s16(r, 2) < 0 ? -1 : s16(r, 2);
    int32_t b = s16(r, 4) < 0 ? -1 : s16(r, 4);
    int32_t has = (int32_t)CALL(e, ORD | 10190, skill, 0, 0, 0, 0);
    int32_t pa = skill;
    auto adj = [&](int32_t& v) {
        CALL(e, ORD | 10799, v, 0, 0, 0, 0);
        v += (int32_t)CALL(e, 0x1380, unit + 0x20, 0, 0, 0, 0);  // &unit[8] (retail int*, +0x20)
        pa = retaddr;
    };
    if (esi != -1) adj(esi);
    if (a != -1) adj(a);
    if (b != -1) adj(b);
    if (has) { CALL(e, ORD | 10945, 0, 0, 0, 0, 0); CALL(e, ORD | 10625, 0, 0, 0, 0, 0); CALL(e, ORD | 10945, 0, 0, 0, 0, 0); }
    if (esi != -1) { CALL(e, 0x6B300, unit, has ? 9 : 4, 0, a, b); return; }
    if (a != -1) {
        int32_t x = has ? pa : 0, k = has ? 8 : 3;
        CALL(e, 0x6B300, unit, k, 0, x, 0);
    }
    if (b != -1) {
        if (has) { CALL(e, 0x6B300, unit, 8, 0, pa, 0); return; }
        CALL(e, 0x6B300, unit, 3, 0, 0, 0);
    }
}

// 1.13c D2Client+0x73000
EMSCRIPTEN_KEEPALIVE void d2c_73000(const D2cB72Env* e, W p1 /*ECX*/, int32_t skill /*EDX*/) {
    if (!skillRec(e, skill)) return;
    W u = CALL(e, ORD | 10871, p1, skill, 0, 0, 0);
    if (!u) return;
    int32_t v84 = (int32_t)CALL(e, ORD | 10680, u, 0x84, 0, 0, 0);
    CALL(e, ORD | 10680, u, 0x85, 0, 0, 0);
    int32_t v3 = (int32_t)CALL(e, ORD | 10680, 0, 0, 0, 0, 0);
    uint32_t n = v84 < 0 ? 1 : 3;
    CALL(e, ORD | 10188, u, 0x163, n, 0, 0);
    if (v3 < (int32_t)n) {
        CALL(e, 0x729A0, 0x163, 0, 0, 0, 0);
        if (u != (W)-1 && n > 2) { CALL(e, 0x6B300, 0x163, 3, 0, 0, 0); CALL(e, 0x6B300, 0x163, 3, 0, 0, 0); }
        if (n > 1) { CALL(e, 0x6B300, 0x163, 3, 0, 0, 0); CALL(e, 0x6B300, 0x163, 3, 0, 0, 0); }
        if (u != (W)-1 && n != 0) { CALL(e, 0x6B300, 0x163, 3, 0, 0, 0); CALL(e, 0x6B300, 0x163, 3, 0, 0, 0); }
    } else {
        // retail: iVar1 != 0 && uVar4 < 3 ; iVar5 (== iVar1) != 0 && iVar5 < 2 (signed)
        if (u != 0 && n < 3) { CALL(e, 0x727F0, u, 0, 0, 0, 0); CALL(e, 0x727F0, u + 1, 0, 0, 0, 0); }
        if (u != 0 && (int32_t)u < 2) { CALL(e, 0x727F0, u, 0, 0, 0, 0); CALL(e, 0x727F0, u + 1, 0, 0, 0, 0); }
    }
}

// 1.13c D2Client+0x734D0   (unit = ESI, p1 = ECX; extraout_ECX is whatever 0x69790 left in ECX: modelled as p1)
EMSCRIPTEN_KEEPALIVE void d2c_734d0(const D2cB72Env* e, W p1, int32_t* unit) {
    for (int i = 0; i < 0x10; i++) CALL(e, 0x69790, p1, unit, 0, 0, 0);
    if (unit) {
        bool go = false;
        if (unit[0] == 0) { if (unit[4] == 0x12) return; go = true; }
        else if (unit[0] == 1) go = true;
        if (go) {
            unit[0x31] &= 0xffffffbf;
            if (CALL(e, ORD | 11090, 0, 0, 0, 0, 0)) {
                CALL(e, 0x6A220, 0, 0, 0, 0, 0);
                CALL(e, 0x2A590, 1, unit, 0, 0, 0);
                CALL(e, 0x24490, 1, 0, 0, 0, 0);
            }
        }
    }
    CALL(e, 0x6A220, 0, 0, 0, 0, 0);
    CALL(e, ORD | 10595, 0, 0, 0, 0, 0);
    unit[0x11] = 0;   // retail derefs unconditionally
    CALL(e, ORD | 11103, 0, 0, 0, 0, 0);
}

// 1.13c D2Client+0x73560
EMSCRIPTEN_KEEPALIVE void d2c_73560(const D2cB72Env* e, int32_t* u) {
    if (u) u[0x31] &= 0xffffffbf;
    if (CALL(e, ORD | 11090, u, 1, 0, 0, 0)) {
        CALL(e, 0x6A220, 0, 0, 0, 0, 0);
        CALL(e, 0x2A590, 1, u, 0, 0, 0);
        CALL(e, 0x24490, 1, 0, 0, 0, 0);
    }
    if (u) u[0x31] |= 0x80000000u;
    CALL(e, ORD | 10704, u, 0, 0, 0, 0);
}

// 1.13c D2Client+0x735B0   (u[0xb] is a retail 32-bit pointer slot; passed through as W)
EMSCRIPTEN_KEEPALIVE void d2c_735b0(const D2cB72Env* e, int32_t* u, W p2, W u0xb) {
    if (!u || u[0] != 1) return;
    W h = CALL(e, ORD | 10871, u, p2, 0, 0, 0);
    if (!h) return;
    int32_t v = (int32_t)CALL(e, ORD | 10680, h, 0x163, 0, 0, 0);
    CALL(e, 0x4D610, v, 0, 0, 0, 0);
    CALL(e, 0x2A6C0, 0, 0, 0, 0, 0);
    if (v == 0x21f) {
        W y = CALL(e, 0x1660, u, 0, 0, 0, 0) + 10;
        W x = CALL(e, 0x1630, u, 0, 0, 0, 0);
        CALL(e, ORD | 10739, u0xb, x, y, 1, 0);
    }
}

// shared by 73890/73930/739C0: zeroed 0x5C-byte request record for 0xB30F0
struct Req { uint32_t w[0x17]; };
static void req_init(Req& r) { for (auto& x : r.w) x = 0; }

// 1.13c D2Client+0x73890
EMSCRIPTEN_KEEPALIVE void d2c_73890(const D2cB72Env* e, W unit, int32_t skill) {
    CALL(e, 0x4C500, unit, 0, 0, 0, 0);
    const uint8_t* r = skillRec(e, skill);
    if (!r || s16(r, 0x3a) < 0 || s16(r, 0x3a) >= (int32_t)e->tbl(0xb6c)) return;
    int16_t s = s16(r, 0x38);
    Req q; req_init(q);
    q.w[0x10 / 4 + 0] = 0;  // keep layout explicit below
    // retail locals: [1]=[2]=unit (local_58/54), +0x10 = local_4c = s16(0x3a), +0x2c = local_30 = max(s,0), +0x30 = 1
    q.w[1] = (uint32_t)unit; q.w[2] = (uint32_t)unit;
    q.w[0x10 / 4 + 0] = 0;
    q.w[(0x5c - 0x4c) / 4] = (uint32_t)(int32_t)s16(r, 0x3a);
    q.w[(0x5c - 0x30) / 4] = s < 0 ? 0 : (uint32_t)(int32_t)s;
    q.w[(0x5c - 0x2c) / 4] = 1;
    CALL(e, 0xB30F0, (W)&q, 0, 0, 0, 0);
}

// 1.13c D2Client+0x73930
EMSCRIPTEN_KEEPALIVE void d2c_73930(const D2cB72Env* e, W unit, int32_t skill) {
    const uint8_t* r = skillRec(e, skill);
    if (!r || s16(r, 0x3a) < 0 || s16(r, 0x3a) >= (int32_t)e->tbl(0xb6c)) return;
    int16_t s = s16(r, 0x38);
    Req q; req_init(q);
    q.w[1] = (uint32_t)unit; q.w[2] = (uint32_t)unit;
    q.w[(0x5c - 0x4c) / 4] = (uint32_t)(int32_t)s16(r, 0x3a);
    q.w[(0x5c - 0x30) / 4] = s < 0 ? 0 : (uint32_t)(int32_t)s;
    q.w[(0x5c - 0x2c) / 4] = 1;
    CALL(e, 0xB30F0, (W)&q, 0, 0, 0, 0);
}

// 1.13c D2Client+0x739C0   (unit = ECX, a 32-bit-offset struct; flags at +200 / +0xC4)
EMSCRIPTEN_KEEPALIVE void d2c_739c0(const D2cB72Env* e, uint8_t* unit, int32_t skill) {
    const uint8_t* r = skillRec(e, skill);
    if (!r) return;
    int32_t o = s16(r, 0x38);
    if (o < 0) return;
    const uint8_t* o2 = rec(e, 0xba0, 0xb98, 0x23c, o);
    if (!o2) return;
    int16_t t = s16(o2, 0xea);
    if (t < 0 || t >= (int32_t)e->tbl(0xb6c)) return;
    Req q; req_init(q);
    q.w[1] = (uint32_t)(W)unit; q.w[2] = (uint32_t)(W)unit;
    q.w[(0x5c - 0x4c) / 4] = (uint32_t)(int32_t)t;
    q.w[(0x5c - 0x30) / 4] = (uint32_t)o;
    q.w[(0x5c - 0x2c) / 4] = 1;
    CALL(e, 0xB30F0, (W)&q, 0, 0, 0, 0);
    if (unit) { *(uint32_t*)(unit + 200) |= 0x40000; *(uint32_t*)(unit + 0xc4) |= 0x20; }
}

// 1.13c D2Client+0x73AA0   (p[0] kind, p[0xb] the retail 32-bit pointer slot; wasm64 slot is p11)
EMSCRIPTEN_KEEPALIVE void d2c_73aa0(const D2cB72Env* e, const int32_t* p, const uint8_t* p11 /*[0xb] target*/) {
    uint32_t loc[7] = {0, 0, 0, 0, 0, 0, 0};
    int32_t k = p[0];
    bool direct = (k == 2) || (k > 3 && k < 6);
    loc[0] = direct ? *(const uint32_t*)(p11 + 0xc) : (p11 ? (uint32_t)CALL(e, ORD | 10867, p11, 0, 0, 0, 0) : 0);
    loc[1] = direct ? *(const uint32_t*)(p11 + 0x10) : (p11 ? (uint32_t)CALL(e, ORD | 10750, p11, 0, 0, 0, 0) : 0);
    CALL(e, 0x2A750, 0x19, p, loc, 0, 0);
    CALL(e, ORD | 10425, p11, 0, 0, 0, 0);
}

// 1.13c D2Client+0x73BC0   (ESI = unit, EDI = arg)
EMSCRIPTEN_KEEPALIVE void d2c_73bc0(const D2cB72Env* e, const int32_t* esi, W edi) {
    if (!esi || esi[0] != 3 || !edi) return;
    if (!rec(e, 0xb6c, 0xb64, 0x1a4, esi[1])) return;
    CALL(e, ORD | 10441, 0, 0, 0, 0, 0);
}

// 1.13c D2Client+0x73C10
EMSCRIPTEN_KEEPALIVE uint32_t d2c_73c10(const D2cB72Env* e, W p1, int32_t idx) {
    if (!CALL(e, ORD | 10331, p1, 0, 0, 0, 0)) return 0;
    uint32_t tmp[3] = {0, 0, 0};  // retail passes &stack0xfffffff4 (a caller-frame out buffer)
    if (!CALL(e, 0xA1340, tmp, 0, 0, 0, 0)) return 0;
    W v = CALL(e, 0x1400, 0, 0, 0, 0, 0);
    if (!v) return 0;
    const uint8_t* r = ordRec(e, idx);
    if (!r) return 0;
    if (!(e->bitmask(0) & r[5]) && CALL(e, ORD | 10057, v, 0, 0, 0, 0)) return 0;
    return 1;
}

// 1.13c D2Client+0x73DF0
EMSCRIPTEN_KEEPALIVE uint32_t d2c_73df0(const D2cB72Env* e, uint8_t* unit, int32_t idx) {
    if (!ordRec(e, idx)) return 0;
    int32_t s = (int32_t)CALL(e, 0x40550, unit, 0, 0, 0, 0);
    if (s < 0 || s >= (int32_t)e->tbl(0xb6c)) return 0;
    if (unit) *(uint32_t*)(unit + 0xc4) |= 0x40;
    if (!(CALL(e, 0xA1410, unit, 0, 0, 0, 0) & 0xff)) return 0;
    return CALL(e, 0xA1540, 0, 0, 0, 0, 0) != 0;
}

}
