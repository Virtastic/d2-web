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

// shared by 73890/73930/739C0: zeroed 0x5C-byte request record for 0xB30F0
struct Req { uint32_t w[0x17]; };
static void req_init(Req& r) { for (auto& x : r.w) x = 0; }

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
