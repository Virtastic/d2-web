// src/core/realm/bnclient_batch_2558.cpp -- twelve UNMAPPED 1.13c BNClient.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane, Stage A: implementation +
// marker only, no spec, no vector). Base 0x6FF20000; every global is named at its retail RVA.
//
// Rows: ___crtExitProcess 0x13E2, __initterm 0x1424, __SEH_prolog 0x2558, __SEH_epilog 0x2593,
// __lock 0x3163, ___updatetlocinfo_lk 0x36D1, sub_37C4 0x37C4, sub_37CD 0x37CD, __lseek 0x5BBB,
// __write 0x5E22, __getbuf 0x5ECD, __isatty 0x5F11. All are MSVC 2003 (VC7.1) Release CRT leaves.
//
// Host services retail reaches through imports or through the CRT's own module data (the ioinfo handle
// table, the per-handle lock, the file and console kernels, the runtime message writer, mscoree.dll) are
// seams after the retail arguments, in retail order (src/core/realm/mcp_crt_mem.cpp pattern); the rules
// between them are the decompile. `_ptiddata` is the single block bn_getptd (0x3255) hands out.
// Prior art: the sibling bnclient_batch*.cpp files and src/core/gfx/d2ddraw_batch_10da.cpp (CRT tlocinfo
// bookkeeping), src/core/d2game/d2game_batch_2a6d.cpp (__lock, __SEH_prolog/epilog, ___crtExitProcess,
// __initterm).
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION: host (kernel) state, a no-op under the wasm compat shim

extern "C" void* bn_getptd(void);            // 0x03255 __getptd
extern "C" void* bn_updatetlocinfo(void);    // 0x03792 ___updatetlocinfo, the port's default locale block

// Retail's ExceptionList (FS:[0]). There is no SEH in wasm; the frame walk is modelled against this
// module word so the prolog/epilog pair round-trips (src/core/gfx/d2ddraw_batch2_rowex.cpp pattern).
static void* g_bn_exception_list;             // FS:[0]

// Retail's +0x3D090 CRT lock table: 0x24 slots of 8 bytes, the first dword the critical-section pointer
// (filled by __mtinitlocknum) and the second its static-init flag.
static uint32_t g_bn_crt_lock_table[0x24 * 2];   // DAT_6FF3D090

// printf buffer counter at +0x1E654.
static uint32_t g_bn_buf_count;                  // DAT_6FF3E654

// DAT_6FF3D1C8: a static tlocinfo the release path must never free.
static uint8_t g_bn_static_tlocinfo[0x90];

// The default locale (PTR_DAT_6FF3D21C) is the port's stable zeroed block from bn_updatetlocinfo. Retail
// fills its owned sub-objects with refcounted tables; here they are stable statics so the +1/-1 walks are
// harmless and the identity the callers compare stays the one block.
static int g_bn_locale_ref[4];
static uint8_t g_bn_locale_lconv[0xB8];
static bool g_bn_locale_ready;

static int* bn_ptd_errno(void)    { return (int*)((uint8_t*)bn_getptd() + 0x08); }   // _terrno
static int* bn_ptd_doserrno(void) { return (int*)((uint8_t*)bn_getptd() + 0x0C); }   // _tdoserrno
static int** bn_ptd_tfpecode(void) { return (int**)((uint8_t*)bn_getptd() + 0x64); } // _tfpecode

static int* bn_load_ptr(const int* base, uint32_t off) {
    int* p;
    std::memcpy(&p, (const uint8_t*)base + off, sizeof p);
    return p;
}
static void bn_store_ptr(int* base, uint32_t off, int* p) {
    std::memcpy((uint8_t*)base + off, &p, sizeof p);
}

static void bn_prepare_default_locale(int* def) {
    if (g_bn_locale_ready) return;
    bn_store_ptr(def, 0x2C, &g_bn_locale_ref[0]);
    bn_store_ptr(def, 0x34, &g_bn_locale_ref[1]);
    bn_store_ptr(def, 0x30, &g_bn_locale_ref[2]);
    bn_store_ptr(def, 0x40, &g_bn_locale_ref[3]);
    bn_store_ptr(def, 0x4C, (int*)g_bn_locale_lconv);
    g_bn_locale_ready = true;
}

// The 1.13c FILE (32-bit CRT): ptr 0x0, cnt 0x4, base 0x8, flag 0xC, file 0x10, charbuf 0x14,
// bufsiz 0x18, tmpfname 0x1C (same layout as src/core/realm/bnclient_batch2_crt.cpp).
struct BnCrtFile32 {
    uint32_t ptr;
    int32_t  cnt;
    uint32_t base;
    uint32_t flag;
    int32_t  file;
    uint32_t charbuf;
    int32_t  bufsiz;
    uint32_t tmpfname;
};
static_assert(sizeof(BnCrtFile32) == 32 && __builtin_offsetof(BnCrtFile32, base) == 0x8 &&
              __builtin_offsetof(BnCrtFile32, flag) == 0xC && __builtin_offsetof(BnCrtFile32, charbuf) == 0x14 &&
              __builtin_offsetof(BnCrtFile32, bufsiz) == 0x18, "1.13c MSVC 2003 FILE layout");

// Host seams. A NULL environment pointer selects these defaults.
struct BnCrt2558Env {
    uint8_t  (*ioinfo_flags)(int32_t handle);                              // ioinfo[handle] byte +4
    void     (*lock_fhandle)(int32_t handle);                              // 0x790D __lock_fhandle
    void     (*unlock_fhandle)(int32_t handle);                            // 0x79AD __unlock_fhandle
    int32_t  (*lseek_lk)(int32_t handle, int32_t offset, int32_t origin);  // 0x5B47 __lseek_lk
    int32_t  (*write_lk)(int32_t handle, const void* buf, uint32_t count); // 0x5C66 __write_lk
    void*    (*mt_init_lock_num)(int32_t lock_num);                        // 0x30C3 __mtinitlocknum
    void     (*amsg_exit)(int32_t code);                                   // 0x2242 __amsg_exit
    void     (*freetlocinfo)(void* tlocinfo);                              // 0x3601 ___freetlocinfo
    void*    (*get_module_handle)(const char* name);                       // KERNEL32 GetModuleHandleA
    void*    (*get_proc_address)(void* module, const char* name);          // KERNEL32 GetProcAddress
    void     (*exit_process)(uint32_t code);                               // KERNEL32 ExitProcess
    void     (*initterm_entry)(uint32_t rva);                              // one retail initialiser
};

static uint8_t bn_default_ioinfo(int32_t) { return 0; }
static void bn_default_lock(int32_t) {}
static void bn_default_unlock(int32_t) {}
static int32_t bn_default_lseek(int32_t, int32_t, int32_t) { return -1; }
static int32_t bn_default_write(int32_t, const void*, uint32_t) { return -1; }
static CRITICAL_SECTION g_bn_default_cs[0x24];
static void* bn_default_mtinit(int32_t n) {
    return (n >= 0 && n < 0x24) ? (void*)&g_bn_default_cs[n] : nullptr;
}
static void bn_default_void(int32_t) {}
static void bn_default_free(void*) {}
static void* bn_default_no_module(const char*) { return nullptr; }
static void* bn_default_no_proc(void*, const char*) { return nullptr; }
static void bn_default_exit(uint32_t) {}
static void bn_default_initterm(uint32_t) {}

static const BnCrt2558Env g_bn_crt2558_default = {
    bn_default_ioinfo, bn_default_lock, bn_default_unlock, bn_default_lseek, bn_default_write,
    bn_default_mtinit, bn_default_void, bn_default_free, bn_default_no_module, bn_default_no_proc,
    bn_default_exit, bn_default_initterm,
};

extern "C" void bn_unlock_lock(int32_t lock_num) {
    if (lock_num < 0 || lock_num >= 0x24) return;
    const uint32_t cs = g_bn_crt_lock_table[lock_num * 2];
    if (cs != 0) LeaveCriticalSection((LPCRITICAL_SECTION)(uintptr_t)cs);
}

// 1.13c BNClient+0x013E2 (47 B, caller 0x14A6 _doexit): ___crtExitProcess(code). Look up mscoree.dll;
// when loaded, GetProcAddress("CorExitProcess") and call it on the code. Then ExitProcess(code), which
// never returns. All three imports are host seams; the lookups and the terminal exit are the rule.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_crt_exit_process(int32_t code, const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    void* module = env->get_module_handle("mscoree.dll");
    if (module != nullptr) {
        void* cor_exit = env->get_proc_address(module, "CorExitProcess");
        if (cor_exit != nullptr)
            ((void (*)(int32_t))(uintptr_t)cor_exit)(code);
    }
    env->exit_process((uint32_t)code);
}

// 1.13c BNClient+0x01424 (24 B, caller 0x14A6 _doexit): __initterm(begin, end). The begin pointer arrives
// in EAX and the end is the stack argument; walk [begin, end) in ascending dword order and call every
// non-NULL entry with no arguments. Each entry is a retail initialiser address, hence the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_initterm(const uint32_t* begin, const uint32_t* end, const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    for (const uint32_t* p = begin; p < end; ++p)
        if (*p != 0) env->initterm_entry(*p);
}

// 1.13c BNClient+0x02558 (59 B): __SEH_prolog. frame_size is the local frame (param_2) and frame points
// at auStack_18[0]; the handler (param_1) is registered separately and is not read here. Spill the
// caller's EBX, ESI, EDI and return address at frame - frame_size + 0xC/8/4/0, then point ExceptionList
// at the frame's local_8 slot (frame + 0x10, above auStack_18[4]).
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_seh_prolog(uint32_t handler, int frame_size, uint32_t* frame,
                                        uint32_t ebx, uint32_t esi, uint32_t edi, uint32_t retaddr) {
    (void)handler;
    uint8_t* base = (uint8_t*)frame - (size_t)frame_size;
    *(uint32_t*)(base + 0xC) = ebx;
    *(uint32_t*)(base + 0x8) = esi;
    *(uint32_t*)(base + 0x4) = edi;
    *(uint32_t*)(base + 0x0) = retaddr;
    g_bn_exception_list = (void*)((uint8_t*)frame + 0x10);
}

// 1.13c BNClient+0x02593 (17 B): __SEH_epilog. ExceptionList = EBP[-4] (the next outer frame's list),
// then the return address is restored to [EBP]. Both operands are the caller's frame, hence the seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_seh_epilog(uint32_t* ebp, uint32_t retaddr) {
    g_bn_exception_list = (void*)(uintptr_t)ebp[-4];
    ebp[0] = retaddr;
}

// 1.13c BNClient+0x03163 (49 B): __lock(lock_num). When table[lock_num] is still empty, initialise it
// through __mtinitlocknum; a failure runs __amsg_exit(0x11). Then enter the slot's critical section.
// The initialiser is the host seam and hands back the section; the engine is single-threaded.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_lock(int32_t lock_num, const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    if (lock_num >= 0 && lock_num < 0x24) {
        uint32_t* slot = &g_bn_crt_lock_table[lock_num * 2];
        if (*slot == 0) {
            void* cs = env->mt_init_lock_num(lock_num);
            if (cs == nullptr) env->amsg_exit(0x11);
            *slot = (uint32_t)(uintptr_t)cs;
        }
        EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)*slot);
    }
}

// 1.13c BNClient+0x036D1 (193 B, caller 0x3792 ___updatetlocinfo): ___updatetlocinfo_lk. Take the
// calling thread's _tfpecode; when it is not the process default, release the old locale (its own count
// plus the counts of its sub-objects at +0xB/+0xC/+0xD/+0x10 and the lconv at +0x13/+0xB4), install the
// default and add those references back, then free the old locale at zero unless it is the static
// +0x1C8 record. Return the installed _tfpecode. The default's owned tables are stable statics here.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_updatetlocinfo_lk(const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    int** slot = bn_ptd_tfpecode();
    int* loc = *slot;
    int* def = (int*)(uintptr_t)bn_updatetlocinfo();
    if (loc != def) {
        bn_prepare_default_locale(def);
        if (loc != nullptr) {
            int* p = bn_load_ptr(loc, 0x0B);
            --*loc;
            if (p != nullptr) --*p;
            p = bn_load_ptr(loc, 0x0D);
            if (p != nullptr) --*p;
            p = bn_load_ptr(loc, 0x0C);
            if (p != nullptr) --*p;
            p = bn_load_ptr(loc, 0x10);
            if (p != nullptr) --*p;
            p = bn_load_ptr(loc, 0x13);
            if (p != nullptr) --*(int*)((uint8_t*)p + 0xB4);
        }
        *slot = def;
        ++*def;
        int* p;
        if ((p = bn_load_ptr(def, 0x2C)) != nullptr) ++*p;
        if ((p = bn_load_ptr(def, 0x34)) != nullptr) ++*p;
        if ((p = bn_load_ptr(def, 0x30)) != nullptr) ++*p;
        if ((p = bn_load_ptr(def, 0x40)) != nullptr) ++*p;
        if ((p = bn_load_ptr(def, 0x4C)) != nullptr) ++*(int*)((uint8_t*)p + 0xB4);
        if (loc != nullptr && *loc == 0 && loc != (int*)g_bn_static_tlocinfo)
            env->freetlocinfo(loc);
    }
    return (int32_t)(intptr_t)*slot;
}

// 1.13c BNClient+0x037C4 (9 B, caller 0x3792): __unlock(0xC) -- leave the CRT lock slot's section.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_37c4(void) {
    bn_unlock_lock(0xC);
}

// 1.13c BNClient+0x037CD (9 B): &__getptd()->_terrno, the thread's errno slot (+0x08 in _ptiddata).
extern "C"
EMSCRIPTEN_KEEPALIVE int* bn_errno_ptr(void) {
    return bn_ptd_errno();
}

// 1.13c BNClient+0x05BBB (160 B, caller 0x26AD __flsbuf): __lseek(handle, offset, origin). A handle
// outside the ioinfo table, or one whose flag byte lacks bit 0, sets _terrno = 9 (EBADF) and _tdoserrno = 0
// and returns -1. Otherwise lock the handle, re-check the flag (a handle closed meanwhile takes the same
// error), call __lseek_lk, unlock, and return its result. The table, the per-handle lock and the kernel
// call are seams; the seam returns flag 0 for an out-of-range handle.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_lseek(int32_t handle, int32_t offset, int32_t origin, const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    if ((env->ioinfo_flags(handle) & 1) != 0) {
        env->lock_fhandle(handle);
        int32_t rc;
        if ((env->ioinfo_flags(handle) & 1) == 0) {
            *bn_ptd_errno() = 9;
            *bn_ptd_doserrno() = 0;
            rc = -1;
        } else {
            rc = env->lseek_lk(handle, offset, origin);
        }
        env->unlock_fhandle(handle);
        return rc;
    }
    *bn_ptd_errno() = 9;
    *bn_ptd_doserrno() = 0;
    return -1;
}

// 1.13c BNClient+0x05E22 (160 B, callers 0x26AD __flsbuf, 0x7AED): __write(handle, buf, count). The same
// validation and per-handle lock as __lseek; the locked body is __write_lk(handle, buf, count).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_write(int32_t handle, const void* buf, uint32_t count, const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    if ((env->ioinfo_flags(handle) & 1) != 0) {
        env->lock_fhandle(handle);
        int32_t rc;
        if ((env->ioinfo_flags(handle) & 1) == 0) {
            *bn_ptd_errno() = 9;
            *bn_ptd_doserrno() = 0;
            rc = -1;
        } else {
            rc = env->write_lk(handle, buf, count);
        }
        env->unlock_fhandle(handle);
        return rc;
    }
    *bn_ptd_errno() = 9;
    *bn_ptd_doserrno() = 0;
    return -1;
}

// 1.13c BNClient+0x05ECD (68 B, caller 0x26AD __flsbuf): __getbuf(file). Bump the buffer counter, then
// malloc(0x1000) into _base. On failure flag |= 4 (_IONBF), _base = &_charbuf and _bufsiz = 2; on success
// flag |= 8 (_IOMYBUF) and _bufsiz = 0x1000. Either way _cnt = 0 and _ptr = _base. The allocation is the
// host heap.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_getbuf(BnCrtFile32* file) {
    ++g_bn_buf_count;
    uint8_t* buf = (uint8_t*)std::malloc(0x1000);
    file->base = (uint32_t)(uintptr_t)buf;
    if (buf == nullptr) {
        file->flag |= 4;
        file->base = (uint32_t)(uintptr_t)&file->charbuf;
        file->bufsiz = 2;
    } else {
        file->flag |= 8;
        file->bufsiz = 0x1000;
    }
    file->cnt = 0;
    file->ptr = file->base;
}

// 1.13c BNClient+0x05F11 (42 B, caller 0x26AD __flsbuf): __isatty(handle) = ioinfo[handle] flag byte &
// 0x40. A handle outside the table returns 0; the seam reports flag 0 there.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_isatty(int32_t handle, const BnCrt2558Env* env) {
    if (env == nullptr) env = &g_bn_crt2558_default;
    return (int32_t)(env->ioinfo_flags(handle) & 0x40);
}
