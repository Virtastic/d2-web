// src/core/realm/bnclient_batch_6265.cpp -- twelve UNMAPPED 1.13c BNClient.dll rows reimplemented from
// the cached Ghidra decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane: implementation
// + marker only, no spec, no vector). Base 0x6FF20000; every global is named at its retail RVA.
//
// Rows: ___crtInitCritSecAndSpinCount 0x6265, ___free_lc_time 0x67E5, ___free_lconv_num 0x6975,
// ___free_lconv_mon 0x69D4, __dosmaperr 0x37DF, __get_osfhandle 0x78CC, __unlock_fhandle 0x79AD,
// sub_79A4 0x79A4, __lseeki64_lk 0x79CF, __heap_alloc 0x249D, _memmove 0x7510, report_failure's
// security reporter sub_72CA. All are MSVC 2003 (VC7.1) Release CRT leaves.
//
// Host/module state (the ioinfo handle table, the CRT lock table, the dosmaperr code table, the process
// heap and its mode, the kernel32 entry points, the _ptiddata errno slots) arrives as explicit seam
// arguments after the retail arguments, in retail order (src/core/realm/bnclient_batch_2558.cpp /
// mcp_crt_mem.cpp pattern). The rules between them are the decompile.
// Prior art: src/core/realm/bnclient_batch_2558.cpp (__lock/__unlock, errno slots), bnclient_batch_2544.cpp
// (_free), bnclient_batch_5b47.cpp (ioinfo seams), bnclient_batch_030c3.cpp (___sbh_alloc_block,
// ___crtInitCritSecAndSpinCount model), src/core/realm/mcp_exit.cpp and
// src/core/d2client/d2client_batch_8xx.cpp (the same security-failure reporter).
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION / InitializeCriticalSection: host (kernel) state

extern "C" int*  bn_errno_ptr(void);      // 0x37CD &__getptd()->_terrno
extern "C" int*  bn_get_doserrno(void);   // 0x37D6 &__getptd()->_tdoserrno
extern "C" void  bn_unlock_30ae(int32_t lock_num);   // 0x30AE __unlock(lock_num)

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x06265 (103 B, callers 0x3010/0x30C3/0x408D/0x790D): ___crtInitCritSecAndSpinCount.
// Resolve InitializeCriticalSectionAndSpinCount once into the module slot: on the NT platform word 1 the
// lookup is skipped, otherwise it goes through kernel32 and a null module or export leaves the no-spin
// thunk in the slot. Then call the slot with (cs, spin).
typedef int32_t (*BnInitCritSecSpinFn)(void* cs, uint32_t spin);

struct BnCrtInitCritSecEnv {
    uint32_t os_platform;                                    // +0x1E324 DAT_6ff3e324 (1 = NT)
    uintptr_t (*get_module_handle)(const char* name);        // IAT GetModuleHandleA
    BnInitCritSecSpinFn (*get_proc_address)(uintptr_t mod, const char* name);  // IAT GetProcAddress
};

// 0x6255 ___crtInitCritSecNoSpinCount@8, retail's fallback (its own function): the spin argument is
// ignored, InitializeCriticalSection(cs) and return 1.
static int32_t bn_init_crit_sec_no_spin(void* cs, uint32_t spin) {
    (void)spin;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

static BnInitCritSecSpinFn g_bn_init_crit_sec_spin;   // +0x1E658 DAT_6ff3e658

// 1.13c BNClient+0x06265
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_crt_init_crit_sec_and_spin_count(void* cs, uint32_t spin,
                                                               const BnCrtInitCritSecEnv* env) {
    if (g_bn_init_crit_sec_spin == nullptr) {
        BnInitCritSecSpinFn resolved = nullptr;
        if (env->os_platform != 1) {
            const uintptr_t module = env->get_module_handle("kernel32.dll");
            if (module != 0)
                resolved = env->get_proc_address(module, "InitializeCriticalSectionAndSpinCount");
        }
        g_bn_init_crit_sec_spin = resolved != nullptr ? resolved : bn_init_crit_sec_no_spin;
    }
    g_bn_init_crit_sec_spin(cs, spin);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x067E5 (400 B, caller 0x3601 ___freetlocinfo): ___free_lc_time(lc_time). Free the
// record's 43 owned string pointers in retail's order: slots 1..6, slot 0, then 8..0x2A. A null record
// frees nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_free_lc_time(uint32_t* lc_time, void (*free)(void*)) {
    if (lc_time == nullptr) return;
    for (int i = 1; i <= 6; ++i) free((void*)(uintptr_t)lc_time[i]);
    free((void*)(uintptr_t)lc_time[0]);
    for (int i = 8; i <= 0x2A; ++i) free((void*)(uintptr_t)lc_time[i]);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x06975 (95 B, caller 0x3601 ___freetlocinfo): ___free_lconv_num. Each of the three
// numeric-format fields (+0x0/+0x4/+0x8) is freed unless it is the default lconv entry (+0x24C70 table,
// PTR_PTR_6ff3dacc) or the static string at +0x24A9C/0x24AA0/0x24AA4.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_free_lconv_num(uint32_t* num, const uint32_t* default_lconv,
                                            const uint32_t* static_num, void (*free)(void*)) {
    if (num == nullptr) return;
    for (int i = 0; i < 3; ++i) {
        const uint32_t p = num[i];
        if (p != default_lconv[i] && p != static_num[i]) free((void*)(uintptr_t)p);
    }
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x069D4 (217 B, caller 0x3601 ___freetlocinfo): ___free_lconv_mon. Each of the eight
// monetary-format fields (+0xC..+0x24) is freed unless it is the default lconv entry or the matching
// static string at +0x24AA8..0x24AC0.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_free_lconv_mon(uint32_t* mon, const uint32_t* default_lconv,
                                            const uint32_t* static_mon, void (*free)(void*)) {
    if (mon == nullptr) return;
    for (int i = 3; i <= 10; ++i) {
        const uint32_t p = mon[i];
        if (p != default_lconv[i] && p != static_mon[i - 3]) free((void*)(uintptr_t)p);
    }
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x037DF (115 B, callers 0x1F1F/0x5B47/0x5C66/0x79CF/0x7E74): __dosmaperr. Store the
// DOS error in the thread's _tdoserrno, then find it in the 0x2D-entry (dos, errno) table (+0x1D358) and
// copy the mapped errno into _terrno. Codes 0x13..0x24 map to 0xD, 0xBC..0xCA to 8, everything else to
// 0x16. The table is module image data and arrives as a seam (pairs of dwords).
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_dosmaperr(uint32_t dos_error, const uint32_t* map_table) {
    *bn_get_doserrno() = (int32_t)dos_error;
    for (uint32_t i = 0; i < 0x2D; ++i) {
        if (dos_error == map_table[i * 2]) {
            *bn_errno_ptr() = (int32_t)map_table[i * 2 + 1];
            return;
        }
    }
    if (dos_error > 0x12 && dos_error < 0x25) {          // 0x13..0x24
        *bn_errno_ptr() = 0xD;                            // EACCES
        return;
    }
    if (dos_error > 0xBB && dos_error < 0xCB) {           // 0xBC..0xCA
        *bn_errno_ptr() = 8;                              // ENOEXEC
        return;
    }
    *bn_errno_ptr() = 0x16;                               // EINVAL
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x078CC (65 B, callers 0x5B47/0x79CF/0x7D84/0x7E74): __get_osfhandle. A handle inside
// the ioinfo table whose record has the FOPEN bit (osfile & 1) yields its osfhnd (+0x0); anything else
// sets _terrno = 9 (EBADF), _tdoserrno = 0 and returns -1. The table is a host seam; it answers null for
// a handle outside it.
struct BnOsfHandleEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);   // DAT_6FF40C00 record or null
};

// 1.13c BNClient+0x078CC
extern "C"
EMSCRIPTEN_KEEPALIVE intptr_t bn_get_osfhandle(int32_t handle, const BnOsfHandleEnv* env) {
    uint8_t* const pio = env->ioinfo_entry(handle);
    if (pio != nullptr && (pio[4] & 1) != 0) {
        intptr_t osfhnd;
        std::memcpy(&osfhnd, pio, sizeof osfhnd);
        return osfhnd;
    }
    *bn_errno_ptr() = 9;
    *bn_get_doserrno() = 0;
    return -1;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x079AD (34 B, callers 0x5C42/0x5EA9/0x7E24/0x7F6E): __unlock_fhandle. Leave the
// critical section at record + 0xC for the handle's ioinfo slot.
struct BnUnlockFHandleEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);
    void (*leave_critical_section)(void* cs);
};

// 1.13c BNClient+0x079AD
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_fhandle(int32_t handle, const BnUnlockFHandleEnv* env) {
    env->leave_critical_section(env->ioinfo_entry(handle) + 0xC);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x079A4 (9 B, caller 0x790D __lock_fhandle): the __unlock(0xA) thunk -- leave CRT lock
// slot 10.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock_fhandle_thunk(void) {
    bn_unlock_30ae(10);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x079CF (131 B, caller 0x5C66 __lseeki64_nolock): __lseeki64_lk. Resolve the OS handle;
// -1 sets _terrno = 9 and returns -1/-1 without touching the file. Otherwise SetFilePointer(h, low,
// &high, method); a -1 result with a posted last error goes through __dosmaperr and returns -1/-1.
// Otherwise clear the FEOFLAG bit (osfile & 0xFD) and return (high, result).
struct BnLseekI64LkEnv {
    intptr_t (*get_osfhandle)(int32_t handle);                                  // 0x78CC __get_osfhandle
    uint32_t (*set_file_pointer)(void* os_handle, int32_t low, int32_t* high,
                                 uint32_t method);                              // Kernel32
    uint32_t (*get_last_error)(void);                                           // Kernel32
    void     (*dosmaperr)(uint32_t dos_error);                                  // 0x37DF __dosmaperr
    uint8_t* (*ioinfo_entry)(int32_t handle);                                   // DAT_6FF40C00 record
};

// 1.13c BNClient+0x079CF (returns EDX:EAX, high in EDX)
extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t bn_lseeki64_lk(uint32_t handle, int32_t low, int32_t high,
                                             uint32_t method, const BnLseekI64LkEnv* env) {
    const intptr_t os_handle = env->get_osfhandle((int32_t)handle);
    if (os_handle == -1) {
        *bn_errno_ptr() = 9;                                   // EBADF
        return 0xFFFFFFFFFFFFFFFFull;
    }
    int32_t high_out = high;
    const uint32_t result = env->set_file_pointer((void*)os_handle, low, &high_out, method);
    if (result == 0xFFFFFFFFu) {
        const uint32_t last_error = env->get_last_error();
        if (last_error != 0) {
            env->dosmaperr(last_error);
            return 0xFFFFFFFFFFFFFFFFull;
        }
    }
    env->ioinfo_entry((int32_t)handle)[4] &= 0xFD;             // clear FEOFLAG
    return ((uint64_t)(uint32_t)high_out << 32) | result;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x0249D (111 B, caller 0x2518 __nh_malloc): __heap_alloc. With the small-block heap
// active (mode 3) and the size inside its threshold, take __lock(4), try ___sbh_alloc_block(size),
// __unlock(4) and return a non-null block. Otherwise round the size up (0 -> 1; +0xF & ~0xF unless mode
// is 1) and HeapAlloc it.
struct BnHeapAllocEnv {
    uint32_t heap_mode;                              // +0x20BE4 DAT_6ff40be4
    uint32_t sbh_threshold;                          // +0x20BD0 DAT_6ff40bd0
    void*    heap;                                   // +0x20BE0 DAT_6ff40be0
    void     (*lock)(int32_t lock_num);              // 0x3163 __lock
    uint32_t (*sbh_alloc_block)(uint32_t size);      // 0x51F0 ___sbh_alloc_block
    void     (*unlock4)(void);                       // 0x250F __unlock(4)
    void*    (*heap_alloc)(void* heap, uint32_t flags, size_t size);  // Kernel32 HeapAlloc
};

// 1.13c BNClient+0x0249D
extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_heap_alloc(size_t size, const BnHeapAllocEnv* env) {
    if (env->heap_mode == 3 && size <= env->sbh_threshold) {
        env->lock(4);
        const uint32_t block = env->sbh_alloc_block((uint32_t)size);
        env->unlock4();
        if (block != 0) return (void*)(uintptr_t)block;
    }
    if (size == 0) size = 1;
    if (env->heap_mode != 1) size = (size + 0xF) & ~(size_t)0xF;
    return env->heap_alloc(env->heap, 0, size);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x07510 (672 B, callers 0x4A3C ___sbh_free_block/0x17CFD BuildCatchObject): _memmove.
// The decompile's word-at-a-time blocks with their overlap switch are the compiler's form of memmove:
// an overlapping copy with dst > src runs backwards from the end, everything else forwards. A byte loop
// produces the same bytes and returns dst.
extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_memmove(void* dst, const void* src, size_t size) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    if ((uintptr_t)s < (uintptr_t)d && (uintptr_t)d < (uintptr_t)s + size) {
        for (size_t i = size; i != 0; --i) d[i - 1] = s[i - 1];
    } else {
        for (size_t i = 0; i < size; ++i) d[i] = s[i];
    }
    return dst;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x072CA (322 B, caller 0x3DF8 report_failure): the MSVC 2003 security-failure reporter.
// A non-null installed handler (+0x1E6D8) is called with no arguments and the call returns to the exit;
// otherwise the module path goes into a 0x104-byte buffer ("<program name unknown>" on failure), is
// shortened to a 60-byte window with a "..." tail when strlen + 0xB > 0x3C, and the box text is the
// title + "\n\n" + "Program: " + name + "\n\n" + the long detail text (param 1 selects the buffer-overrun
// pair, anything else the unknown-failure pair), shown with type 0x12010. Both paths __exit(3); the
// following `swi 3` is unreachable.
struct BnReportGsFailureEnv {
    void     (*security_handler)(void);                       // +0x1E6D8 DAT_6ff3e6d8
    uint32_t (*get_module_file_name)(uint32_t mod, char* name, uint32_t size);  // GetModuleFileNameA
    int32_t  (*message_box_a)(const char* text, const char* caption, uint32_t type);  // 0x7415
    void     (*exit_)(uint32_t code);                         // 0x157A __exit
};

// 1.13c BNClient+0x072CA
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_report_gsfailure(int32_t code, const BnReportGsFailureEnv* env) {
    if (env->security_handler != nullptr) {
        env->security_handler();
    } else {
        const char* const title = code == 1 ? "Buffer overrun detected!"
                                            : "Unknown security failure detected!";
        const char* const detail = code == 1
            ? "A buffer overrun has been detected which has corrupted the program's\n"
              "internal state.  The program cannot safely continue execution and must\n"
              "now be terminated.\n"
            : "A security error of unknown cause has been detected which has\n"
              "corrupted the program's internal state.  The program cannot safely\n"
              "continue execution and must now be terminated.\n";
        char name[0x104];
        name[0] = '\0';
        if (env->get_module_file_name(0, name, 0x104) == 0)
            std::strcpy(name, "<program name unknown>");
        const char* shown = name;
        const size_t len = std::strlen(name);
        if (len + 0xB > 0x3C) {
            std::strncpy(name + len - 0x31, "...", 3);
            shown = name + len - 0x31;
        }
        char text[0x300];                       // the retail alloca, bounded
        std::strcpy(text, title);
        std::strcat(text, "\n\n");
        std::strcat(text, "Program: ");
        std::strcat(text, shown);
        std::strcat(text, "\n\n");
        std::strcat(text, detail);
        env->message_box_a(text, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
    env->exit_(3);
}
