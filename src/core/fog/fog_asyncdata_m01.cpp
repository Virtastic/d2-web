// src/core/fog/fog_asyncdata_m01.cpp -- 1.13c Fog.dll AsyncData.cpp wing (+0x1CD50 +0x1D7F0 +0x1DB40
// +0x1DCC0 +0x1DF00 +0x1E010) and the CRT-adjacent +0x212C0 / +0x21AE0, from
// build-wasm/ghidra-fog/decomp/0x<RVA>.c. Callees outside this file (pool alloc/free +0x1B560/+0x1B0C0/+0x1CCC0,
// the Storm async-file ordinals 253/265/267/271/282-285/289, event slots +0x1D870, completion +0x1D740, poll
// +0x1DC40, FogError +0x10F30) are seams on FogAsyncEnv: the host (wasm: StormLib + synchronous reads) binds them.
// Win32 events are host no-ops in wasm; the sequencing and the error lines are retail's.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

namespace {
constexpr uint32_t kMagic = 0x41737960u, kMagicMask = 0xfffffff0u;
constexpr const char* kAsyncFile = "..\\Source\\Fog\\Src\\AsyncData.cpp";
}

struct FogAsyncSlot { void* event; uint32_t busy; void* req; };  // 12-byte retail slot (req = +8, set by 1DCC0)

struct FogAsyncReq {          // 0x54 bytes in retail; offsets in comments
    void* file;               // +0x00 Storm handle
    void* pool;               // +0x04
    void* ioctx;              // +0x08 (&r[2], async read context)
    void* r3;                 // +0x0C
    uint32_t seek;            // +0x10 file offset
    void* r5;                 // +0x14
    void* event;              // +0x18 slot event
    uint32_t cb[7];           // +0x1C callback block
    FogAsyncSlot* slot;       // +0x38
    void* buf;                // +0x3C
    int32_t size;             // +0x40
    int32_t priority;         // +0x44 (userdata at creation, then IO priority)
    uint32_t flags;           // +0x48 magic | 2 owns buf | 8 deferred
    int32_t savedPriority;    // +0x4C
    FogAsyncReq* next;        // +0x50 deferred list
};

struct FogAsyncEnv {
    // globals
    FogAsyncSlot* table = nullptr;   // DAT 9af70
    uint32_t tableCount = 0;         // DAT 9af6c
    uint32_t enabled = 0;            // DAT 9af68
    FogAsyncReq* deferredHead = nullptr;  // DAT 9af64
    int32_t deferCount = 0;          // DAT 9af60
    uint32_t poolEnabled = 1;        // DAT 7f350
    // seams
    void* (*poolAlloc)(void* pool);                    // 1B560 (pool 0 = global)
    void (*poolFree)(void* pool, void* p);             // 1B0C0 / 1CCC0
    void (*report)(int type, const char* file, uint32_t line);  // 10F30
    void* (*eventCreate)();                            // CreateEventA(manual, unsignaled)
    void (*eventClose)(void* e);
    void (*eventWait)(void* e);                        // WaitForSingleObject INFINITE
    int (*stormOpen)(const char* name, void** out);    // Ordinal 267
    int (*stormSize)(void* f);                         // 265
    int (*stormSeek)(void* f, uint32_t off);           // 271 (-1 = fail)
    void (*stormClose)(void* f);                       // 253
    void (*stormSetPriority)(void* buf, int32_t p);    // 282
    void (*stormCancel)(void* buf);                    // 283
    void (*stormBudget)(uint32_t which, uint32_t n);   // 284 / 285
    void (*stormRead)(void* f, void* buf, int32_t n, FogAsyncReq* r, void* ud, uint32_t* cb);  // 289
    FogAsyncSlot* (*slotAcquire)();                    // 1D870
    void (*complete)(FogAsyncReq* r);                  // 1D740
    uint32_t (*poll)(FogAsyncReq* r);                  // 1DC40
};

namespace {
[[noreturn]] void fatal(FogAsyncEnv& e, uint32_t line) {  // FogError(6,"",line,"Unrecoverable internal error %08x"); abort; _exit(-1)
    if (e.report) e.report(6, "", line);
    std::fprintf(stderr, "Fog unrecoverable internal error line %u\n", line);
    std::abort();
}
}

// 1.13c Fog+0x1CD50
extern "C" void* fog_sub_1cd50(FogAsyncEnv& e, void* self, const char* file, uint32_t line) {
    void* p = e.poolAlloc(e.poolEnabled ? self : nullptr);
    if (!p) e.report(2, file, line);
    return p;
}

// 1.13c Fog+0x1D7F0
extern "C" void fog_sub_1d7f0(FogAsyncEnv& e) {
    if (e.table) {
        for (uint32_t i = 0; i < e.tableCount; ++i)
            if (e.table[i].event) { e.eventClose(e.table[i].event); e.table[i].event = nullptr; }
        e.poolFree(nullptr, e.table);
        e.table = nullptr;
        e.tableCount = 0;
    }
    e.enabled = 0;
}

// 1.13c Fog+0x1DB40
extern "C" void fog_sub_1db40(FogAsyncEnv& e, FogAsyncReq* r, int32_t prio) {
    if (!r) fatal(e, 0x1df);
    if ((r->flags & kMagicMask) != kMagic) fatal(e, 0x1e0);
    if (prio > 0) fatal(e, 0x1e1);
    if (e.deferCount < 1) {
        if (prio != r->priority) { r->priority = prio; e.stormSetPriority(r->buf, prio); }
    } else if (!(r->flags & 8)) {
        r->savedPriority = r->priority;
        r->priority = prio;
        r->flags |= 8;
        r->next = e.deferredHead;
        e.deferredHead = r;
    } else if (r->priority < prio) {
        r->priority = prio;
    }
}

// 1.13c Fog+0x1DCC0 (fastcall: pool=ECX, name=EDX; the stack tail carries the async flag (EBX) and the file offset)
extern "C" FogAsyncReq* fog_sub_1dcc0(FogAsyncEnv& e, void* pool, const char* name, int32_t size, void* buf,
                                      const uint32_t* callbacks, void* userdata, bool async, uint32_t offset) {
    auto* r = static_cast<FogAsyncReq*>(e.poolAlloc(e.poolEnabled ? pool : nullptr));
    if (!r) e.report(2, kAsyncFile, 0x12d);
    std::memset(r, 0, sizeof *r);
    r->flags = kMagic;
    if (!e.stormOpen(name, &r->file)) { e.poolFree(nullptr, r); return nullptr; }
    r->pool = pool;
    r->priority = static_cast<int32_t>(reinterpret_cast<intptr_t>(userdata));
    if (offset && e.stormSeek(r->file, offset) == -1) fatal(e, 0x147);
    if (size < 1) size = e.stormSize(r->file);
    r->size = size;
    if (!buf) { r->buf = e.poolAlloc(pool); r->flags |= 2; }
    else { r->buf = buf; r->flags &= ~2u; }
    uint32_t* cb = nullptr;
    if (callbacks) { std::memcpy(r->cb, callbacks, sizeof r->cb); cb = r->cb; }
    if (async && e.enabled) {
        r->slot = e.slotAcquire();
        if (r->slot) {
            r->slot->req = r;
            r->event = r->slot->event;
            r->seek = offset;
            e.stormRead(r->file, r->buf, r->size, r, userdata, cb);
            return r;
        }
        if (r->file) { e.stormClose(r->file); r->file = nullptr; }
        if (r->flags & 2) e.poolFree(pool, r->buf);
        e.poolFree(pool, r);
        return nullptr;
    }
    e.stormRead(r->file, r->buf, r->size, nullptr, userdata, nullptr);
    if (cb) {  // retail: (*cb[0])(buf, size, cb)
        using Fn = void (*)(void*, int32_t, uint32_t*);
        if (cb[0]) reinterpret_cast<Fn>(static_cast<uintptr_t>(cb[0]))(r->buf, r->size, cb);
    }
    e.complete(r);
    return r;
}

// 1.13c Fog+0x1DF00
extern "C" void fog_sub_1df00(FogAsyncEnv& e, int enable) {
    if (e.enabled) fatal(e, 0x7b);
    e.stormBudget(284, 0x40000);
    e.stormBudget(285, 0x4000);
    if (!enable) return;
    e.table = static_cast<FogAsyncSlot*>(e.poolAlloc(nullptr));
    if (!e.table) { e.report(2, kAsyncFile, 0x88); fatal(e, 0x89); }
    for (int i = 0; i < 0x100; ++i) { e.table[i].event = e.eventCreate(); e.table[i].busy = 1; }
    e.tableCount = 0x100;
    e.enabled = 1;
}

// 1.13c Fog+0x1E010
extern "C" void fog_sub_1e010(FogAsyncEnv& e, FogAsyncReq* r) {
    if (!r) fatal(e, 0x295);
    if ((r->flags & kMagicMask) != kMagic) fatal(e, 0x296);
    if (r->flags & 8) {
        for (FogAsyncReq** pp = &e.deferredHead; *pp; pp = &(*pp)->next)
            if (*pp == r) { *pp = r->next; break; }
    }
    if (!e.poll(r)) {
        e.stormCancel(r->buf);
        e.eventWait(r->slot->event);
        e.complete(r);
    }
    if (r->flags & 2) {
        if (!r->buf) fatal(e, 0x2c8);
        e.poolFree(nullptr, r->buf);
        r->buf = nullptr;
        r->flags &= 0xf;
    }
    e.poolFree(nullptr, r);
}

// 1.13c Fog+0x212C0 -- CRT-adjacent crossfade filler (hidden-arg recovery from the disassembly):
// out[i] for i = 0..count-1 = trunc(0.01 * (pow(i/(count-1), 100/n) * max * (100-pct) + min(i/(count-1) * max * n * 0.01, max) * pct)),
// stored as 1, 2 or 4 byte elements. n = ECX (>=0), out = EDX, pct (0..100) double; elem size 1/2/4; count; max.
// ponytail: doubles (retail is x87); n == 0 follows IEEE (inf) where retail routes through the CRT error hook 0x21BF7.
extern "C" void fog_sub_212c0(int32_t n, void* out, double pct, int32_t elemSize, int32_t count, int32_t maxv) {
    auto die = [](uint32_t line) { std::fprintf(stderr, "Fog unrecoverable internal error line %u\n", line); std::abort(); };
    if (elemSize != 1 && elemSize != 2 && elemSize != 4) die(0xe);
    if (n < 0) die(0xf);
    if (!(pct >= 0.0 && pct <= 100.0)) die(0x10);
    const double a = 100.0 - pct, s = n * 0.01, b = 100.0 / n, lim = count - 1.0, m = maxv, d = 1.0 / lim;
    if (!(0.0 <= lim)) return;
    auto* p = static_cast<uint8_t*>(out);
    for (double t = 0.0; t <= lim; t += 1.0, p += elemSize) {
        double x = d * t * m * s;
        if (!(x < m) && x == x) x = m;  // retail keeps x on x<m or unordered
        const int32_t v = static_cast<int32_t>((std::pow(d * t, b) * m * a + x * pct) * 0.01);
        if (elemSize == 1) *p = static_cast<uint8_t>(v);
        else if (elemSize == 2) { const uint16_t w = static_cast<uint16_t>(v); std::memcpy(p, &w, 2); }
        else std::memcpy(p, &v, 4);
    }
}

// 1.13c Fog+0x21AE0 -- CRT x87 operand classifier (cdecl): operand 1 = (lo, hi, se), operand 2 = (hi2 = param_5, se2 = param_6).
// `scale` is retail's FMUL by the long double at Fog+0x30018 (renormalise), `table` the 16-byte class table at Fog+0x30000.
extern "C" uint32_t fog_sub_21ae0(uint32_t lo, uint32_t hi, uint16_t se, uint32_t hi2, uint32_t se2,
                                  void (*scale)(uint32_t* lo, uint32_t* hi, uint16_t* se), const uint8_t* table) {
    while (!(hi & 0x80000000u)) {
        if (lo == 0 && hi == 0) return 0;
        if (se & 0x7fff) return se & 0x7fff;
        uint32_t e2 = se2 & 0x7fff, r;
        if (e2 == 0) { if (hi2 & 0x80000000u) return hi2 * 2; }
        else if (e2 == 0x7fff || (r = hi2 * 2, !(hi2 & 0x80000000u))) return e2 == 0x7fff ? e2 : r;
        scale(&lo, &hi, &se);
    }
    uint32_t u = (hi * 2) ^ 0xe000000u;
    if (u & 0xe000000u) return u;
    uint32_t idx = (hi & 0x7fffffffu) >> 27;
    if (table[idx]) {
        if ((se & 0x7fff) != 0 && (se & 0x7fff) != 0x7fff) return (se2 & 0x7fff) != 1 ? (se2 & 0x7fff) : 1;
        return se & 0x7fff;
    }
    return idx;
}
