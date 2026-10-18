// src/core/d2client/d2client_batch_908f.cpp -- thirteen UNMAPPED 1.13c D2Client.dll CRT leaves
// reimplemented from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane).
// Image base 0x6FAB0000; every global is named at its retail RVA. Callees that are not recovered yet
// (and every kernel32/user32 import, since the wasm module has no PE import table) are host seams
// AFTER the retail arguments, which keep their order (the d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION / InitializeCriticalSection: host (kernel) state, no-op under wasm

// --- retail module state, all zero-initialised ------------------------------------------------
static uint32_t g_platform_word;      // +0x0FA8A4  DAT_6fbaa8a4: OS/platform selector (1 = NT, 2 = Win9x)
static uint32_t g_win_version;        // +0x0FA8B0  DAT_6fbaa8b0: version word gating the MB_* flags

// --- ___crtInitCritSecAndSpinCount state ------------------------------------------------------
typedef int32_t (*D2cInitCritSecFn)(void*, uint32_t);
static D2cInitCritSecFn g_init_spin;  // +0x0FAB88  DAT_6fbaab88: resolved initialiser (kernel32 or 0x907F)

// --- __local_unwind2 state --------------------------------------------------------------------
static uint32_t g_exception_list;     // FS:[0] SEH chain head (single-threaded here)

// --- __sbh_* state (the CRT small-block heap) --------------------------------------------------
static uint32_t g_sbh_regions;        // +0x123974  DAT_6fbd3974: region-array base
static uint32_t g_sbh_count;          // +0x123970  DAT_6fbd3970: region count
static uint32_t g_sbh_last;           // +0x12396C  DAT_6fbd396c: region owning the last free
static uint32_t g_sbh_last_region;    // +0x12397C  DAT_6fbd397c: last region handed out
static uint32_t g_sbh_seq;            // +0x123984  DAT_6fbd3984: subregion index of the last free
static uint32_t g_sbh_heap;           // +0x123988  DAT_6fbd3988: Win32 heap handle

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// 1.13c D2Client+0x0907F (stdcall @8, 16 B): initialize cs, ignore the spin word, return 1. Retail
// pops both stack words (`ret 8`), so it is called through the same two-argument pointer as the
// kernel32 initialiser it replaces.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_907f(void* cs, uint32_t spin) {
    (void)spin;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

struct D2ClientInitCritSecEnv {
    uint32_t (*get_module_handle)(const char* name);                       // IAT 0xCEF80 GetModuleHandleA
    D2cInitCritSecFn (*get_proc_address)(uint32_t mod, const char* name);  // IAT 0xCEF74 GetProcAddress
};

// 1.13c D2Client+0x0908F (103 B): resolve InitializeCriticalSectionAndSpinCount once. On NT
// (platform word 1) retail never asks kernel32; otherwise a null GetModuleHandleA or
// GetProcAddress leaves the no-spin thunk 0x907F in the slot. Then call the slot with (cs, spin).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_908f(void* cs, uint32_t spin, const D2ClientInitCritSecEnv* env) {
    if (g_init_spin == nullptr) {
        D2cInitCritSecFn resolved = nullptr;
        if (g_platform_word != 1) {
            uint32_t mod = env->get_module_handle("kernel32.dll");
            if (mod != 0)
                resolved = env->get_proc_address(mod, "InitializeCriticalSectionAndSpinCount");
        }
        g_init_spin = resolved != nullptr ? resolved : d2_client_907f;
    }
    g_init_spin(cs, spin);
}

// 0x091F2 ___abnormal_termination, already implemented in d2client_batch_1e70.cpp: it stores EAX,
// [EBP+8] and EBP into +0x0DB8DC/+0x0DB8E0/+0x0DB8E4. __local_unwind2 calls it with EAX = the record
// handler, [EBP+8] = its own first argument (the frame) and EBP = the caller's frame word.
extern "C" void d2_client_91f2(uint32_t eax, uint32_t ebp, uint32_t stack8);

// 1.13c D2Client+0x0915E (104 B): walk the frame's table (frame+8) from index frame+0xC, unwinding
// until the index is -1 or the target. Each 12-byte record is {next index, filter, handler}; a zero
// filter first runs 0x91F2 with the pushed selector 0x101, then the handler. The chain head is saved
// and restored. `caller_ebp` is the frame word the abandoned 0x91F2 call records.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_915e(uint8_t* frame, int32_t target, uint32_t caller_ebp) {
    uint32_t saved_list = g_exception_list;
    g_exception_list = (uint32_t)(uintptr_t)frame;        // retail publishes &pvStack_1c as the head
    for (;;) {
        uint32_t table = *(uint32_t*)(frame + 8);
        uint32_t idx = *(uint32_t*)(frame + 0xc);
        if (idx == 0xFFFFFFFFu || (int32_t)idx == target) break;
        uint8_t* rec = P8(table + idx * 12);
        *(uint32_t*)(frame + 0xc) = *(uint32_t*)rec;      // advance to the record's next index
        if (*(uint32_t*)(rec + 4) == 0) {
            uint32_t handler_addr = *(uint32_t*)(rec + 8);
            d2_client_91f2(handler_addr, caller_ebp, (uint32_t)(uintptr_t)frame);
            ((void (*)(void))(uintptr_t)handler_addr)();
        }
    }
    g_exception_list = saved_list;
}

// 1.13c D2Client+0x09252 (43 B): first 0x14-byte region whose base (record+0xC) is within 0x100000
// of the block, else 0. The scan stops at regions + count*0x14.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_9252(uint32_t block) {
    uint32_t end = g_sbh_regions + g_sbh_count * 0x14;
    for (uint32_t r = g_sbh_regions; r < end; r += 0x14)
        if ((uint32_t)(block - *(uint32_t*)(uintptr_t)(r + 0xc)) < 0x100000u) return r;
    return 0;
}

// Scenario setup: the OS words 0x908F and 0xBA32 branch on, and the whole sbh region table.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_crt_pin(uint32_t platform_word, uint32_t win_version) {
    g_platform_word = platform_word;
    g_win_version = win_version;
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_sbh_pin(uint32_t regions, uint32_t count, uint32_t last,
                                            uint32_t last_region, uint32_t seq, uint32_t heap) {
    g_sbh_regions = regions;
    g_sbh_count = count;
    g_sbh_last = last;
    g_sbh_last_region = last_region;
    g_sbh_seq = seq;
    g_sbh_heap = heap;
}

struct D2ClientSbhEnv {
    void (*virtual_free)(uint32_t addr, uint32_t size, uint32_t type);     // IAT 0xCEF6C VirtualFree
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);   // IAT 0xCEF38 HeapFree
};

// 1.13c D2Client+0x0927D (792 B): ___sbh_free_block(region, block). `local_8 = header - 1`: an odd
// result means the block was already free and the function returns. Otherwise coalesce with the free
// next block (record+0) and the free previous block (header-8), maintaining the two 32-entry bucket
// bitmaps (region[0]/region[4] for buckets 0..0x1F, region[4]+0xC4 for 0x20..0x3F) and the bucket
// counters at heap+4+bucket. Then link the block and, when its subregion empties, decommit it and
// (when the whole region empties) release the region and memmove the tail of the region table down.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_927d(uint32_t region, uint32_t block, const D2ClientSbhEnv* env) {
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

    if (g_sbh_last != 0) {
        env->virtual_free(g_sbh_seq * 0x8000 + P32(g_sbh_last)[3], 0x8000, 0x4000);   // MEM_DECOMMIT
        P32(g_sbh_last)[2] |= 0x80000000u >> (g_sbh_seq & 0x1f);
        *(uint32_t*)(uintptr_t)(P32(g_sbh_last)[4] + 0xc4 + g_sbh_seq * 4) = 0;
        uint8_t* regcount = P8(P32(g_sbh_last)[4] + 0x43);
        if (--*regcount == 0) P32(g_sbh_last)[1] &= 0xfffffffeu;
        if (P32(g_sbh_last)[2] == 0xFFFFFFFFu) {
            env->virtual_free(P32(g_sbh_last)[3], 0, 0x8000);                         // MEM_RELEASE
            env->heap_free(g_sbh_heap, 0, P32(g_sbh_last)[4]);
            std::memmove(P32(g_sbh_last), P32(g_sbh_last + 0x14),
                         (uint32_t)(g_sbh_count * 0x14u - g_sbh_last - 0x14u + g_sbh_regions));
            g_sbh_count -= 1;
            if (g_sbh_last < region) region -= 20;    // param_1 -= 5 uint words
            g_sbh_last_region = g_sbh_regions;
        }
    }
    g_sbh_last = region;
    g_sbh_seq = uVar14;
}

// --- __getptd ---------------------------------------------------------------------------------
static uint32_t g_ptd_tls_index = 0x0DB774u;   // +0x0DB774  DAT_6fb8b774: the TLS index slot

struct D2ClientGetptdEnv {
    uint32_t (*get_last_error)();                              // IAT 0xCEF34 GetLastError
    void     (*set_last_error)(uint32_t);                      // IAT 0xCEF2C SetLastError
    void*    (*tls_get_value)(uint32_t index);                 // IAT 0xFAA64 TlsGetValue
    int32_t  (*tls_set_value)(uint32_t index, void* value);    // IAT 0xFAA68 TlsSetValue
    void*    (*calloc_)(uint32_t count, uint32_t size);        // 0x8797 _calloc
    uint32_t (*get_current_thread_id)();                       // IAT 0xCEF40 GetCurrentThreadId
    void     (*amsg_exit)(int32_t code);                       // 0x46DA __amsg_exit
};

// 1.13c D2Client+0x072F7 (113 B): the current thread's _ptiddata, created on first use. The 0x8C-byte
// block is calloc(1, 0x8C); its fields are _tid +0x00, _thandle +0x04 (set to -1), _holdrand +0x14
// (set to 1) and _initaddr +0x54 (the retail VA 0x6FB8B7B8). A failed calloc or TlsSetValue runs
// __amsg_exit(0x10). The caller's last error is restored before returning.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_72f7(const D2ClientGetptdEnv* env) {
    uint32_t saved_error = env->get_last_error();
    void* ptd = env->tls_get_value(g_ptd_tls_index);
    if (ptd == nullptr) {
        ptd = env->calloc_(1, 0x8C);
        if (ptd != nullptr && env->tls_set_value(g_ptd_tls_index, ptd) != 0) {
            uint8_t* p = (uint8_t*)ptd;
            *(uint32_t*)(p + 0x54) = 0x6FB8B7B8u;
            *(uint32_t*)(p + 0x14) = 1;
            *(uint32_t*)(p + 0x04) = 0xFFFFFFFFu;
            *(uint32_t*)(p + 0x00) = env->get_current_thread_id();
        } else {
            env->amsg_exit(0x10);
        }
    }
    env->set_last_error(saved_error);
    return ptd;
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_getptd_pin(uint32_t tls_index) {
    g_ptd_tls_index = tls_index;
}
// The same slot, read and reset by __mtterm (0x72C7 in d2client_batch_057ae.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_getptd_index(void) {
    return g_ptd_tls_index;
}

struct D2ClientNhMallocEnv {
    void*   (*heap_alloc)(uint32_t size);    // 0x7CB8 __heap_alloc
    int32_t (*callnewh)(uint32_t size);      // 0xBA17 __callnewh
};

// 1.13c D2Client+0x07D33 (44 B): __nh_malloc(size, nhflag). A size at or above 0xFFFFFFE1 fails
// outright; otherwise __heap_alloc, then, when nhflag is set, retry through __callnewh until it
// declines. A zero nhflag returns the first failure.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_client_7d33(uint32_t size, int32_t nhflag, const D2ClientNhMallocEnv* env) {
    if (size >= 0xFFFFFFE1u) return nullptr;
    for (;;) {
        void* p = env->heap_alloc(size);
        if (p != nullptr) return p;
        if (nhflag == 0) return nullptr;
        if (env->callnewh(size) == 0) return nullptr;
    }
}

// SYSTEM_INFO as GetSystemInfo fills it on x86; only dwPageSize (+0x04) is read here.
struct D2ClientSystemInfo {
    uint32_t oem_id;                   // +0x00
    uint32_t page_size;                // +0x04
    uint32_t min_app_address;          // +0x08
    uint32_t max_app_address;          // +0x0C
    uint32_t active_processor_mask;    // +0x10
    uint32_t number_of_processors;     // +0x14
    uint32_t processor_type;           // +0x18
    uint32_t allocation_granularity;   // +0x1C
    uint16_t processor_level;          // +0x20
    uint16_t processor_revision;       // +0x22
};
static_assert(offsetof(D2ClientSystemInfo, page_size) == 0x04, "SYSTEM_INFO.dwPageSize");
static_assert(sizeof(D2ClientSystemInfo) == 0x24, "SYSTEM_INFO is 0x24 bytes on x86");

// MEMORY_BASIC_INFORMATION as VirtualQuery fills it on x86 (0x1C bytes).
struct D2ClientResetStkMemoryBasicInformation {
    uint32_t base;               // +0x00
    uint32_t allocation_base;    // +0x04
    uint32_t allocation_protect; // +0x08
    uint32_t region_size;        // +0x0C
    uint32_t state;              // +0x10
    uint32_t protect;            // +0x14
    uint32_t type;               // +0x18
};
static_assert(offsetof(D2ClientResetStkMemoryBasicInformation, protect) == 0x14, "MBI.Protect");
static_assert(sizeof(D2ClientResetStkMemoryBasicInformation) == 0x1c, "MBI is 0x1C bytes");

// kernel32 imports, seams: the wasm module has no PE import table.
struct D2ClientResetStkEnv {
    uint32_t (*virtual_query)(uint32_t addr, D2ClientResetStkMemoryBasicInformation* mbi, uint32_t len);
    void     (*get_system_info)(D2ClientSystemInfo* si);
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
    int32_t  (*virtual_protect)(uint32_t addr, uint32_t size, uint32_t new_protect,
                                uint32_t* old_protect);
};

// 1.13c D2Client+0x07D71 (227 B): __resetstkoflw. Restore the guard page one page below the stack slot
// the caller probed. `stack_slot` is the address of the decompile's local `uStack_60` (retail stores
// its own return address 0x6FAB7D82 there; only the address is read). With platform != 1 (Win9x) the
// committed region covering the stack is found by walking regions up from AllocationBase, a region
// already carrying PAGE_GUARD returns 1, and an uncommitted page at the walk start is committed first.
// Then the target page is protected: PAGE_NOACCESS on NT, PAGE_READWRITE|PAGE_GUARD on Win9x. All four
// kernel32 calls are seams; the page size and both limits come from the OS, the platform selector from
// +0x0FA8A4.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_7d71(uint32_t stack_slot, const D2ClientResetStkEnv* env) {
    D2ClientResetStkMemoryBasicInformation mbi;
    if (env->virtual_query(stack_slot, &mbi, 0x1c) == 0) return 0;

    D2ClientSystemInfo si;
    env->get_system_info(&si);
    const uint32_t page = si.page_size;
    const uint32_t guard = (~(page - 1) & stack_slot) - page;   // pvVar4
    // pvVar3: (-(platform != 1) & 0xFFFFFFF1) + 0x11 pages above AllocationBase: 17 on NT, 2 on Win9x.
    const uint32_t limit = ((g_platform_word != 1) ? 2u : 17u) * page + mbi.allocation_base;
    if (limit > guard) return 0;

    uint32_t target = guard;
    if (g_platform_word != 1) {
        target = mbi.allocation_base;
        for (;;) {
            if (env->virtual_query(target, &mbi, 0x1c) == 0) return 0;
            target = target + mbi.region_size;
            if ((mbi.state & 0x1000) != 0) break;             // MEM_COMMIT
        }
        target = mbi.base;
        if (((mbi.protect >> 8) & 1u) != 0) return 1;         // Protect._1_1_ & 1: PAGE_GUARD
        if (guard < mbi.base) return 0;
        if (mbi.base < limit) target = limit;
        env->virtual_alloc(target, page, 0x1000, 4);          // MEM_COMMIT, PAGE_READWRITE
    }
    uint32_t old_protect;
    const uint32_t protect = ((g_platform_word != 1) ? 0x103u : 0u) + 1;  // 0x104 / PAGE_NOACCESS
    return env->virtual_protect(target, page, protect, &old_protect);
}

// 1.13c D2Client+0x07E60 (61 B): __chkstk. `size` arrives in EAX, `esp` is the entry stack pointer
// (the address the return address was pushed to) and `retaddr` the word stored by the probe. Every
// 0x1000-byte page downward is touched so the guard page faults before the frame is entered. Under
// wasm there is no guard page, so the observable is the sequence of stored addresses.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_7e60(uint32_t size, uint8_t* esp, uint32_t retaddr) {
    if (size < 0x1000) {                       // *(undefined4 *)(&stack0 + -size) = retaddr
        *(uint32_t*)(esp - size) = retaddr;
        return;
    }
    uint8_t* p = esp + 4;
    do {
        p -= 0x1000;
        size -= 0x1000;
    } while (size > 0xFFF);
    *(uint32_t*)(p - 4 - size) = retaddr;
}

// --- ___crtMessageBoxA ------------------------------------------------------------------------
typedef int32_t (*D2cMessageBoxFn)(uint32_t hwnd, const char* text, const char* caption, uint32_t type);
static D2cMessageBoxFn g_msg_box;                                   // +0x0FAC68  DAT_6fbaac68
static uint32_t (*g_get_active_window)();                           // +0x0FAC6C  DAT_6fbaac6c
static uint32_t (*g_get_last_active_popup)(uint32_t hwnd);          // +0x0FAC70  DAT_6fbaac70
static uint32_t (*g_get_process_window_station)();                  // +0x0FAC74  DAT_6fbaac74
static int32_t (*g_get_user_object_information)(uint32_t handle, int32_t index, void* info,
                                                uint32_t len, uint32_t* needed);  // +0x0FAC78

struct D2ClientMsgBoxEnv {
    // user32.dll imports; a null member models the GetProcAddress failure retail tests for.
    uint32_t (*load_library)(const char* name);                     // LoadLibraryA("user32.dll")
    D2cMessageBoxFn message_box;                                    // "MessageBoxA"
    uint32_t (*get_active_window)();                                // "GetActiveWindow"
    uint32_t (*get_last_active_popup)(uint32_t hwnd);               // "GetLastActivePopup"
    uint32_t (*get_process_window_station)();                       // "GetProcessWindowStation"
    int32_t (*get_user_object_information)(uint32_t handle, int32_t index, void* info,
                                           uint32_t len, uint32_t* needed);  // "GetUserObjectInformationA"
};

// 1.13c D2Client+0x0BA32 (249 B): resolve user32 once, then pick the owner window and message flags
// exactly as the CRT does. With no window station, or with a visible station (the UOI class bit 0),
// pass GetLastActivePopup(GetActiveWindow()) as the owner; otherwise force MB_TOPMOST (0x40000)
// below version 4 and MB_TASKMODAL (0x200000) from version 4. The user32 lookups are host seams.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_ba32(const char* text, const char* caption, uint32_t type,
                                            const D2ClientMsgBoxEnv* env) {
    uint32_t hwnd = 0;
    if (g_msg_box == nullptr) {
        if (env->load_library("user32.dll") == 0) return 0;
        g_msg_box = env->message_box;
        if (g_msg_box == nullptr) return 0;
        g_get_active_window = env->get_active_window;
        g_get_last_active_popup = env->get_last_active_popup;
        if (g_platform_word == 2) {
            g_get_user_object_information = env->get_user_object_information;
            if (g_get_user_object_information != nullptr)
                g_get_process_window_station = env->get_process_window_station;
        }
    }
    uint8_t info[12] = {};
    uint32_t needed = 0;
    bool visible_station = true;                 // the decompile's `pws == NULL` arm
    if (g_get_process_window_station != nullptr) {
        uint32_t station = g_get_process_window_station();
        visible_station = station != 0
            && g_get_user_object_information != nullptr
            && g_get_user_object_information(station, 1, info, 0xC, &needed) != 0
            && (info[8] & 1) != 0;
    }
    if (visible_station) {
        if (g_get_active_window != nullptr) {
            hwnd = g_get_active_window();
            if (hwnd != 0 && g_get_last_active_popup != nullptr)
                hwnd = g_get_last_active_popup(hwnd);
        }
    } else if (g_win_version < 4) {
        type |= 0x40000;
    } else {
        type |= 0x200000;
    }
    return g_msg_box(hwnd, text, caption, type);
}

// 1.13c D2Client+0x03AC0 (292 B): _strncpy. Copies at most `count` bytes, NUL-padding the tail when
// the source terminates first; returns dst.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2_client_3ac0(char* dst, const char* src, uint32_t count) {
    char* out = dst;
    while (count != 0) {
        char c = *src++;
        *dst++ = c;
        --count;
        if (c == 0) {
            while (count != 0) { *dst++ = 0; --count; }
            break;
        }
    }
    return out;
}

// 1.13c D2Client+0x06FF0 (139 B): _strlen.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_6ff0(const char* s) {
    const char* p = s;
    while (*p != 0) ++p;
    return (uint32_t)(p - s);
}

// 1.13c D2Client+0x07BD0 (232 B): _strcpy -- copy through the terminator, return dst.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2_client_7bd0(char* dst, const char* src) {
    char* out = dst;
    while ((*dst++ = *src++) != 0) {}
    return out;
}
