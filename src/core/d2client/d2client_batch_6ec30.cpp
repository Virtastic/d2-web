// src/core/d2client/d2client_batch_6ec30.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows from the cached
// decompiles. Alpha batch lane: implementation + marker only, no spec/vector. Callees are function
// pointers on Env (address = retail RVA); globals are fields on Env too; register args are explicit.
// Packet/struct args are read at their retail byte offsets (packed, unaligned).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

template <class T> static inline T rd(uintptr_t p, uint32_t o) { T v; memcpy(&v, (const void*)(p + o), sizeof v); return v; }

#include "d2client_batch_6ec30.h"

static uintptr_t slot(D2ClientBatch6EC30Env* e, uintptr_t pkt, uint32_t idOff, uint32_t tagOff) {
    uint8_t t = rd<uint8_t>(pkt, tagOff);
    return e->lookup(rd<uint32_t>(pkt, idOff), e->unitTable + (uint32_t)t * 0x80, t);
}

// 1.13c D2Client+0x6EC30 -- chat command-prefix match. stdcall(char** pLine, uint8_t* outKind, char* dst): copies *pLine (0x100) to a local, walks the
// 3 command entries (0xE bytes: +0 name ptr, +4 short kind, +6 dword "has argument" flag, +0xA dword allow flag; all gated by the allow-all global),
// advances *pLine past the matched name (+ the formatted argument text appended to dst), writes the kind byte. Callees (0x3AC0 strncpy, 0xBE90 strlen,
// 0xBEFC prefix compare, 0xBF08 formatter, 0xBE96 append) and the command table are seams; fmtStr is the retail format-string address passed to 0xBF08.
struct D2c6EC30Env {
    char* (*strncpy_)(char*, const char*, int32_t); int32_t (*strlen_)(const char*); int32_t (*cmp)(const char*, const char*, int32_t);
    void (*fmt)(char**, char*, int32_t, uint32_t, int32_t); void (*cat)(char*, const char*, int32_t);
    const uint8_t* table; int32_t allowAll; uint32_t fmtStr;
};
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2c_6ec30(const D2c6EC30Env* e, char** pLine, uint8_t* outKind, char* dst) {
    *dst = 0;
    char local[0x100];
    e->strncpy_(local, *pLine, 0x100);
    int32_t len = e->strlen_(local);
    for (int i = 0; i < 3; i++) {
        const uint8_t* c = e->table + 0xe * i;
        const char* name = (const char*)(uintptr_t)rd<uint32_t>((uintptr_t)c, 0);
        int32_t n = e->strlen_(name);
        if (rd<uint32_t>((uintptr_t)c, 0xa) == 0 && e->allowAll == 0) continue;
        uint8_t kind = c[4];
        if (rd<uint16_t>((uintptr_t)c, 4) == 3) {
            if (len < n) continue;
            if (e->cmp(local, name, n) != 0) continue;
            *outKind = kind; return 1;
        }
        if (len <= n) continue;
        if (e->cmp(local, name, n) != 0) continue;
        if (local[n] == 0) continue;
        char* p = local + n;
        char tmp[0x100];
        if (rd<uint32_t>((uintptr_t)c, 6) == 0) { *pLine += n; *outKind = kind; return 1; }
        e->fmt(&p, tmp, 0x100, e->fmtStr, 0);
        if (strlen(p) == 0) return 0;
        if (strlen(tmp) >= 0x32) return 0;
        e->cat(dst, tmp, 0x7fffffff);
        *pLine += (int32_t)strlen(tmp) + n + 1;
        *outKind = kind; return 1;
    }
    return 0;
}

// 1.13c D2Client+0xAECA0 (guess: register `local_f` is an uninitialised-stack artefact; passed in)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_aeca0(D2ClientBatch6EC30Env* e, uint16_t v) {
    e->fnC240();
    uint32_t u = e->curUnit;
    if (!u) return;
    e->set(u, 6, (uint32_t)v << 8, 0);
    e->set(u, 8, 0, 0);
    e->set(u, 10, 0, 0);
    e->set(u, 0x4a, 0, 0);
    e->set(u, 0x1a, u & 0xff, 0);
    e->fn2A1B0(0x4a00, 0, u >> 8 & 0xffff);
    if (v && rd<uint32_t>(u, 0x10) == 0x11) { e->fn2A6C0(); e->fn6210(); }
}

// 1.13c D2Client+0xAEEE0 (ecx = packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_aeee0(D2ClientBatch6EC30Env* e, uintptr_t p) {
    uint32_t a = rd<uint32_t>(p, 0xf), b = rd<uint32_t>(p, 0x13), c = rd<uint32_t>(p, 0x1a);
    uint32_t l[0x17] = {0};
    l[1] = e->curUnit;
    l[4] = rd<uint16_t>(p, 5);
    l[5] = rd<uint32_t>(p, 7);
    l[6] = rd<uint32_t>(p, 0xb);
    l[0xc] = rd<uint8_t>(p, 0x1e);            // local_2c
    l[0] = 0x40000001;
    if (a && b) { l[0] = 0x40000021; l[7] = a; l[8] = b; }
    uintptr_t u = e->fn630F0(l);
    if (!u) return;
    e->s10069(u, rd<uint8_t>(p, 0x19), c);
    if (rd<uint8_t>(p, 0x1f)) e->set(u, 0x148, rd<uint8_t>(p, 0x1f), 0);
    e->s10970(u, 0);
    *(uint32_t*)(u + 0x44) = e->fn1380(u + 0x20) << 8;
}

// 1.13c D2Client+0xAEFB0 (ecx = packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_aefb0(D2ClientBatch6EC30Env* e, uintptr_t p) {
    if (!slot(e, p, 7, 6)) return;
    uintptr_t u = slot(e, p, 0xc, 0xb);
    int32_t i = rd<int16_t>(p, 2);
    if (i < 0 || i >= (int32_t)rd<uint32_t>(e->dataTables, 0xba0)) return;
    uintptr_t rec = rd<uint32_t>(e->dataTables, 0xb98) + (uint32_t)i * 0x23c;
    if (!rec) return;
    if (e->bitMask8 & rd<uint8_t>(rec, 4)) { e->fn52950(u, i, rd<int16_t>(p, 4), rd<int32_t>(p, 0x10)); return; }
    e->fn51B10(u, i, rd<int16_t>(p, 4), rd<int32_t>(p, 0x10), (uint8_t)rd<uint32_t>(p, 0x14));
}

// 1.13c D2Client+0xAF090 (ecx = packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af090(D2ClientBatch6EC30Env* e, uintptr_t p) {
    if (!slot(e, p, 2, 1)) return;
    if (rd<int16_t>(p, 0xb) && rd<int16_t>(p, 0xd)) e->fn79030(rd<uint16_t>(p, 6));
}

// 1.13c D2Client+0xAF0F0 (ecx = packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af0f0(D2ClientBatch6EC30Env* e, uintptr_t p) {
    if (!slot(e, p, 2, 1)) return;
    if (slot(e, p, 10, 9)) e->fn79030(rd<uint16_t>(p, 6));
}

// 1.13c D2Client+0xAF4C0 (ecx = a, edx = packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af4c0(D2ClientBatch6EC30Env* e, uintptr_t a, uintptr_t p) {
    uint32_t l[2] = { rd<uint8_t>(p, 7), rd<uint32_t>(p, 8) };
    e->fn2A1B0(rd<uint16_t>(p, 0xe), 0, 0);
    e->fn2A750(rd<uint8_t>(p, 6), a, l);
}

// 1.13c D2Client+0xAF5F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af5f0(D2ClientBatch6EC30Env* e, uintptr_t a, uintptr_t p) {
    uint32_t l[7] = { rd<uint8_t>(p, 7), rd<uint8_t>(p, 8), 0, 0, 0, 0, 0 };
    e->fn2A750(rd<uint8_t>(p, 6), a, l);
}

// 1.13c D2Client+0xAF750
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af750(D2ClientBatch6EC30Env* e, uintptr_t a, uintptr_t p) {
    e->fn2A1B0(rd<uint16_t>(p, 0xe), 0, 0);
    uint32_t l[6] = { rd<uint8_t>(p, 6), rd<uint32_t>(p, 7), rd<uint8_t>(p, 0xb), 2, 0, 4 };
    e->fn2A750(rd<uint8_t>(p, 5), a, l);
}

// 1.13c D2Client+0xAF980
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af980(D2ClientBatch6EC30Env* e, uintptr_t p) { uint8_t l[8] = {0}; memcpy(l, (const void*)p, 7); e->fn50250(l); }   // ECX = packet, 7 bytes to a local, ESI = &local

// 1.13c D2Client+0xAF9B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af9b0(D2ClientBatch6EC30Env* e, uintptr_t p) { uint8_t l[16] = {0}; memcpy(l, (const void*)p, 15); e->fn4BC80(l); }   // ECX = packet, 15 bytes to a local, ESI = &local

// 1.13c D2Client+0xAFC10
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_afc10(D2ClientBatch6EC30Env* e) {
    if (!e->flagAAE18) return 0;
    e->flagAAE18 = 0;
    return e->s10047();
}
