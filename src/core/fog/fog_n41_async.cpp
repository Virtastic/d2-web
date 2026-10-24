// src/core/fog/fog_n41_async.cpp -- 1.13c Fog.dll alpha-lane rows (fhandle unlock stubs, AsyncData,
// the bit-packer and its statistics, the d2exp.mpq probe). Reimplemented from the decompiles in
// build-wasm/ghidra-fog/decomp/0x<RVA>.c. Base 0x6FF50000. Module globals are passed as state structs,
// module-internal and Win32 callees as seams on an Api struct.
#include <cstdint>
#include <cstdlib>

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t u8;

// Shared fatal path: capture ip, WSOCK32 ordinal 116, log at level 6, abort, _exit(-1).
struct FogN41Fatal {
    u32 (*capture_ip)(void);                                                         // Fog+0xFC20
    void (*ordinal116)(void);
    void (*fatal)(int level, const char* file, int line, const char* fmt, ...);      // Fog+0x10F30
    void (*abort)(void);                                                             // Fog+0x184A0
    void (*exit_process)(int code);                                                  // _exit
};
static void fog_n41_die(const FogN41Fatal* f, int line) {
    const u32 ip = f->capture_ip();
    f->ordinal116();
    f->fatal(6, "", line, "Unrecoverable internal error %08x", ip);
    f->abort();
    f->exit_process(-1);
}

// --- __unlock_fhandle tail stubs ----------------------------------------------------------------
struct FogUnlockApi { void (*unlockFhandle)(int fh); };   // Fog+0xA33A

// 1.13c Fog+0x6F46
void fog_sub_6f46(const FogUnlockApi* a, int fh) { a->unlockFhandle(fh); }
// 1.13c Fog+0x725F
void fog_sub_725f(const FogUnlockApi* a, int fh) { a->unlockFhandle(fh); }
// 1.13c Fog+0x7731
void fog_sub_7731(const FogUnlockApi* a, int fh) { a->unlockFhandle(fh); }
// 1.13c Fog+0x78E2
void fog_sub_78e2(const FogUnlockApi* a, int fh) { a->unlockFhandle(fh); }

// --- AsyncData event table: 256 entries of 12 bytes (HANDLE, 1, pad) at DAT_6ff9af70 -----------
struct FogAsyncState {
    u8* table;        // DAT_6ff9af70
    u32 count;        // DAT_6ff9af6c
    u32 initialized;  // DAT_6ff9af68
    int dataBase;     // DAT_6ff9af60
    u8* pending;      // DAT_6ff9af64 (list head)
};
struct FogAsyncApi {
    void* (*alloc)(void* ctx);                                                       // Fog+0x1B560
    void (*free_)(void* p);                                                          // Fog+0x1B0C0
    void (*closeHandle)(u32 h);
    u32 (*createEvent)(void);                                                        // CreateEventA(0,1,0,0)
    void (*setLimit)(u32 v);                                                         // Ordinal_284 / 285 / 282 seam
    void (*setHandleLimit)(u32 handle, int v);                                       // Ordinal_282
    FogN41Fatal die;
    void (*logAlloc)(int level, const char* file, int line, const char* fmt);        // Fog+0x10F30
};
static u32 fa_ld(const u8* p) { return *(const u32*)p; }
static void fa_st(u8* p, u32 v) { *(u32*)p = v; }

// 1.13c Fog+0x1CD50
void* fog_sub_1cd50(const FogAsyncApi* a, void* thisp, bool flag, const char* file, int line) {
    void* r = a->alloc(flag ? thisp : nullptr);   // -(DAT_6ff7f350 != 0) & this
    if (!r) a->logAlloc(2, file, line, "");
    return r;
}

// 1.13c Fog+0x1D7F0
void fog_sub_1d7f0(const FogAsyncApi* a, FogAsyncState* s) {
    if (s->table) {
        for (u32 i = 0; i < s->count; i++) {
            u8* e = s->table + i * 12;
            if (fa_ld(e)) { a->closeHandle(fa_ld(e)); fa_st(e, 0); }
        }
        a->free_(s->table);
        s->table = nullptr;
        s->count = 0;
    }
    s->initialized = 0;
}

// 1.13c Fog+0x1DB40
void fog_sub_1db40(const FogAsyncApi* a, FogAsyncState* s, u8* obj, int v) {
    if (!obj) { fog_n41_die(&a->die, 0x1df); return; }
    const u32 f = fa_ld(obj + 0x48);
    if ((f & 0xfffffff0) != 0x41737960) { fog_n41_die(&a->die, 0x1e0); return; }
    if (0 < v) { fog_n41_die(&a->die, 0x1e1); return; }
    if (s->dataBase < 1) {
        if ((u32)v != fa_ld(obj + 0x44)) {
            fa_st(obj + 0x44, (u32)v);
            a->setHandleLimit(fa_ld(obj + 0x3c), v);
        }
        return;
    }
    if ((f & 8) == 0) {
        fa_st(obj + 0x4c, fa_ld(obj + 0x44));
        fa_st(obj + 0x44, (u32)v);
        fa_st(obj + 0x48, f | 8);
        fa_st(obj + 0x50, (u32)(uintptr_t)s->pending);
        s->pending = obj;
        return;
    }
    if ((int)fa_ld(obj + 0x44) < v) fa_st(obj + 0x44, (u32)v);
}

// 1.13c Fog+0x1DF00
void fog_sub_1df00(const FogAsyncApi* a, FogAsyncState* s, int enable) {
    if (s->initialized) { fog_n41_die(&a->die, 0x7b); return; }
    a->setLimit(0x40000);   // Ordinal_284
    a->setLimit(0x4000);    // Ordinal_285
    if (!enable) return;
    u8* t = (u8*)a->alloc(nullptr);
    if (!t) {
        a->logAlloc(2, "..\\Source\\Fog\\Src\\AsyncData.cpp", 0x88, "");
        s->table = nullptr;
        fog_n41_die(&a->die, 0x89);
        return;
    }
    s->table = t;
    for (int o = 0; o < 0xc00; o += 12) {
        fa_st(t + o, a->createEvent());
        fa_st(t + o + 4, 1);
    }
    s->count = 0x100;
    s->initialized = 1;
}

// --- Bit packer: lens/codes are the retail tables at +0x7F210 / +0x80DD0 -------------------------
// 1.13c Fog+0x1EE60
int fog_sub_1ee60(u8* dst, int dstCap, const u8* src, int srcLen, const u8* lens, const u8* codes) {
    u8 acc = 0;
    int bits = 8;
    u8* p = dst;
    if (srcLen != 0) {
        do {
            srcLen--;
            const u8 b = *src++;
            u32 n = lens[b];
            const u8 code = codes[b];
            int cap = dstCap;
            if (bits <= (int)n) {
                do {
                    n -= bits;
                    dstCap = cap - 1;
                    if (cap == 0) return 0;
                    bits = 8;
                    *p++ = (u8)(code >> (n & 0x1f) | acc);
                    acc = 0;
                    cap = dstCap;
                } while (7 < (int)n);
            }
            if (n != 0) {
                bits -= n;
                acc |= (u8)(code << (bits & 0x1f));
            }
        } while (srcLen != 0);
        if (bits < 8) {
            if (dstCap == 0) return 0;
            *p++ = acc;
        }
    }
    return (int)(p - dst);
}

// Statistics: freq[256] u64 at +0x80ED0, five u64 counters at +0x9AF48..+0x9AF5C.
struct FogHuffStats {
    u64 freq[256];
    u64 inBytes;     // DAT_6ff9af48/4c
    u64 outBytes;    // DAT_6ff9af50/54
    u64 blocks;      // DAT_6ff9af58/5c
};
// 1.13c Fog+0x1EF10
void fog_sub_1ef10(FogHuffStats* st, const u8* data, u32 n, const u8* lens) {
    u32 bits = 0;
    for (u32 i = 0; i < n; i++) {
        st->freq[data[i]]++;
        bits += lens[data[i]];
    }
    u32 out = (bits + 7u >> 3) + 1;
    u32 hdr = 1;
    if (out > 0xef) { out = (bits + 7u >> 3) + 2; hdr = 2; }
    st->inBytes += n;
    st->outBytes += out;
    st->blocks += hdr;
}

// --- d2exp.mpq presence probe -------------------------------------------------------------------
struct FogExpState { u32 probed; u32 found; };   // DAT_6ffa6ecc / DAT_6ffa6ec8
struct FogExpApi {
    void (*moduleDir)(char* buf, u32 cap);                                           // Fog+0x17E60
    void (*joinName)(char* buf, u32 cap, const char* name);                          // Ordinal_503
    void (*joinNameAlt)(char* buf, const char* name);                                // Ordinal_501
    u32 (*getFileAttributes)(const char* path);
};
// 1.13c Fog+0x1F9B0
u32 fog_sub_1f9b0(const FogExpApi* a, FogExpState* s) {
    if (s->probed) return s->found;
    s->probed = 1;
    char path[260];
    a->moduleDir(path, 0x104);
    a->joinName(path, 0x104, "d2exp.mpq");
    if (a->getFileAttributes(path) == 0xffffffffu) {
        a->joinNameAlt(path, "d2exp.mpq");
        if (a->getFileAttributes(path) == 0xffffffffu) return s->found;
    }
    s->found = 1;
    return 1;
}
