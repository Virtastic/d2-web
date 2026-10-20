// src/core/d2client/d2client_batch_6ec30.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows from the cached
// decompiles. Alpha batch lane: implementation + marker only, no spec/vector. Callees are function
// pointers on Env (address = retail RVA); globals are fields on Env too; register args are explicit.
// Packet/struct args are read at their retail byte offsets (packed, unaligned).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

template <class T> static inline T rd(uintptr_t p, uint32_t o) { T v; memcpy(&v, (const void*)(p + o), sizeof v); return v; }

struct CmdEntry { uint32_t name; uint16_t kind; uint32_t pad; uint32_t flag; } __attribute__((packed)); // 0xE
static_assert(sizeof(CmdEntry) == 14, "CmdEntry");

struct D2ClientBatch6EC30Env {
    // globals
    CmdEntry* cmdTable;      uint32_t cmdCount;   // 0xB285C table, (0xB2885-0xB285C)/14+1 entries
    uint32_t  cmdAllowAll;   // 0xBC394
    uint32_t  curUnit;       // 0xBBBFC DAT_6fbcbbfc
    uint32_t  flagAAE18;     // 0xAAE18
    uintptr_t unitTable;     // 0xBA608 (stride 0x80)
    uintptr_t dataTables;    // *sgptDataTables
    uint32_t  bitMask8;      // gdwBitMasks[8]
    // callees
    int32_t   (*strlen_)(const char*);                                   // Ordinal_506
    int32_t   (*cmp)(const char*, const char*, int32_t);                 // Ordinal_509 (arg order guessed)
    void      (*fmt)(const char*, char*, int32_t, const char*);          // Ordinal_504 (args guessed)
    void      (*cat)(uint32_t, const char*, int32_t);                    // Ordinal_501
    uintptr_t (*lookup)(uint32_t id, uintptr_t slot, uint32_t idx);      // 0xA4E20
    void      (*fn51B10)(uintptr_t, int32_t, int32_t, int32_t, uint8_t); // 0xA1B10
    void      (*fn52950)(uintptr_t, int32_t, int32_t, int32_t);          // 0xA2950
    void      (*fn79030)(uint32_t);                                      // 0xC9030
    void      (*fn2A1B0)(uint32_t, uint32_t, uint32_t);                  // 0x2A1B0
    void      (*fn2A750)(uint32_t, uintptr_t, const uint32_t*);          // 0x2A750
    void      (*fnC240)(void);                                           // 0xAC240
    void      (*set)(uintptr_t, uint32_t, uint32_t, uint32_t);           // Ordinal_10887
    void      (*fn2A6C0)(void);                                          // 0x2A6C0
    void      (*fn6210)(void);                                           // 0xA6210
    uintptr_t (*fn630F0)(uint32_t*);                                     // 0xB30F0
    void      (*s10069)(uintptr_t, uint8_t, uint32_t);                   // Ordinal_10069
    void      (*s10970)(uintptr_t, uint16_t);                            // Ordinal_10970
    uint32_t  (*fn1380)(uintptr_t);                                      // 0x1380
    void      (*fn50250)(void);                                          // 0xA0250
    void      (*fn4BC80)(void);                                          // 0x4BC80
    uint32_t  (*s10047)(void);                                           // Ordinal_10047
};

static uintptr_t slot(D2ClientBatch6EC30Env* e, uintptr_t pkt, uint32_t idOff, uint32_t tagOff) {
    uint8_t t = rd<uint8_t>(pkt, tagOff);
    return e->lookup(rd<uint32_t>(pkt, idOff), e->unitTable + (uint32_t)t * 0x80, t);
}

// 1.13c D2Client+0x6EC30 -- command-prefix match; guesses: cmp(line,name,len)==0 means equal.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2c_6ec30(D2ClientBatch6EC30Env* e, char** pLine, uint8_t* outFlag,
        const char* buf, int32_t* adv, char* tmp, uint32_t dst, const char* fmtArg) {
    *outFlag = 0;
    char local[0x100];
    strncpy(local, *pLine, 0x100);
    int32_t len = e->strlen_(local);
    for (uint32_t i = 0; i < e->cmdCount; i++) {
        CmdEntry* c = &e->cmdTable[i];
        int32_t n = e->strlen_((const char*)(uintptr_t)c->name);
        if (!c->flag && !e->cmdAllowAll) continue;
        if (c->kind == 3) {
            if (n <= len && e->cmp(buf, (const char*)(uintptr_t)c->name, n) == 0) { *(uint8_t*)pLine = (uint8_t)c->kind; return 1; }
        } else if (n < len && e->cmp(buf, (const char*)(uintptr_t)c->name, n) == 0 && buf[n]) {
            if (!c->pad) { *adv += n; *(uint8_t*)pLine = (uint8_t)c->kind; return 1; }
            e->fmt(fmtArg, tmp, 0x100, 0);
            if (strlen(tmp) && strlen(tmp) < 0x32) {
                e->cat(dst, tmp, 0x7fffffff);
                *adv += (int32_t)strlen(tmp) + n;
                *(uint8_t*)pLine = (uint8_t)c->kind; return 1;
            }
            return 0;
        }
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
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af980(D2ClientBatch6EC30Env* e) { e->fn50250(); }

// 1.13c D2Client+0xAF9B0
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_af9b0(D2ClientBatch6EC30Env* e) { e->fn4BC80(); }

// 1.13c D2Client+0xAFC10
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_afc10(D2ClientBatch6EC30Env* e) {
    if (!e->flagAAE18) return 0;
    e->flagAAE18 = 0;
    return e->s10047();
}
