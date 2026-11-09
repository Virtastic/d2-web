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

// 1.13c D2Client+0xAF5F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af5f0(D2ClientBatch6EC30Env* e, uintptr_t a, uintptr_t p) {
    uint32_t l[7] = { rd<uint8_t>(p, 7), rd<uint8_t>(p, 8), 0, 0, 0, 0, 0 };
    e->fn2A750(rd<uint8_t>(p, 6), a, l);
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
