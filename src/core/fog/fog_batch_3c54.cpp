// Fog.dll MSVC 2003 CRT rows 0x3C54..0x5510, from build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane).
// Image base 0x6FF50000. Win32 imports and unrecovered callees are host seams after the retail args.
// The sbh_free_block body is the same CRT code as src/core/realm/mcp_sbh_free.cpp (D2MCPClient+0x356F).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// --- __sbh_* state, named at its retail RVA ---------------------------------------------------
uint32_t g_fog_sbh_entry;        // +0x58264 DAT_6ffa8264
uint32_t g_fog_sbh_count;        // +0x58268
uint32_t g_fog_sbh_regions;      // +0x5826C
uint32_t g_fog_sbh_max;          // +0x58270
uint32_t g_fog_sbh_last_region;  // +0x58274
uint32_t g_fog_sbh_capacity;     // +0x58278
uint32_t g_fog_sbh_seq;          // +0x5827C
uint32_t g_fog_heap;             // +0x58280 Win32 heap handle
uint32_t g_fog_heap_kind;        // +0x58284 ___heap_select result
// +0x6E210 lock table: 8-byte slots, first dword = CRITICAL_SECTION*
uint32_t g_fog_locktbl[2 * 48];

struct FogCrtEnv {
    void*    (*malloc)(uint32_t);                         // _malloc
    void     (*free)(void*);                              // _free
    void     (*lock)(int32_t);                            // __lock (0x3CF4)
    void     (*unlock10)(void);                           // 0x3CEB
    int32_t  (*init_cs_spin)(void*, uint32_t);            // ___crtInitCritSecAndSpinCount
    void     (*enter_cs)(void*);                          // EnterCriticalSection
    void     (*amsg_exit)(int32_t);                       // __amsg_exit
    void     (*set_errno)(int32_t);                       // *FUN_6ff56a5a() = v
    void     (*local_unwind2)(int32_t, int32_t);          // 0x9182
    uint32_t (*heap_create)(uint32_t, uint32_t, uint32_t);// HeapCreate
    void     (*heap_destroy)(uint32_t);                   // HeapDestroy
    void*    (*heap_alloc)(uint32_t, uint32_t, uint32_t); // HeapAlloc
    int32_t  (*heap_free)(uint32_t, uint32_t, uint32_t);  // HeapFree
    int32_t  (*virtual_free)(uint32_t, uint32_t, uint32_t);// VirtualFree
    int32_t  (*heap_select)(void);                        // ___heap_select (0x-6FF4FFBE slot)
    void     (*write_char)(void);                         // 0x4091
};

// 1.13c FOG+0x3C54 __mtinitlocknum(n): allocate and publish lock slot n (spin count 4000) under lock 10.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_mtinitlocknum_3c54(int32_t n, const FogCrtEnv* e) {
    uint32_t* slot = &g_fog_locktbl[n * 2];
    if (*slot) return 1;
    void* m = e->malloc(0x18);
    if (!m) { e->set_errno(0xc); return 0; }
    e->lock(10);
    if (*slot == 0) {
        if (e->init_cs_spin(m, 4000) == 0) { e->free(m); e->set_errno(0xc); e->unlock10(); return 0; }
        *slot = (uint32_t)(uintptr_t)m;
    } else e->free(m);
    e->unlock10();
    return 1;
}

// 1.13c FOG+0x3CF4 __lock(n): init the slot on first use (fatal 0x11 on failure), then enter it.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_lock_3cf4(int32_t n, const FogCrtEnv* e) {
    if (g_fog_locktbl[n * 2] == 0 && fog_mtinitlocknum_3c54(n, e) == 0) e->amsg_exit(0x11);
    e->enter_cs((void*)(uintptr_t)g_fog_locktbl[n * 2]);
}

// 1.13c FOG+0x3D25 __RTC_Initialize: the init-pointer range is empty (begin == end), so it does nothing.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_rtc_initialize_3d25(void) {}

// 1.13c FOG+0x3DB0 __SEH_prolog / 1.13c FOG+0x3DEB __SEH_epilog: compiler-injected x86 frame/ExceptionList
// linkage; no wasm counterpart, nothing to emulate.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_seh_prolog_3db0(void) {}
extern "C" EMSCRIPTEN_KEEPALIVE void fog_seh_epilog_3deb(void) {}

// 1.13c FOG+0x3EEA: __local_unwind2(frame[+0x18], frame[+0x1c]).
extern "C" EMSCRIPTEN_KEEPALIVE void fog_unwind_3eea(const uint8_t* frame, const FogCrtEnv* e) {
    int32_t a, b; memcpy(&a, frame + 0x18, 4); memcpy(&b, frame + 0x1c, 4);
    e->local_unwind2(a, b);
}

// 1.13c FOG+0x40E8 write_string(count): `stream_flags` = *(stream+0xc), `stream_ptr` = *(stream+8), `*written`
// = EAX target. Unbuffered-string stream (flag 0x40 with null ptr) just adds count; else write_char per char
// until *written == -1.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_write_string_40e8(int32_t count, int32_t* written,
        uint32_t stream_flags, uint32_t stream_ptr, const FogCrtEnv* e) {
    if ((stream_flags & 0x40) == 0 || stream_ptr != 0) {
        do {
            if (count < 1) return;
            count--;
            e->write_char();
        } while (*written != -1);
    } else *written += count;
}

// 1.13c FOG+0x48F5 __heap_init(mt): HeapCreate(mt==0, 0x1000, 0); select allocator; kind 3 -> sbh init(0x3f8).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_heap_init_48f5(int32_t mt, const FogCrtEnv* e);

// 1.13c FOG+0x49C5 ___sbh_heap_init(max): region table of 0x140 bytes, empty.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t fog_sbh_heap_init_49c5(uint32_t max, const FogCrtEnv* e) {
    g_fog_sbh_regions = (uint32_t)(uintptr_t)e->heap_alloc(g_fog_heap, 0, 0x140);
    if (!g_fog_sbh_regions) return 0;
    g_fog_sbh_entry = 0;
    g_fog_sbh_count = 0;
    g_fog_sbh_last_region = g_fog_sbh_regions;
    g_fog_sbh_max = max;
    g_fog_sbh_capacity = 0x10;
    return 1;
}

extern "C" int32_t fog_heap_init_48f5(int32_t mt, const FogCrtEnv* e) {
    g_fog_heap = e->heap_create(mt == 0, 0x1000, 0);
    if (!g_fog_heap) return 0;
    g_fog_heap_kind = (uint32_t)e->heap_select();
    if (g_fog_heap_kind == 3 && fog_sbh_heap_init_49c5(0x3f8, e) == 0) { e->heap_destroy(g_fog_heap); return 0; }
    return 1;
}

// 1.13c FOG+0x4946 __heap_term: kind 3 -> per region decommit+release its 1 MB range and free its group
// block (20-byte records from regions+0xC), free the table; then HeapDestroy.
extern "C" EMSCRIPTEN_KEEPALIVE void fog_heap_term_4946(const FogCrtEnv* e) {
    if (g_fog_heap_kind == 3) {
        uint32_t* p = P32(g_fog_sbh_regions + 0xc);
        for (int32_t i = 0; i < (int32_t)g_fog_sbh_count; i++, p += 5) {
            e->virtual_free(p[0], 0x100000, 0x4000);
            e->virtual_free(p[0], 0, 0x8000);
            e->heap_free(g_fog_heap, 0, p[1]);
        }
        e->heap_free(g_fog_heap, 0, g_fog_sbh_regions);
    }
    e->heap_destroy(g_fog_heap);
}

// 1.13c FOG+0x5510 _memcpy: the retail body is overlap-safe (copies backward when dst lies inside src..src+n),
// which is memmove semantics; returns dst.
extern "C" EMSCRIPTEN_KEEPALIVE void* fog_memcpy_5510(void* dst, const void* src, uint32_t n) {
    return memmove(dst, src, n);
}

struct FogSbhFreeEnv {
    void (*virtual_free)(uint32_t addr, uint32_t size, uint32_t type);
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);
};
// 1.13c FOG+0x4A38 (792 B) ___sbh_free_block(region, block): same CRT code as D2MCPClient+0x356F.
extern "C"
EMSCRIPTEN_KEEPALIVE void fog_sbh_free_block_4a38(uint32_t region, uint32_t block, const FogSbhFreeEnv* env) {
    uint32_t uVar6 = P32(region)[4];                    // heap control block
    uint32_t puVar12 = block - 4;                       // block header
    uint32_t uVar14 = (uint32_t)(block - P32(region)[3]) >> 0xf;   // subregion index
    uint32_t piVar4 = uVar14 * 0x204 + 0x144 + uVar6;   // bucket descriptor array
    uint32_t local_8 = *P32(puVar12) - 1;
    if ((local_8 & 1) != 0) return;

    uint32_t puVar10 = local_8 + puVar12;               // next block
    uint32_t uVar13 = *P32(puVar10);
    uint32_t uVar7 = *P32(block - 8);                   // previous block size word
    if ((uVar13 & 1) == 0) {                            // next block is free: unlink it
        uint32_t bucket = (uint32_t)(((int32_t)uVar13 >> 4) - 1);
        if (bucket > 0x3f) bucket = 0x3f;
        if (P32(puVar10)[1] == P32(puVar10)[2]) {       // singleton in its bucket
            uint8_t* tally = P8(bucket + 4 + uVar6);
            if (bucket < 0x20) {
                uint32_t mask = ~(0x80000000u >> (bucket & 0x1f));
                uint32_t* q = P32(uVar6 + 0x44 + uVar14 * 4);
                *q &= mask;
                if (--*tally == 0) *P32(region) &= mask;
            } else {
                uint32_t mask = ~(0x80000000u >> ((bucket - 0x20) & 0x1f));
                uint32_t* q = P32(uVar6 + 0xc4 + uVar14 * 4);
                *q &= mask;
                if (--*tally == 0) P32(region)[1] &= mask;
            }
        }
        local_8 += uVar13;
        *(uint32_t*)(uintptr_t)(P32(puVar10)[2] + 4) = P32(puVar10)[1];
        *(uint32_t*)(uintptr_t)(P32(puVar10)[1] + 8) = P32(puVar10)[2];
    }

    uint32_t bucket = (uint32_t)(((int32_t)local_8 >> 4) - 1);
    if (bucket > 0x3f) bucket = 0x3f;
    uint32_t prev_bucket = region;                      // stand-in for the decompile's param_1 pointer
    if ((uVar7 & 1) == 0) {                             // previous block is free: unlink it
        puVar12 -= uVar7;
        prev_bucket = (uint32_t)(((int32_t)uVar7 >> 4) - 1);
        if (prev_bucket > 0x3f) prev_bucket = 0x3f;
        local_8 += uVar7;
        bucket = (uint32_t)(((int32_t)local_8 >> 4) - 1);
        if (bucket > 0x3f) bucket = 0x3f;
        if (prev_bucket != bucket) {
            if (P32(puVar12)[1] == P32(puVar12)[2]) {
                uint8_t* tally = P8(prev_bucket + 4 + uVar6);
                if (prev_bucket < 0x20) {
                    uint32_t mask = ~(0x80000000u >> (prev_bucket & 0x1f));
                    uint32_t* q = P32(uVar6 + 0x44 + uVar14 * 4);
                    *q &= mask;
                    if (--*tally == 0) *P32(region) &= mask;
                } else {
                    uint32_t mask = ~(0x80000000u >> ((prev_bucket - 0x20) & 0x1f));
                    uint32_t* q = P32(uVar6 + 0xc4 + uVar14 * 4);
                    *q &= mask;
                    if (--*tally == 0) P32(region)[1] &= mask;
                }
            }
            *(uint32_t*)(uintptr_t)(P32(puVar12)[2] + 4) = P32(puVar12)[1];
            *(uint32_t*)(uintptr_t)(P32(puVar12)[1] + 8) = P32(puVar12)[2];
        }
    }
    if ((uVar7 & 1) != 0 || prev_bucket != bucket) {     // link the coalesced block
        uint32_t piVar1 = piVar4 + bucket * 8;
        uint32_t next = P32(piVar1)[1];
        P32(puVar12)[2] = piVar1;
        P32(puVar12)[1] = next;
        P32(piVar1)[1] = puVar12;
        *(uint32_t*)(uintptr_t)(P32(puVar12)[1] + 8) = puVar12;
        if (P32(puVar12)[1] == P32(puVar12)[2]) {
            uint8_t* tally = P8(bucket + uVar6 + 4);
            uint8_t cVar5 = *tally;
            *tally = (uint8_t)(cVar5 + 1);
            if (bucket < 0x20) {
                uint32_t bit = 0x80000000u >> (bucket & 0x1f);
                if (cVar5 == 0) *P32(region) |= bit;
                uint32_t* q = P32(uVar6 + 0x44 + uVar14 * 4);
                *q |= bit;
            } else {
                uint32_t bit = 0x80000000u >> ((bucket - 0x20) & 0x1f);
                if (cVar5 == 0) P32(region)[1] |= bit;
                uint32_t* q = P32(uVar6 + 0xc4 + uVar14 * 4);
                *q |= bit;
            }
        }
    }
    *P32(puVar12) = local_8;
    *(uint32_t*)(uintptr_t)(local_8 - 4 + puVar12) = local_8;
    uint32_t* bucketHead = P32(piVar4);
    *bucketHead -= 1;
    if (*bucketHead != 0) return;

    if (g_fog_sbh_entry != 0) {
        env->virtual_free(g_fog_sbh_seq * 0x8000 + P32(g_fog_sbh_entry)[3], 0x8000, 0x4000);   // MEM_DECOMMIT
        P32(g_fog_sbh_entry)[2] |= 0x80000000u >> (g_fog_sbh_seq & 0x1f);
        *(uint32_t*)(uintptr_t)(P32(g_fog_sbh_entry)[4] + 0xc4 + g_fog_sbh_seq * 4) = 0;
        uint8_t* regcount = P8(P32(g_fog_sbh_entry)[4] + 0x43);
        if (--*regcount == 0) P32(g_fog_sbh_entry)[1] &= 0xfffffffeu;
        if (P32(g_fog_sbh_entry)[2] == 0xFFFFFFFFu) {
            env->virtual_free(P32(g_fog_sbh_entry)[3], 0, 0x8000);                         // MEM_RELEASE
            env->heap_free(g_fog_heap, 0, P32(g_fog_sbh_entry)[4]);
            std::memmove(P32(g_fog_sbh_entry), P32(g_fog_sbh_entry + 0x14),
                         (uint32_t)(g_fog_sbh_count * 0x14u - g_fog_sbh_entry - 0x14u + g_fog_sbh_regions));
            g_fog_sbh_count -= 1;
            if (g_fog_sbh_entry < region) region -= 20;   // param_1 -= 5 uint words
            g_fog_sbh_last_region = g_fog_sbh_regions;
        }
    }
    g_fog_sbh_entry = region;
    g_fog_sbh_seq = uVar14;
}
