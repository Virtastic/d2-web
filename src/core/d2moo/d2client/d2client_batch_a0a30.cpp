// src/core/d2client/d2client_batch_a0a30.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0xA0A30..0xA1540: unit-class
// predicates, skill-list walk, light-table fill, sound record, pet/party tests) from the cached decompiles.
// Alpha batch lane: implementation + marker only, no spec, no vector. Base 0x6FAB0000.
// Ordinals/unrecovered callees are host seams (the d2client_batch_88930.cpp pattern). Game pointers are 32-bit
// handles (uint32_t) handed to the seams; host pointers are only ever uintptr_t/pointers, never cast to uint32_t.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

struct D2cA0Env {
    uint32_t (*ord)(int ordinal, uint32_t a, uint32_t b, uint32_t c); // generic Ordinal_<n>(a,b,c)
    void*    (*fn)(uint32_t rva, void* a, void* b, void* c);          // unrecovered in-DLL callee by RVA
    void     (*fatal)(uint32_t code);                                 // Ordinal 10265+10024+_exit(-1)
};
struct D2cA0State {
    uint8_t*  dt;          // *sgptDataTables
    uint8_t*  list;        // DAT_6fbcc4d4 node: +8 key, +0xC value, +0x30 next
    uint32_t* bitMasks;    // gdwBitMasks
    uint32_t* tabHi;       // DAT_6fb84390 (64 u32)
    uint32_t* tabLo;       // DAT_6fb84290 (64 u32)
};

template <class T> static T rd(const void* p, size_t o) { T v; memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
template <class T> static void wr(void* p, size_t o, T v) { memcpy((uint8_t*)p + o, &v, sizeof v); }
static uint8_t* nextNode(uint8_t* n) { return rd<uint8_t*>(n, 0x30); }
static uint8_t* tblRec(const D2cA0State* s, size_t cntOff, size_t baseOff, int32_t i, size_t stride) {
    if (i < 0 || i >= rd<int32_t>(s->dt, cntOff)) return nullptr;
    return rd<uint8_t*>(s->dt, baseOff) + (size_t)i * stride;
}

// Legacy shim for d2_client_a0e70 (the retail port of 0xA0A30 is d2client_a0a30.cpp).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a0a30(const D2cA0Env* e, uint8_t* u) {
    if (!u) return 0;
    if (rd<int32_t>(u, 0) != 0) return 1;
    uint32_t p = e->ord(10641, (uint32_t)(uintptr_t)u, 0, 0);
    bool b = true;
    if (!p) return 1;
    if (e->ord(10543, p, 0, 0)) {
        b = false;
        uint32_t c = e->ord(10511, 0, 0, 0);
        if (!e->ord(10601, c, 0, 0) && e->ord(10910, p, 0x9d, 0)) return 1;
    }
    uint32_t h = rd<uint32_t>(u, 0x18 * 4);
    if (!h) return 0;
    uint32_t x = e->ord(11139, h, 4, 0);
    if (!x || (b && x != p) ||
        (!e->ord(10048, x, 0, 0) && !e->ord(10711, x, 0, 0) && !e->ord(10910, x, 0x7d, 0))) {
        x = e->ord(11139, h, 5, 0);
        if (!x) return 0;
        if (b && x != p) return 0;
        if (!e->ord(10048, x, 0, 0) && !e->ord(10711, x, 0, 0) && !e->ord(10910, x, 0x7d, 0)) return 0;
    }
    return e->ord(10104, x, 0, 0);
}

// 1.13c D2Client+0xA0E70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a0e70(const D2cA0Env* e, uint8_t* u, uint32_t a) {
    if (!d2_client_a0a30(e, u)) return 0;
    return e->ord(10641, a, 0, 0) != 0;
}

// 1.13c D2Client+0xA1110
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a1110(const D2cA0Env* e, const D2cA0State* s, uint8_t* p) {
    e->fn(0x2a0b0, nullptr, nullptr, nullptr);
    uint32_t h = rd<uint32_t>(p, 0x2c);
    if (!h) { e->fatal(0x946); return 0; }
    int32_t* m = (int32_t*)(uintptr_t)e->ord(10392, h, 0, 0);
    if (!m || m[0] != 1) return 0;
    int32_t k = -1;
    for (uint8_t* n = s->list; n; n = nextNode(n))
        if (rd<int32_t>(n, 8) == m[3]) { k = rd<int32_t>(n, 0xc); break; }
    if (k != rd<int32_t>(p, 0xc)) return 0;
    int32_t i = (int32_t)(uintptr_t)e->fn(0x1670, (void*)(intptr_t)m[3], nullptr, nullptr);
    uint8_t* r = tblRec(s, 0xbf0, 0xbe8, i, 0xe0);
    return r && (r[4] & ((const uint8_t*)s->bitMasks)[0xc]) != 0;
}

// 1.13c D2Client+0xA1260
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a1260(const D2cA0Env* e, uint8_t* p) {
    uint32_t ret = 7;
    e->fn(0x2a0b0, nullptr, nullptr, nullptr);
    uint32_t h = rd<uint32_t>(p, 0x2c);
    if (!h) { e->fatal(7); return 0; }
    int32_t* m = (int32_t*)(uintptr_t)e->ord(10392, h, 0, 0);
    bool b = true;
    if (m && m[0] == 1) {
        uint8_t* r = (uint8_t*)e->fn(0x1260, (void*)(intptr_t)m[1], nullptr, nullptr);
        b = r[0xb] != 2;
        if (!b) ret = 8;
        if (r[0xb] == 1) b = false;
    }
    if (e->ord(10889, (uint32_t)(uintptr_t)p, 0, 0)) { ret = 7; b = false; }
    uint32_t tmp[3];
    if (!e->ord(10101, rd<uint32_t>(p, 0x60), (uint32_t)(uintptr_t)tmp, 0)) return 7;
    if (b) {
        uint64_t v = (uint64_t)rd<uint32_t>(p, 0x20) * 0x6ac690c5u + rd<uint32_t>(p, 0x24);
        wr<uint64_t>(p, 0x20, v);
        if (((uint32_t)v & 0x3f) < 0x20) ret = 8;
    }
    return ret;
}
