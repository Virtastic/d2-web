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

// 1.13c D2Client+0xA0A30
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

// 1.13c D2Client+0xA0C40
// ponytail: the decompile drops the list-advance call; Ordinal_10273 is modelled as "advance, returns next node".
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a0c40(const D2cA0Env* e, const D2cA0State* s, uint8_t* unit, uint32_t cls, uint8_t sub) {
    if (!unit) { e->fatal(0x969); return; }
    if (!cls) { e->fatal(0x96a); return; }
    uint32_t list = rd<uint32_t>(unit, 0xa8);
    if (!list) { e->fatal(0x96c); return; }
    uint32_t n = e->ord(10784, list, 0, 0);
    while (n) {
        uint32_t cur = n;
        n = e->ord(10273, cur, 0, 0);
        int32_t id = (int32_t)e->ord(10601, cur, 0, 0);
        uint8_t* sk = tblRec(s, 0xba0, 0xb98, id, 0x23c);
        if (!sk) continue;
        uint8_t* c = (uint8_t*)e->fn(0x17f0, 0, 0, 0);
        if (!c || (int32_t)e->ord(10306, 0, 0, 0) <= 0) continue;
        if (!e->ord(11068, (uint32_t)id, 0, 0)) continue;
        if (cls && sk[0x1dc] != cls) continue;
        if (sub != 4 && (int)(int8_t)c[2] != sub + 1) continue;
        if ((int32_t)e->ord(10304, cur, 0, 0) != -1) continue;
        e->ord(10886, 0, 0, 0);
        if (e->ord(10708, 0xffffffff, 0, 0) && !e->ord(10306, 0, 0, 0)) e->ord(10773, 0, 0, 0);
    }
    e->ord(10418, 0, 0, 0);
}

// 1.13c D2Client+0xA0DB0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a0db0(const D2cA0State* s, uint32_t* alt, uint32_t* self,
                                                     void (*b30f0)(uint32_t*)) {
    for (int i = 0; i < 64; i++) {
        if (!alt) {
            self[7] = s->tabHi[i]; self[8] = s->tabLo[i];
            if (self[4] == 0xb0) self[0] ^= 0x4000;
            b30f0(self);
            continue;
        }
        uint8_t* rng = rd<uint8_t*>(self, 4);
        uint64_t v = (uint64_t)rd<uint32_t>(rng, 0x20) * 0x6ac690c5u + rd<uint32_t>(rng, 0x24);
        wr<uint64_t>(rng, 0x20, v);
        uint32_t* t = ((uint32_t)v % 6 == 0) ? alt : self;
        t[7] = s->tabHi[i]; t[8] = s->tabLo[i];
        b30f0(t);
    }
}

// 1.13c D2Client+0xA0E70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a0e70(const D2cA0Env* e, uint8_t* u, uint32_t a) {
    if (!d2_client_a0a30(e, u)) return 0;
    return e->ord(10641, a, 0, 0) != 0;
}

// 1.13c D2Client+0xA0EA0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a0ea0(const D2cA0Env* e, uint8_t* u, uint32_t a) {
    if (!e->ord(10889, a, 0, 0)) {
        uint32_t v = e->ord(10641, a, 0, 0);
        if (v && e->ord(10744, v, 0x26, 0)) return 0;
    } else if (!d2_client_a0a30(e, u)) return 0;
    return 1;
}

// 1.13c D2Client+0xA0EE0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a0ee0(const D2cA0Env* e, const D2cA0State* s, const int32_t* u, uint32_t p) {
    int32_t k;
    if (!u) k = -1;
    else {
        k = u[3];
        if (u[0] == 1) {
            uint8_t* n = s->list;
            while (n && rd<int32_t>(n, 8) != k) n = nextNode(n);
            if (!n) return 0;
            k = rd<int32_t>(n, 0xc);
            if (k == -1) return 0;
        }
    }
    uint8_t* a = (uint8_t*)e->fn(0x9d630, (void*)(uintptr_t)p, (void*)(intptr_t)(p ? (int32_t)rd<int32_t>((void*)(uintptr_t)p, 0xc) : -1), nullptr);
    uint8_t* b = (uint8_t*)e->fn(0x9d630, nullptr, (void*)(intptr_t)k, nullptr);
    return a && b && rd<int16_t>(a, 0x22) == rd<int16_t>(b, 0x22) && rd<int16_t>(a, 0x22) != -1;
}

// 1.13c D2Client+0xA0F60
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a0f60(const D2cA0Env* e, const D2cA0State* s, int32_t idx, uint32_t p1) {
    uint8_t* r = tblRec(s, 0xb6c, 0xb64, idx, 0x1a4);
    if (!r) return; // original reads +0x130 of NULL (faults)
    uint8_t v = r[0x130];
    if (!v) return;
    uint8_t* z = (uint8_t*)e->fn(0xa8c20, (void*)(uintptr_t)1, nullptr, nullptr);
    uint32_t t = v > 0x11 ? 0x12 : v;
    if (!z) { e->fatal(0x131); return; }
    wr<uint32_t>(z, 0x1c, t << 3);
    if (e->ord(10327, p1, (uint32_t)(uintptr_t)z, 0)) e->fn(0xa8630, nullptr, nullptr, nullptr);
}

// 1.13c D2Client+0xA1020
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_a1020(const D2cA0Env* e, uint8_t* u, uint32_t p1, uint32_t p2,
                                                     int32_t p3, int32_t flag, void (*a0db0)(uint32_t*)) {
    uint32_t r[23] = {0};
    int32_t t = rd<int32_t>(u, 0);
    uint32_t o = rd<uint32_t>(u, 0xb * 4);
    bool plr = t == 2 || (t > 3 && t < 6);
    r[0] = 3;
    r[11] = plr ? rd<uint32_t>((void*)(uintptr_t)o, 0xc) : (o ? e->ord(10867, o, 0, 0) : 0);
    r[12] = plr ? rd<uint32_t>((void*)(uintptr_t)o, 0x10) : (o ? e->ord(10750, o, 0, 0) : 0);
    r[1] = p1; r[10] = p2;
    if (flag) r[0] |= 4;
    if (p3 == -1) { a0db0(nullptr); return; }
    uint32_t c[23]; memcpy(c, r, sizeof c);
    // iStack_58 = auStack_68[0x10/4 .. ] : the copy ends at +0x58 relative to the copy base; p3 lands at index 16.
    c[16] = (uint32_t)p3;
    a0db0(c);
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
    return r && (r[4] & (uint8_t)s->bitMasks[0xc]) != 0;
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
        uint8_t* r = (uint8_t*)e->fn(0x1260, nullptr, nullptr, nullptr);
        b = r[0xb] != 2;
        if (!b) ret = 8;
        if (r[0xb] == 1) b = false;
    }
    if (e->ord(10889, 0, 0, 0)) { ret = 7; b = false; }
    uint32_t tmp[3];
    if (!e->ord(10101, rd<uint32_t>(p, 0x60), (uint32_t)(uintptr_t)tmp, 0)) return 7;
    if (b) {
        uint64_t v = (uint64_t)rd<uint32_t>(p, 0x20) * 0x6ac690c5u + rd<uint32_t>(p, 0x24);
        wr<uint64_t>(p, 0x20, v);
        if (((uint32_t)v & 0x3f) < 0x20) ret = 8;
    }
    return ret;
}

// 1.13c D2Client+0xA1410
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a1410(const D2cA0Env* e, const D2cA0State* s, int32_t idx, uint32_t p1) {
    uint8_t* r = tblRec(s, 0xb6c, 0xb64, idx, 0x1a4);
    uint32_t q = e->ord(10331, p1, 0, 0);
    int32_t out[2] = {0, 0};
    if (!r) { e->fatal(0x9be); return 0; }
    if (q) e->fn(0xa1340, out, nullptr, nullptr);
    return 0;
}

// 1.13c D2Client+0xA1540
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a1540(const D2cA0Env* e, void* unit) {
    int32_t out[2] = {0, 0}; // out[0]=local_4, out[1]=local_8
    if (!e->fn(0xa1340, out, nullptr, nullptr)) return 0;
    if (out[1] && out[0]) {
        uint32_t y = (uint32_t)(uintptr_t)e->fn(0x1660, unit, nullptr, nullptr);
        uint32_t x = (uint32_t)(uintptr_t)e->fn(0x1630, unit, nullptr, nullptr);
        if ((int32_t)e->ord(10769, x, y, (uint32_t)out[1]) < 0x2711)
            return (uint32_t)(uintptr_t)e->fn(0xaff10, (void*)(intptr_t)out[0], unit, nullptr);
    }
    return 0;
}
