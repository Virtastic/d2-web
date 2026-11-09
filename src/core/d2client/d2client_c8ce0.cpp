// 1.13c D2Client+0xC8CE0: spawn the cast-effect requests of a skill row. Disassembly: EAX = unit, stack id, p2, px, py, q (ret 0x14).
// Builds a 23-dword request (0x2021, unit twice, the row's effect id, px/py, the id, p2), randomises it with 0xA2380 according to the bit masks
// of the 0x1260 record, then submits up to two requests through 0xB30F0 and, when 0xA5F70 is zero, the 0x88A70 follow-up.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2cC8ce0Env {
    const uint8_t* (*f1260)(uint32_t q);                                  // EAX = q; record pointer or 0 (byte +5 is the class mask)
    uint32_t (*f1330)(uint32_t n, uint32_t seedAddr);                     // EAX = n, ECX = &unit->seed (unit+0x20)
    void (*a2380)(uint32_t unit, uint32_t* req, uint32_t count, uint32_t s0);   // ECX = count, ESI = unit, EDI = &req, one stack word, ret 4
    const uint8_t* (*f1580)(uint32_t row);                                // EAX = row index, record pointer (shorts +0x18, +0x1a) or 0
    uint32_t (*b30f0)(uint32_t* req);                                     // ECX = &req, plain ret
    uint32_t (*a5f70)(uint32_t unit);                                     // ESI = unit
    void (*f88a70)(int32_t ebx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);   // EBX = short, 4 stack words, ret 0x10
};
static uint32_t c8_rd(const uint8_t* p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static int32_t c8_s16(const uint8_t* p, size_t o) { int16_t v; memcpy(&v, p + o, 2); return v; }
extern "C" {
// dt = *sgptDataTables (+0xB6C row count of the 0x1A4 table, +0xB98 skill rows 0x23C, +0xBA0 count); bm = gdwBitMasks (dwords [2], [3]);
// unit = host pointer of the unit blob (only +0xC4 is written), unitH its 32-bit handle (what retail pushes).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_c8ce0(const D2cC8ce0Env* e, const uint8_t* dt, const uint32_t* bm, uint8_t* unit, uint32_t unitH,
                                        int32_t id, uint32_t p2, uint32_t px, uint32_t py, uint32_t q) {
    if (id < 0 || id >= (int32_t)c8_rd(dt, 0xba0)) return 0;
    const uint8_t* rec = (const uint8_t*)(uintptr_t)c8_rd(dt, 0xb98) + (size_t)(uint32_t)id * 0x23c;
    if (!rec) return 0;
    int32_t eb = c8_s16(rec, 0xea);
    if (eb < 0 || eb >= (int32_t)c8_rd(dt, 0xb6c) || !px || !py) return 0;
    const uint8_t* qr = e->f1260(q);
    if (!qr) return 0;
    if (unit) { uint32_t f = c8_rd(unit, 0xc4) | 0x40; memcpy(unit + 0xc4, &f, 4); }
    uint32_t req[23] = {};
    req[0] = 0x2021; req[1] = unitH; req[2] = unitH;
    req[7] = e->f1330(3, unitH + 0x20) - 1 + px;
    req[8] = e->f1330(3, unitH + 0x20) - 1 + py;
    req[6] = py; req[5] = px; req[11] = (uint32_t)id; req[12] = p2; req[4] = (uint32_t)eb;
    uint32_t b = qr[5];
    const uint32_t nrow = c8_rd(dt, 0xb6c);
    if (bm[2] & b) {
        e->a2380(unitH, req, 1, 0);
    } else if (bm[3] & b) {
        e->a2380(unitH, req, 3, 3);
        const uint8_t* r = e->f1580((uint32_t)eb);
        int32_t s = c8_s16(r, 0x18);
        if (s > 0 && s < (int32_t)nrow) { req[4] = (uint32_t)s; e->a2380(unitH, req, 4, 2); }
    } else {
        e->a2380(unitH, req, 2, 2);
        const uint8_t* r = e->f1580((uint32_t)eb);
        int32_t s = c8_s16(r, 0x1a);
        if (s > 0 && s < (int32_t)nrow) { req[4] = (uint32_t)s; e->a2380(unitH, req, 3, 1); }
    }
    req[6] = py; req[5] = px;
    uint32_t edi = 0;
    int32_t a = c8_s16(rec, 0xec);
    if (a > 0 && a < (int32_t)nrow) { req[4] = (uint32_t)a; edi = e->b30f0(req); }
    a = c8_s16(rec, 0xee);
    if (a > 0 && a < (int32_t)nrow) { req[12] = 2; req[4] = (uint32_t)a; e->b30f0(req); }
    if (e->a5f70(unitH) == 0 && edi != 0) {
        int32_t v = c8_s16(rec, 0x10a);
        if (v > 0) e->f88a70(v, edi, 0, 0, 0);
    }
    return 1;
}
}
