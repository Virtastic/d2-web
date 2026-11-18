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
    void (*dec11105)(const uint8_t* data, int32_t len, uint32_t zero, uint32_t* hdr);   // thunk 0xCA90 (0x869E0)
    int  (*flag10197)(uint32_t v, uint32_t mask);                                      // thunk 0xCA3C
    void (*call85cf0)(const uint8_t* pkt, uint32_t* hdr);                              // EAX = pkt, stack = hdr
    void (*call84300)(uint32_t* hdr, const uint8_t* pkt);                              // EAX = hdr, stack = pkt
    void (*hinit)(void* ctx);                                                          // thunk 0xBFDA (ECX = ctx)
    void (*hupd)(void* ctx, const void* p, int32_t n);                                 // thunk 0xBFD4 (ECX = ctx, stdcall 2)
    void (*hfin)(void* ctx, void* out);                                                // thunk 0xBFCE (ECX = ctx, stdcall 1)
};

// 1.13c D2Client+0x869B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_869b0(const uint8_t* esi, const D2cWardenEnv* e) {
    e->ord11105();
    e->fn85b30(esi + 0xd, (int32_t)esi[2] - 0xd);
}

// 1.13c D2Client+0x869E0: Ordinal 11105 (stdcall 4: pkt+0xD, len, 0, &hdr) fills a 24-byte header; Ordinal 10197 tests its dword 3
// (hdr[3] & mask): 0x40 runs 0x85CF0 (EAX = pkt, stack = &hdr), else 0x80 runs 0x84300 (EAX = &hdr, stack = pkt).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_869e0(const uint8_t* esi, const D2cWardenEnv* e) {
    uint32_t hdr[6] = {0, 0, 0, 0, 0, 0};
    e->dec11105(esi + 0xd, (int32_t)esi[2] - 0xd, 0, hdr);
    if (e->flag10197(hdr[3], 0x40)) { e->call85cf0(esi, hdr); return; }
    if (e->flag10197(hdr[3], 0x80)) e->call84300(hdr, esi);
}

// 1.13c D2Client+0x87160 (stdcall 2: src, out; EAX = total length, ESI = split, EBX = data): hash context on the stack (thiscall ECX = ctx):
// init; ESI != 0 feeds data[esi..eax) then data[0..esi), else data[0..eax); then the 0x400-byte string at src is copied (501), normalised (510),
// its length taken (506) and fed too; the digest goes to out.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_87160(int32_t eax, int32_t esi, const uint8_t* ebx, void* src, void* out, const D2cWardenEnv* e) {
    uint8_t ctx[0x5c], buf[0x400];
    e->hinit(ctx);
    if (esi) { e->hupd(ctx, ebx + esi, eax - esi); e->hupd(ctx, ebx, esi); }
    else e->hupd(ctx, ebx, eax);
    e->ord501(buf, src, 0x400);
    e->ord510(buf);
    int v = e->ord506(buf);
    e->hupd(ctx, buf, v);
    e->hfin(ctx, out);
}

// 1.13c D2Client+0x87230: ported in d2client_87230.cpp


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

// 1.13c D2Client+0x87840: moved to d2client_87840.cpp (the vtable and entry calls keyed by the pointers retail reads).

// 1.13c D2Client+0x87940: moved to d2client_87940.cpp (the ctx+0xCC/+0xC8 calls and the 0x87560 registers retail passes).

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
