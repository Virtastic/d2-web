// src/core/realm/bnclient_batch_1000.cpp -- twelve UNMAPPED 1.13c BNClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane). Image base 0x6FF20000; every
// DAT_ / string is named at its retail RVA. The DLL's imported host services are seams after the retail
// arguments (src/core/realm/mcp_crt_mem.cpp pattern): these are the NewsTable array's Storm SMem*
// allocator, the CRT heap behind __onexit, the two SHA-1 steps at 0x14930/0x149E0, and the CRT lock.
// The rows are: a deleting-destructor helper, a 16-byte zero initialiser, three NewsTable destructors,
// the array's growth step and ensure-capacity, the SHA-1 update+final wrapper, and the four MSVC 2003
// __onexit leaves.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 0x6FF3E2E8: the RTTI name of the NewsTable::AUItem array, passed as SMem*'s `file` argument.
static const char kBnNewsTableType[] = ".?AUItem@NewsTable@@";
// 0x6FF3E8CC is the header of that array: [0] capacity, [1] count, [2] buffer, [0xC] growth field
// (0x6FF3E8D8). Entries are 0x408 bytes (0x102 dwords).

// Storm.dll!#403 SMemFree(ptr, file, line, flags), called through thunk 0x801E.
typedef void (*BnSmemFreeFn)(uint32_t ptr, const char* file, uint32_t line, uint32_t flags);
// Storm.dll!#401 SMemAlloc(size, file, line, flags), called through thunk 0x8018.
typedef uint32_t (*BnSmemAllocFn)(uint32_t size, const char* file, uint32_t line, uint32_t flags);
// Storm.dll!#405 SMemReAlloc(ptr, size, file, line, flags), called through thunk 0x8072.
typedef uint32_t (*BnSmemReAllocFn)(uint32_t ptr, uint32_t size, const char* file, uint32_t line,
                                    uint32_t flags);

// The onexit table: DAT_6FF40D18 is the block base, DAT_6FF40D14 the next free slot. Retail stores
// 4-byte entries; the base/tail are host addresses and stay pointer-sized.
static uintptr_t g_bn_onexit_base;   // DAT_6FF40D18
static uintptr_t g_bn_onexit_tail;   // DAT_6FF40D14

// The CRT heap leaves 0x129A/0x131A call: __msize (0x2427), _realloc (0x2275), _malloc (0x2544).
typedef size_t (*BnMsizeFn)(void* p);
typedef void* (*BnReallocFn)(void* p, size_t size);
typedef void* (*BnMallocFn)(size_t size);

extern "C" void d2_fn_bnclient_1374(void (*crt_unlock)(int));

// The destructor rule shared by 0x10C0, 0x1110 and 0x1160 (and the failure path of 0x12A70): release
// the NewsTable array at this+8 through SMemFree(ptr, ".?AUItem@NewsTable@@", -2, 0). The pointer is
// not cleared. At 0x1160 retail passes `this` in EAX; Ghidra could not recover it as a parameter.
static void bn_news_table_release(uint32_t self, BnSmemFreeFn smem_free) {
    const uint32_t array = *(const uint32_t*)(uintptr_t)(self + 8);
    if (array != 0) smem_free(array, kBnNewsTableType, 0xFFFFFFFEu, 0);
}

// 1.13c BNClient+0x01000 (24 B, caller 0x1C5A the scalar deleting destructor): operator delete. A
// non-NULL block goes to SMemFree(ptr, "delete", -1, 0); NULL is ignored. Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1000(uint32_t block, BnSmemFreeFn smem_free) {
    if (block != 0) smem_free(block, "delete", 0xFFFFFFFFu, 0);
}

// 1.13c BNClient+0x01070 (66 B, no callers in the image): initialise a 16-byte object -- four dwords
// zeroed in order. Returns nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1070(uint32_t* r) {   // r[0] = the object (function probe shape)
    uint32_t* object = (uint32_t*)(uintptr_t)r[0];
    object[0] = 0;
    object[1] = 0;
    object[2] = 0;
    object[3] = 0;
    r[4] = r[0];   // retail leaves the object pointer in EAX (vector result 0xB1000000 = blob 0)
}

// 1.13c BNClient+0x010C0 (71 B, no callers in the image): NewsTable destructor. Under the SEH frame
// (0x6FF385D2) it releases this+8 through the shared rule. The frame is exception plumbing, not state.
void bn_news_table_dtor_10c0(uint32_t self, BnSmemFreeFn smem_free) {
    bn_news_table_release(self, smem_free);   // lab_6ff385d2 is the SEH continuation
}

// 1.13c BNClient+0x01110 (71 B, callers 0x185C0/0x185E8): the same destructor body as 0x10C0 under a
// different SEH continuation (0x6FF385A8). Release this+8, do not clear it.
void bn_news_table_dtor_1110(uint32_t self, BnSmemFreeFn smem_free) {
    bn_news_table_release(self, smem_free);   // lab_6ff385a8 is the SEH continuation
}

// 1.13c BNClient+0x01160 (23 B, callers 0x185A0/0x185CA/0x185E0): the destructor's release step with
// no SEH frame. `this` arrives in EAX (Ghidra sees only in_EAX); the only argument is the SMemFree thunk.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1160(uint32_t self, BnSmemFreeFn smem_free) {
    const uint32_t array = *(const uint32_t*)(uintptr_t)(self + 8);
    if (array != 0) smem_free(array, kBnNewsTableType, 0xFFFFFFFEu, 0);
}

// 1.13c BNClient+0x01180 (57 B, caller 0x12A70): the NewsTable capacity step. The requested capacity
// arrives in EAX (our `n`), the array header at 0x6FF3E8CC is the stack argument. n >= 8: the header's
// growth field at +0xC (0x6FF3E8D8) becomes 8 and 8 is returned. n < 8: return n with its set bits
// cleared down to the last non-zero value -- the largest power of two <= n -- and 1 when n is 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_1180(uint32_t n, uint32_t* header) {
    if (n < 8) {
        uint32_t value = n;
        for (uint32_t next = (value - 1) & value; next != 0; next = (value - 1) & value) value = next;
        return value == 0 ? 1u : value;
    }
    header[3] = 8;   // param_1 + 0xC, DAT_6FF3E8D8
    return 8;
}

// 1.13c BNClient+0x011C0 (124 B, caller 0x12A70): ensure the array holds `new_capacity` records. The
// new capacity arrives in EAX and is stored at header[0]; header[1] is the live count, header[2] the
// buffer. The old buffer is reallocated to new_capacity*0x408 through SMemReAlloc(old, size, tag, -2,
// 0x10). If that returns NULL, SMemAlloc(size, tag, -2, 0) is used instead, min(count, new_capacity)
// full 0x408-byte records are copied to the new block, and the old block is freed. No alloc failure is
// checked after the fallback (a NULL buffer simply skips the copies, as retail's pointer test does).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_11c0(uint32_t new_capacity, uint32_t* header,
                                              BnSmemReAllocFn smem_realloc,
                                              BnSmemAllocFn smem_alloc, BnSmemFreeFn smem_free) {
    const uint32_t old_buffer = header[2];
    const uint32_t bytes = new_capacity * 0x408u;
    header[0] = new_capacity;
    uint32_t buffer = smem_realloc(old_buffer, bytes, kBnNewsTableType, 0xFFFFFFFEu, 0x10u);
    header[2] = buffer;
    if (buffer != 0) return;
    buffer = smem_alloc(bytes, kBnNewsTableType, 0xFFFFFFFEu, 0);
    header[2] = buffer;
    if (old_buffer == 0) return;
    uint32_t count = header[1] <= new_capacity ? header[1] : new_capacity;
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t dst = buffer + i * 0x408u;
        if (dst != 0) {
            std::memcpy((void*)(uintptr_t)dst, (const void*)(uintptr_t)(old_buffer + i * 0x408u), 0x408);
        }
    }
    smem_free(old_buffer, kBnNewsTableType, 0xFFFFFFFEu, 0);
}

// 1.13c BNClient+0x01240 (90 B, caller 0x103B0): SHA-1 finish. The context arrives in EDI and the data
// pointer in EAX; the stack arguments are param_1 = digest output, param_2 (never read in the body) and
// param_3 = data length. It runs the update step 0x14930(ctx, data, length) and then the final step
// 0x149E0(ctx, digest output). Both are UNMAPPED, so they are seams after the retail arguments.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1240(uint32_t digest_out, uint32_t unused, uint32_t length,
                                              const uint8_t* data, void* context,
                                              void (*sha1_update)(void*, const uint8_t*, uint32_t),
                                              void (*sha1_final)(void*, uint32_t)) {
    (void)unused;
    sha1_update(context, data, length);   // 0x14930: EAX = data, EDI = context, length on the stack
    sha1_final(context, digest_out);      // 0x149E0: EAX = context, digest buffer on the stack
}

// 1.13c BNClient+0x0129A (__onexit_lk, 128 B, caller 0x1342): append the function pointer retail passes
// in EDI (our `func`) to the onexit table. The block size comes from __msize(base). When the table lacks
// room for one 4-byte slot (size < tail - base + 4), grow it by min(size, 0x800) through _realloc and
// rebase the tail to new_base + (tail - base); if that fails retry once with size + 0x10; if that also
// fails return 0. Otherwise store the pointer at the tail, advance the tail by 4 and return the pointer.
// Ghidra types it void; __onexit returns its EAX as the registration result.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_129a(uint32_t func, BnMsizeFn crt_msize,
                                                  BnReallocFn crt_realloc) {
    const uintptr_t base = g_bn_onexit_base;
    const uintptr_t tail = g_bn_onexit_tail;
    const size_t size = crt_msize((void*)base);
    if (size < (size_t)(tail + 4 - base)) {
        const size_t grow = size < 0x800 ? size : 0x800;
        void* grown = crt_realloc((void*)base, grow + size);
        if (grown == nullptr) {
            grown = crt_realloc((void*)base, size + 0x10);
            if (grown == nullptr) return 0;
        }
        g_bn_onexit_tail = (uintptr_t)grown + (tail - base);
        g_bn_onexit_base = (uintptr_t)grown;
    }
    *(uint32_t*)g_bn_onexit_tail = func;
    g_bn_onexit_tail += 4;
    return func;
}

// 1.13c BNClient+0x0131A (___onexitinit, 40 B, caller 0x143C): create the onexit table. _malloc(0x80);
// NULL returns 0x18 (ENOMEM) and leaves the globals untouched; otherwise the first dword is zeroed
// (NULL terminator), DAT_6FF40D14 = DAT_6FF40D18 = the block, and the result is 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_131a(BnMallocFn crt_malloc) {
    void* table = crt_malloc(0x80);
    if (table == nullptr) return 0x18;
    g_bn_onexit_base = (uintptr_t)table;
    *(uint32_t*)table = 0;
    g_bn_onexit_tail = g_bn_onexit_base;
    return 0;
}

// 1.13c BNClient+0x01342 (__onexit, 50 B, caller 0x137A _atexit): __lock(8) (the 0x1412 wrapper),
// register through __onexit_lk with the function pointer in EDI, then the 0x1374 unlock wrapper; return
// __onexit_lk's EAX.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_bnclient_1342(uint32_t func, void (*crt_lock)(int),
                                                  void (*crt_unlock)(int), BnMsizeFn crt_msize,
                                                  BnReallocFn crt_realloc) {
    crt_lock(8);
    const uint32_t result = d2_fn_bnclient_129a(func, crt_msize, crt_realloc);
    d2_fn_bnclient_1374(crt_unlock);
    return result;
}

// 1.13c BNClient+0x01374 (6 B, caller 0x1342): the unlock wrapper -- tail-call _unlock(8) through
// 0x141B. The lock number is fixed in the image, so the seam receives it.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_1374(void (*crt_unlock)(int)) {
    crt_unlock(8);
}
