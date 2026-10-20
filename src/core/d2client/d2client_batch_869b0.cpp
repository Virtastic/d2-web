// src/core/d2client/d2client_batch_869b0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x869B0..0x87B30: packet
// frame helpers, WardenClient.cpp module slots, two quest-list scans) from the cached decompiles.
// Alpha batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FAB0000.
// Unrecovered callees (ordinals, other RVAs, unaff_/in_ register args) are host seams (d2client_batch_5d110 pattern).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <emscripten.h>

struct D2cWardenNode {            // 0x14 object from FUN_6fab19b0; own layout, retail offsets noted
    void**   vt;                  // +0x00
    uint32_t pad04[2];
    void**   iface;               // +0x0C
    int32_t  (*entry)(void);      // +0x10 (retail: code ptr; piVar3[4])
};
struct D2cWarden {
    D2cWardenNode* cur;           // +0x10BF4C DAT_6fbcbf4c
    D2cWardenNode* pend;          // +0x10BF50 DAT_6fbcbf50
    void*    blob;                // +0x10BF44
    int32_t  blobLen;             // +0x10BF48
    uint8_t* ctx;                 // +0x10BF54 vtable owner: +0xCC, +0xC8, +0xD0 slots
    uint32_t seed;                // +0x0D137C DAT_6fb9137c
    const uint8_t* sig5;          // DAT_6fb84928 (5 bytes)
};
struct D2cQNode { void* unit; D2cQNode* next; };  // DAT_6fbcbf30 list; [0]->{+4 id, +0x30 state}

struct D2cWardenEnv {
    void (*ord11105)(void);
    void (*fn85b30)(const uint8_t* p, int32_t len);
    int  (*ord10197)(int32_t a, int32_t b);
    void (*fn85cf0)(const uint8_t* p, int32_t len);
    void (*fn84300)(const uint8_t* pkt);
    void (*ord10248)(void);
    void (*ord10249)(const void* p, int32_t n);
    void (*ord501)(void* dst, const void* src, int32_t n);
    void (*ord510)(void* p);
    int  (*ord506)(void* p);
    void (*ord10250)(void);
    void (*fatal)(void);                       // Ordinal 10265+10024+_exit(-1)
    int  (*ord491)(uint32_t* outMagic);        // reads header dword
    int  (*ord401)(int32_t n);                 // alloc, 0 = fail
    void (*free_)(void* p);                    // Ordinal_403
    int  (*sigcheck)(void);                    // Ordinal_495 chain of 87230/872D0 tail, 0 = ok
    void (*fn4e630)(void* node);               // FUN_6fb4e630 node teardown
    void (*fn1a00)(void* node);
    bool (*fn4e690)(void* blob, void* node);
    D2cWardenNode* (*fn19b0)(void* mem);
    int  (*fn78220)(int32_t n, void* blob);
    void (*fn778c0)(void* p, int32_t n);
    void (*fn1920)(void* p, int32_t n);
    uint32_t (*call_entry)(D2cWardenNode* n); // (*node+0x10)()
    void (*send4)(D2cWardenNode* n, const void* p, int32_t len); // (**node->iface)(&seed,4)
    void (*release)(D2cWardenNode* n);        // (**(cur+0xC)+4)()
};

// 1.13c D2Client+0x869B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_869b0(const uint8_t* esi, const D2cWardenEnv* e) {
    e->ord11105();
    e->fn85b30(esi + 0xd, (int32_t)esi[2] - 0xd);
}

// 1.13c D2Client+0x869E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_869e0(const uint8_t* esi, const D2cWardenEnv* e) {
    int32_t len = (int32_t)esi[2] - 0xd;
    e->ord11105();
    if (e->ord10197(len, 0x40)) { e->fn85cf0(esi + 0xd, len); return; }
    if (e->ord10197(len, 0x80)) e->fn84300(esi);
}

// 1.13c D2Client+0x87160
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87160(int32_t eax, int32_t esi, const uint8_t* ebx, void* src, const D2cWardenEnv* e) {
    uint8_t b408[0x400], b414[12], b418[4], b41c[4];
    e->ord10248();
    if (esi) e->ord10249(ebx + esi, eax - esi);
    e->ord10249(nullptr, 0);
    e->ord501(b408, src, 0x400);
    e->ord510(b414);
    int v = e->ord506(b418);
    e->ord10249(b41c, v);
    e->ord10250();
}

// 1.13c D2Client+0x87230 -- hex-format 16 bytes, copy 5-byte tag, ask the Warden ctx slot +0xD0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87230(const uint8_t* key16, D2cWarden* w, const D2cWardenEnv* e,
                                                     void (*slot_d0)(void*, void*, int32_t, int32_t), void* ret) {
    char hex[33]; uint8_t tag[8];
    for (int i = 0; i < 16; i++) snprintf(hex + i * 2, 3, "%02x", key16[i]);
    e->ord501(tag, w->sig5, 5);
    if (w->ctx) { uint8_t ft[8] = {0}; slot_d0(ret, ft, -1, -1); }  // GetSystemTimeAsFileTime -> slot
}

// 1.13c D2Client+0x872D0 -- header check "SIGN" (0x5349474E), ends via sigcheck
extern "C" EMSCRIPTEN_KEEPALIVE bool d2_client_872d0(uint32_t param1, uint32_t eax, int32_t edi, const D2cWardenEnv* e) {
    if (eax < param1) { e->fatal(); return false; }
    if (edi + 4U <= eax) {
        uint32_t magic = 0;
        e->ord491(&magic);
        if (magic == 0x5349474e) return e->sigcheck() == 0;
    }
    return false;
}

static void d2c_drop(D2cWardenNode*& n, const D2cWardenEnv* e) {
    if (n) { e->fn4e630(n); e->fn1a00(n); e->free_(n); }
}

// 1.13c D2Client+0x87480
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87480(D2cWarden* w, const D2cWardenEnv* e) {
    if (w->cur) {
        e->release(w->cur);
        d2c_drop(w->cur, e);
    }
    w->cur = nullptr;
}

// 1.13c D2Client+0x874E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_874e0(D2cWarden* w, const D2cWardenEnv* e) {
    if (w->pend) { d2c_drop(w->pend, e); w->pend = nullptr; }
    d2_client_87480(w, e);
    w->seed = 0xffffffffu;
    w->ctx = nullptr;
    if (w->blob) e->free_(w->blob);
    w->blob = nullptr;
    w->blobLen = 0;
}

// 1.13c D2Client+0x87560
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87560(D2cWarden* w, uint32_t param1, const D2cWardenEnv* e) {
    if (w->pend) { d2c_drop(w->pend, e); w->pend = nullptr; }
    uint32_t* buf = (uint32_t*)(intptr_t)e->ord401((int32_t)param1);
    if (!buf) return 0;
    e->fn778c0(buf, 0x10);
    e->fn1920(buf, (int32_t)param1);
    if (!e->sigcheck()) { e->free_(buf); return 0; }           // FUN_6fb372d0(0) false
    void* blob = (void*)(intptr_t)e->ord401((int32_t)buf[0]);
    if (!blob) { e->free_(buf); return 0; }
    if (e->fn78220((int32_t)param1 - 4, blob)) { e->free_(buf); e->free_(blob); return 0; }
    e->free_(buf);
    void* mem = (void*)(intptr_t)e->ord401(0x14);
    D2cWardenNode* n = mem ? e->fn19b0(mem) : nullptr;
    if (!n) { e->free_(blob); return 0; }
    if (!e->fn4e690(blob, n)) { e->free_(blob); e->fn4e630(n); e->free_(n); return 0; }
    // entry = section table lookup (iVar5 chain), +0x10 slot of n
    int32_t entry = n->entry ? 1 : 0;
    if (!entry) { e->fn4e630(n); e->free_(blob); e->fn4e630(n); e->free_(n); return 0; }
    e->free_(blob);
    w->pend = n;
    return 1;
}

// 1.13c D2Client+0x87840
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87840(D2cWarden* w, const D2cWardenEnv* e) {
    while (w->pend) {
        if (w->cur) { e->release(w->cur); d2c_drop(w->cur, e); }
        D2cWardenNode* n = w->pend;
        w->cur = n;
        w->pend = nullptr;
        uint32_t seed = w->seed;
        e->call_entry(n);                 // returns new iface into n+0xC
        e->send4(n, &seed, 4);
    }
}

// 1.13c D2Client+0x87940
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87940(D2cWarden* w, const uint8_t* key16, const D2cWardenEnv* e,
                                                         uint32_t (*slot_cc)(void*, void*), void (*slot_c8)(void*)) {
    if (e->sigcheck() == 0) return d2_client_87560(w, 0x12b2, e);
    char hex[33]; uint8_t tag[8];
    for (int i = 0; i < 16; i++) snprintf(hex + i * 2, 3, "%02x", key16[i]);
    e->ord501(tag, w->sig5, 5);
    if (w->ctx && slot_cc(hex, tag)) {
        uint32_t r = d2_client_87560(w, (uint32_t)(uintptr_t)tag, e);
        slot_c8(w->ctx);
        return r;
    }
    return 0;
}

static bool d2c_scan(const D2cQNode* n, int32_t lo, int32_t hi) {   // exclusive bounds
    for (; n; n = n->next) {
        const uint8_t* u = (const uint8_t*)n->unit;
        int32_t id; memcpy(&id, u + 4, 4);
        int32_t st; memcpy(&st, u + 0x30, 4);
        if (st != 2 && id > lo && id < hi) return true;
    }
    return false;
}
// 1.13c D2Client+0x87AF0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87af0(const D2cQNode* head) { return d2c_scan(head, 0x1230, 0x125b); }
// 1.13c D2Client+0x87B30
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_87b30(const D2cQNode* head) { return d2c_scan(head, 0xb75, 0x1231); }
