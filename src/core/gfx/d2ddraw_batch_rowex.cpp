// src/core/gfx/d2ddraw_batch_rowex.cpp -- UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's and every module global is named at its retail RVA.
//
// All ten rows are MSVC 2003 (VC7.1) Release CRT glue the linker pulled into the DLL:
// the atexit table and its locks, the __initterm/libcmt initialiser walkers and doexit. They have no D2
// caller in our engine, so what is modelled is the retail contract against our own module state: the
// onexit table (0x10610/0x1060C), the lock table (0xE038, 8-byte record per lock) and the four initialiser
// tables (0xE000/0xE00C and the two empty exit tables at 0xE01C/0xE024). Kernel32 objects (critical
// sections, ExitProcess) are host seams -- the same shape as src/core/d2client/d2client_batch_04860.cpp.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <malloc.h>    // malloc_usable_size: the port's allocator size, retail's __msize
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host (kernel) state, a no-op under wasm

typedef void (*D2DDrawExitFn)(void);
typedef int  (*D2DDrawInitFn)(void);     // __xi/__xc initialisers return an int status

extern "C" {
void d2ddraw_lock8(void);
void d2ddraw_unlock8(void);
int  d2ddraw_onexitinit(void);
void d2ddraw_amsg_exit(int code);
void d2ddraw_exit(int code);
size_t d2ddraw_msize(void* block);
void* d2ddraw_realloc(void* block, size_t size);
void d2ddraw_sub_1265(uint32_t frame_word, uint32_t edi);
}

// --- retail module state -------------------------------------------------------------------------
static D2DDrawExitFn* g_onexit_begin;    // +0x10610  DAT_6f8d0610: malloc'd table, dword slots
static D2DDrawExitFn* g_onexit_end;      // +0x1060C  DAT_6f8d060c: next free slot
static uint32_t       g_lock_table[0x40];// +0xE038  DAT_6f8ce038: [slot*2] = CRITICAL_SECTION pointer
static uint32_t       g_C_Termination_Done; // +0xEF20
static uint32_t       g_C_Exit_Done;        // +0xEF24
static uint8_t        g_exitflag;           // +0xEF1C (byte store of retcaller)
static uint32_t       g_crt_refcount;       // +0xEF28  DAT_6f8cef28: __CRT_INIT attachment count
static uint32_t       g_msg_mode1;          // +0xEF34  DAT_6f8cef34: __FF_MSGBANNER gate
static uint32_t       g_msg_mode2;          // +0xEF38  DAT_6f8cef38: second gate
static const void*    g_entry_hook;         // +0x10604 DAT_6f8d0604: DllMain-style callback

// Host seams after the retail arguments. Retail's ExitProcess and the CRT's message/heap/thunk bodies
// terminate or are Win32 objects; wasm has no analogue, so each is a callback slot.
static void (*g_host_exit_process)(int);
static void (*g_ff_msgbanner)(void);         // RVA 0x2A44, a row not ported
static void (*g_msg_print)(int);             // RVA 0x28CC, a row not ported
static void (*g_fpmath)(int);                // [0xEBC0] __fpmath, zero in the shipped image
static int  (*g_heap_init)(void), (*g_heap_term)(void);
static int  (*g_mtinit)(void),    (*g_mtterm)(void);
static int  (*g_ioinit)(void),    (*g_ioterm)(void);
static int  (*g_setargv)(void),   (*g_setenvp)(void);
static void (*g_rtc_initialize)(void);
static const char* (*g_command_line)(void);
static const void* (*g_environment)(void);
static int  (*g_9310)(void);                 // RVA 0x9310, a row not ported (DllMain body)

// --- __lock/__unlock at RVA 0x1ACC/0x1A17 (modelled, not this batch) -----------------------------
// Slot record is 8 bytes: [0] = CRITICAL_SECTION* from __mtinitlocknum's malloc(0x18), [1] = debug word.
static uint32_t* d2ddraw_lock_slot(int idx) { return g_lock_table + (size_t)idx * 2; }

void d2ddraw_lock(int idx) {
    uint32_t* slot = d2ddraw_lock_slot(idx);
    if (*slot == 0) {
        void* cs = std::malloc(0x18);
        if (cs == nullptr) { d2ddraw_amsg_exit(0x11); return; }
        InitializeCriticalSection((LPCRITICAL_SECTION)cs);
        *slot = (uint32_t)(uintptr_t)cs;
    }
    EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)*slot);
}

void d2ddraw_unlock(int idx) {
    LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)*d2ddraw_lock_slot(idx));
}

// Shared with the batch-2 rows (RVA 0x1979/0x19C2/0x1A17/0x1A2C): the one CRT lock table and the two
// lock operations over it. Helpers, not rows, so no marker.
extern "C" uint32_t* d2ddraw_lock_table(void) { return g_lock_table; }
extern "C" void d2ddraw_locknum(int idx) { d2ddraw_lock(idx); }
extern "C" void d2ddraw_unlocknum(int idx) { d2ddraw_unlock(idx); }

// --- __initterm at RVA 0x1134 (modelled helper for doexit/cinit) ---------------------------------
static void d2ddraw_initterm_range(D2DDrawExitFn* begin, D2DDrawExitFn* end) {
    for (D2DDrawExitFn* p = begin; p < end; ++p)
        if (*p != nullptr) (*p)();
}

// --- __cinit's four tables, at their retail RVAs -------------------------------------------------
// 0xE00C..0xE018 (__xi): slot 1 is ___onexitinit (RVA 0x1080, implemented above); slot 2 is retail
// RVA 0x435E, a row not ported yet, so it stays NULL. 0xE000..0xE008 (__xc): slot 1 is retail RVA
// 0x55A6, not ported, NULL. The two exit tables 0xE01C..0xE020 and 0xE024..0xE028 are all-zero in the
// image, so both __initterm walks in doexit are real but empty.
static D2DDrawInitFn g_xi[3];            // +0xE00C
static D2DDrawInitFn g_xc[2];            // +0xE000
static D2DDrawExitFn g_xt_a[1] = {nullptr}; // +0xE024, empty in the image
static D2DDrawExitFn g_xt_z[1] = {nullptr}; // +0xE028, empty in the image
static D2DDrawExitFn g_xc_a[1] = {nullptr}; // +0xE01C, empty in the image
static D2DDrawExitFn g_xc_z[1] = {nullptr}; // +0xE020, empty in the image

// RVA 0x1B41, the handler __cinit registers with _atexit: a __initterm walk over an empty table
// (begin == end == 0x6F8CCF40), so it makes no calls. Its own row is not part of this batch.
static void d2ddraw_atexit_walker(void) {}

// Table wiring once, independent of call order: slot [1] of __xi is our ___onexitinit.
static void d2ddraw_tables_init(void) {
    static int done;
    if (done) return;
    done = 1;
    g_xi[1] = d2ddraw_onexitinit;
}

// 1.13c D2DDraw+0x01000 (__onexit_lk): append func to the atexit table. size = __msize(table);
// need = (end - begin) + 4 bytes. If size < need, grow by min(size, 0x800), retrying once with
// size + 0x10; two failures return NULL with the globals untouched. On success begin := new block,
// end := new + (old end - old begin); then [end] = func, end += 4, return func.
extern "C"
EMSCRIPTEN_KEEPALIVE D2DDrawExitFn d2ddraw_onexit_lk(D2DDrawExitFn func) {
    size_t size = d2ddraw_msize(g_onexit_begin);
    size_t need = (size_t)((uintptr_t)g_onexit_end - (uintptr_t)g_onexit_begin) + 4;
    if (size < need) {
        size_t grow = size < 0x800 ? size : 0x800;
        D2DDrawExitFn* p = (D2DDrawExitFn*)d2ddraw_realloc(g_onexit_begin, size + grow);
        if (p == nullptr) {
            p = (D2DDrawExitFn*)d2ddraw_realloc(g_onexit_begin, size + 0x10);
            if (p == nullptr) return nullptr;   // both null: globals untouched
        }
        g_onexit_end = (D2DDrawExitFn*)((uint8_t*)p + ((uint8_t*)g_onexit_end - (uint8_t*)g_onexit_begin));
        g_onexit_begin = p;
    }
    *g_onexit_end++ = func;
    return func;
}

// 1.13c D2DDraw+0x01080 (___onexitinit): table = _malloc(0x80); a null allocation returns 0x18
// (the CRT's "not enough memory" code); otherwise slot 0 = NULL, end = begin, return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_onexitinit(void) {
    D2DDrawExitFn* t = (D2DDrawExitFn*)std::malloc(0x80);
    if (t == nullptr) return 0x18;          // the CRT's "not enough memory" status
    t[0] = nullptr;
    g_onexit_begin = t;
    g_onexit_end = t;
    return 0;
}

// 1.13c D2DDraw+0x010A8 (__onexit): __lock(8); result = __onexit_lk(func); _unlock(8); return result.
extern "C"
EMSCRIPTEN_KEEPALIVE D2DDrawExitFn d2ddraw_onexit(D2DDrawExitFn func) {
    d2ddraw_lock8();
    D2DDrawExitFn r = d2ddraw_onexit_lk(func);
    d2ddraw_unlock8();
    return r;
}

// 1.13c D2DDraw+0x010E0 (_atexit): return (__onexit(func) != NULL) - 1, i.e. 0 on success, -1 on failure.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_atexit(D2DDrawExitFn func) {
    return (d2ddraw_onexit(func) != nullptr) - 1;
}

// 1.13c D2DDraw+0x010F2 (___crtExitProcess): if mscoree.dll is loaded and exports CorExitProcess, call it
// with code; then ExitProcess(code), which never returns. Neither host object exists under wasm, so the
// terminating call is the installed seam; with no seam installed the entry is simply consumed.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_crt_exit_process(int code) {
    if (g_host_exit_process) g_host_exit_process(code);
}

// 1.13c D2DDraw+0x01122: __lock(8).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_lock8(void) { d2ddraw_lock(8); }

// 1.13c D2DDraw+0x0112B: __unlock(8), retail's _unlock fragment at 0x1A17.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_unlock8(void) { d2ddraw_unlock(8); }

// 1.13c D2DDraw+0x01134 (__initterm): walk [begin, end) ascending and call every non-NULL dword with no
// arguments. Retail takes end as its cdecl argument and begin in EAX, so both are parameters here.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_initterm(D2DDrawExitFn* begin, D2DDrawExitFn* end) {
    d2ddraw_initterm_range(begin, end);
}

// 1.13c D2DDraw+0x0114C (__cinit): __fpmath(init) if present; then walk 0xE00C..0xE018 raising the first
// non-zero return as the result (each initialiser is checked before the next call); if all returned 0,
// register the RVA 0x1B41 walker with _atexit and walk 0xE000..0xE008, result 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_cinit(int init) {
    d2ddraw_tables_init();
    if (g_fpmath) g_fpmath(init);
    int result = 0;
    for (D2DDrawInitFn* p = g_xi; p < g_xi + 3; ++p) {
        if (result != 0) return result;
        if (*p != nullptr) result = (*p)();
    }
    if (result == 0) {
        d2ddraw_atexit(d2ddraw_atexit_walker);
        for (D2DDrawInitFn* p = g_xc; p < g_xc + 2; ++p)
            if (*p != nullptr) (*p)();
        result = 0;
    }
    return result;
}

// 1.13c D2DDraw+0x011B6 (doexit): __lock(8); if _C_Exit_Done (+0xEF24) == 1, TerminateProcess(current,
// code) and never return; _C_Termination_Done (+0xEF20) = 1; _exitflag (+0xEF1C) = (byte)retcaller;
// when quick == 0 walk the onexit table from end-4 down to begin (calling non-NULL slots), then
// __initterm(0xE01C, 0xE020); always __initterm(0xE024, 0xE028); drop lock 8; if retcaller != 0 return;
// _C_Exit_Done = 1; ___crtExitProcess(code).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_doexit(uint32_t code, int quick, int retcaller) {
    d2ddraw_lock(8);
    if (g_C_Exit_Done == 1) {           // retail: TerminateProcess(GetCurrentProcess(), code)
        if (g_host_exit_process) g_host_exit_process((int)code);
        return;
    }
    g_C_Termination_Done = 1;
    g_exitflag = (uint8_t)retcaller;
    if (quick == 0) {
        if (g_onexit_begin != nullptr) {
            while (g_onexit_end != g_onexit_begin) {
                --g_onexit_end;
                if (*g_onexit_end != nullptr) (*g_onexit_end)();
            }
        }
        d2ddraw_initterm_range(g_xc_a, g_xc_z);
    }
    d2ddraw_initterm_range(g_xt_a, g_xt_z);
    d2ddraw_sub_1265((uint32_t)retcaller, 0);   // RVA 0x1265: EDI is 0 here, the frame word is param_3
    if (retcaller != 0) return;
    g_C_Exit_Done = 1;
    d2ddraw_crt_exit_process((int)code);
}

// 1.13c D2DDraw+0x01265: SEH epilog helper. If the caller frame's saved flag [EBP+0x10] differs from
// EDI, release lock 8 (0x1A17). In doexit's frame the flag is param_3 (retcaller) and EDI is 0, so the
// lock is dropped exactly when doexit is a non-returning (retcaller == 0) path's counterpart.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_1265(uint32_t frame_word, uint32_t edi) {
    if (frame_word != edi) d2ddraw_unlock(8);
}

// 1.13c D2DDraw+0x01279 (_exit): doexit(code, quick=0, retcaller=0) -- runs the onexit list.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_exit(int code) { d2ddraw_doexit((uint32_t)code, 0, 0); }

// 1.13c D2DDraw+0x0128A (__exit): doexit(code, quick=1, retcaller=0) -- skips the onexit list.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw__exit(uint32_t code) { d2ddraw_doexit(code, 1, 0); }

// 1.13c D2DDraw+0x0129B (__cexit): doexit(0, quick=0, retcaller=1) -- runs the list and returns without
// terminating the process.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_cexit(void) { d2ddraw_doexit(0, 0, 1); }

// 1.13c D2DDraw+0x012AA (__CRT_INIT@12): the CRT attach/detach state machine, param_2 = 0 uninit,
// 1 init, 2 thread attach, 3 thread detach. Init chains heap -> mt -> RTC -> command line/env -> io ->
// argv/envp -> __cinit(0), incrementing the +0xEF28 count only on full success and unwinding the
// subsystems already started on failure; uninit runs __cexit when termination is not already done and
// then ioterm/mtterm/heap_term under the +0xEF28 count guard. Thread attach builds a _ptiddata and
// thread detach frees it; both hand to host thread seams (the port has no Win32 threads to attach).
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_crt_init(uint32_t module, int why) {
    if (why == 1) {
        if (g_heap_init && g_heap_init() != 0) {
            if (g_mtinit && g_mtinit() != 0) {
                if (g_rtc_initialize) g_rtc_initialize();
                if (g_command_line) (void)g_command_line();   // +0xEF2C
                if (g_environment) (void)g_environment();     // +0xEF30
                int io = g_ioinit ? g_ioinit() : 0;
                if (io >= 0) {
                    int av = g_setargv ? g_setargv() : 0;
                    int ev = av >= 0 && g_setenvp ? g_setenvp() : -1;
                    if (av >= 0 && ev >= 0 && d2ddraw_cinit(0) == 0) {
                        ++g_crt_refcount;
                        return 1;
                    }
                }
                if (g_ioterm) g_ioterm();
            }
            if (g_mtterm) g_mtterm();
        }
        if (g_heap_term) g_heap_term();
        return 0;
    }
    if (why == 0) {
        if (g_crt_refcount > 0) {
            --g_crt_refcount;
            if (g_C_Termination_Done == 0) d2ddraw_cexit();
            if (g_ioterm) g_ioterm();
            if (g_mtterm) g_mtterm();
            if (g_heap_term) g_heap_term();
            return 1;
        }
        return 0;
    }
    return 1;   // reasons 2 and 3: thread attach/detach has no wasm object to build
}

// 1.13c D2DDraw+0x0142B (entry, DllMain): reason 0 with no CRT attachment fails; reasons 1/2 first
// call the +0x10604 hook then __CRT_INIT(module, reason), and both they and reason 0/3 then call the
// D2DDraw body at RVA 0x9310. A failed 0x9310 on reason 1 detaches the CRT; reasons 0/3 run
// __CRT_INIT(module, reason) first and then the hook again if it succeeded.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2ddraw_entry(uint32_t module, int reason, uint32_t reserved) {
    int32_t (*hook)(uint32_t, int, uint32_t) = (int32_t (*)(uint32_t, int, uint32_t))g_entry_hook;
    int result = 1;
    if (reason == 0 && g_crt_refcount == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (hook) result = hook(module, reason, reserved);
        if (result == 0 || d2ddraw_crt_init(module, reason) == 0) return 0;
    }
    result = g_9310 ? g_9310() : 1;
    if (reason == 1 && result == 0) d2ddraw_crt_init(module, 0);
    if (reason == 0 || reason == 3) {
        if (d2ddraw_crt_init(module, reason) == 0) result = 0;
        if (result != 0 && hook) result = hook(module, reason, reserved);
    }
    return result;
}

// 1.13c D2DDraw+0x0150F (__amsg_exit): if the +0xEF34 mode is 1, or 0 while +0xEF38 is 1, print the
// CRT banner; print the message for code (RVA 0x28CC); then tail-call __exit(0xFF) through the +0xE030
// slot, which the image points at RVA 0x128A.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_amsg_exit(int code) {
    if (g_msg_mode1 == 1 || (g_msg_mode1 == 0 && g_msg_mode2 == 1)) {
        if (g_ff_msgbanner) g_ff_msgbanner();
    }
    if (g_msg_print) g_msg_print(code);
    d2ddraw__exit(0xFF);
}

// 1.13c D2DDraw+0x01542 (_realloc): NULL block -> malloc(size); size 0 -> free and NULL; otherwise
// resize, preserving min(old, new) bytes. Retail's two branches (the small-block heap at +0x1064E0 == 3
// and HeapReAlloc) are two allocators for the same contract; the port has one heap.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2ddraw_realloc(void* block, size_t size) {
    if (block == nullptr) return std::malloc(size);
    if (size == 0) { std::free(block); return nullptr; }
    return std::realloc(block, size);
}

// 1.13c D2DDraw+0x016AA: __unlock(4), the same fragment as 0x112B for lock number 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_unlock4(void) { d2ddraw_unlock(4); }

// 1.13c D2DDraw+0x016F4 (__msize): bytes usable in an allocation. Retail asks the small-block heap
// (+0x1064E0 == 3) or HeapSize; the port's dlmalloc exposes both through malloc_usable_size.
extern "C"
EMSCRIPTEN_KEEPALIVE size_t d2ddraw_msize(void* block) {
    return malloc_usable_size(block);
}

// --- heap globals (retail RVAs) ------------------------------------------------------------------
// DAT_6f8d04e0 (+0x104E0) is the heap mode, DAT_6f8d04cc (+0x104CC) the sbh threshold and
// DAT_6f8cf1b4 (+0xF1B4) the new-handler mode that _malloc passes to __nh_malloc.
static uint32_t g_heap_mode;        // +0x104E0: 1 = Win32 heap only, 3 = small-block heap enabled
static uint32_t g_sbh_threshold;    // +0x104CC: largest request the small-block heap serves
static int      g_newmode;          // +0x0F1B4: nonzero = __callnewh may be asked to make room
static void*  (*g_sbh_alloc_block)(uint32_t size); // RVA 0x32BF, a row not ported
static int    (*g_callnewh)(uint32_t size);        // RVA 0x2A7D, a row not ported

// 1.13c D2DDraw+0x01761: __unlock(4), the standalone fragment at RVA 0x1A17 with lock 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_1761(void) { d2ddraw_unlock(4); }

// 1.13c D2DDraw+0x017DC: __unlock(4), the release __heap_alloc's small-block path performs after
// ___sbh_alloc_block.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_17DC(void) { d2ddraw_unlock(4); }

// 1.13c D2DDraw+0x0176A (__heap_alloc): when the heap is in mode 3 and the request fits the sbh
// threshold, take it from ___sbh_alloc_block under lock 4 (released by RVA 0x17DC) and return it;
// otherwise serve it from the heap, with 0 mapped to 1 byte and the size rounded up to 16 unless the
// heap is in mode 1. The port's heap is the module allocator (wasm has no Win32 heap object).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2ddraw_heap_alloc(size_t size) {
    if (g_heap_mode == 3 && size <= g_sbh_threshold) {
        d2ddraw_lock(4);
        void* p = g_sbh_alloc_block ? g_sbh_alloc_block((uint32_t)size) : nullptr;
        d2ddraw_sub_17DC();
        if (p != nullptr) return p;
    }
    size_t bytes = size == 0 ? 1 : size;
    if (g_heap_mode != 1) bytes = (bytes + 0xf) & ~(size_t)0xf;
    return std::malloc(bytes);
}

// 1.13c D2DDraw+0x017E5 (__nh_malloc): a size at or above 0xFFFFFFE1 fails outright; otherwise
// __heap_alloc, then, when nhflag is set, retry through __callnewh until it declines. A zero nhflag
// returns the first failure.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2ddraw_nh_malloc(size_t size, int nhflag) {
    if (size < 0xFFFFFFE1u) {
        for (;;) {
            void* p = d2ddraw_heap_alloc(size);
            if (p != nullptr) return p;
            if (nhflag == 0) return nullptr;
            if (g_callnewh == nullptr || g_callnewh((uint32_t)size) == 0) return nullptr;
        }
    }
    return nullptr;
}

// 1.13c D2DDraw+0x01811 (_malloc): __nh_malloc(size, __newmode).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2ddraw_malloc(size_t size) {
    return d2ddraw_nh_malloc(size, g_newmode);
}

// 1.13c D2DDraw+0x09320 (the export at ordinal 10000): return the address of the interface table
// at +0xE918, never NULL. The table is 54 function-pointer slots (retail RVAs 0x8880 .. 0x8870);
// each slot is a row of its own and is filled in by its implementation, so the table starts null and
// the installed slots are the port's.
static void* g_interface_table[54];     // +0xE918
extern "C"
EMSCRIPTEN_KEEPALIVE void** d2ddraw_sub_9320(void) { return g_interface_table; }

// Host-side installation of the CRT seams (used by a bring-up or a later function probe).
extern "C" void d2ddraw_host_seams(void (*exit_process)(int), void (*msgbanner)(void), void (*msg_print)(int),
                                   void (*fpmath)(int), int (*heap_init)(void), int (*heap_term)(void),
                                   int (*mtinit)(void), int (*mtterm)(void), int (*ioinit)(void),
                                   int (*ioterm)(void), int (*setargv)(void), int (*setenvp)(void),
                                   void (*rtc_initialize)(void), const char* (*command_line)(void),
                                   const void* (*environment)(void), int (*dll_body_9310)(void),
                                   const void* entry_hook) {
    g_host_exit_process = exit_process;
    g_ff_msgbanner = msgbanner;
    g_msg_print = msg_print;
    g_fpmath = fpmath;
    g_heap_init = heap_init; g_heap_term = heap_term;
    g_mtinit = mtinit; g_mtterm = mtterm;
    g_ioinit = ioinit; g_ioterm = ioterm;
    g_setargv = setargv; g_setenvp = setenvp;
    g_rtc_initialize = rtc_initialize;
    g_command_line = command_line;
    g_environment = environment;
    g_9310 = dll_body_9310;
    g_entry_hook = entry_hook;
}
