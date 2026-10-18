// src/core/realm/bnclient_batch_5b47.cpp -- two UNMAPPED 1.13c BNClient.dll alpha-lane rows (Stage A:
// implementation + marker only, no spec, no vector), transcribed from the cached decompiles in
// build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (image base 0x6FF20000).
//
// Rows: __lseek_lk 0x5B47, __lock_fhandle 0x790D. Both are MSVC 2003 (VC7.1) Release CRT leaves.
//
// The ioinfo table DAT_6FF40C00 (an array of block pointers, each block 0x24 bytes: osfhnd 0x0, osfile
// flag 0x4, lockinitflag 0x8, CRITICAL_SECTION 0xC) is module state a host supplies; it arrives as the
// ioinfo_entry seam. __get_osfhandle 0x78CC, __dosmaperr 0x37DF, SetFilePointer/GetLastError and the
// CRT lock slot 10 are seams too: they are unmapped callees of these rows. The rule between them is the
// decompile. Prior art: src/core/realm/bnclient_batch_2558.cpp (__lock, __lseek, ioinfo seams).
#include <cstdint>
#include <emscripten.h>

extern "C" int* bn_errno_ptr(void);   // 0x37CD &__getptd()->_terrno

// The ioinfo record for a handle (retail DAT_6FF40C00[h >> 5] + (h & 0x1F) * 0x24), or NULL.
struct BnIoinfoEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);
    void     (*lock)(int32_t lock_num);                       // 0x3163 __lock
    void     (*unlock)(int32_t lock_num);                     // 0x30AE __unlock
    int32_t  (*init_crit_sec)(void* cs, uint32_t spin_count); // ___crtInitCritSecAndSpinCount
    void     (*enter_critical_section)(void* cs);
};

static uint8_t* bn_no_ioinfo(int32_t) { return nullptr; }
static void bn_no_lock(int32_t) {}
static int32_t bn_no_init_crit_sec(void*, uint32_t) { return 0; }
static void bn_no_enter_critical_section(void*) {}

static const BnIoinfoEnv g_bn_ioinfo_default = {
    bn_no_ioinfo, bn_no_lock, bn_no_lock, bn_no_init_crit_sec, bn_no_enter_critical_section,
};

// Host seams for __lseek_lk (ioinfo entry, get_osfhandle, the file kernel and __dosmaperr).
struct BnLseekLkEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);                              // DAT_6FF40C00 entry
    intptr_t (*get_osfhandle)(int32_t handle);                             // 0x78CC __get_osfhandle
    uint32_t (*set_file_pointer)(void* os_handle, int32_t offset, uint32_t origin); // KERNEL32
    uint32_t (*get_last_error)(void);                                      // KERNEL32
    void     (*dosmaperr)(uint32_t dos_error);                             // 0x37DF __dosmaperr
};

static intptr_t bn_no_osfhandle(int32_t) { return -1; }
static uint32_t bn_no_set_file_pointer(void*, int32_t, uint32_t) { return 0xFFFFFFFF; }
static uint32_t bn_no_last_error(void) { return 0; }
static void bn_no_dosmaperr(uint32_t) {}

static const BnLseekLkEnv g_bn_lseek_lk_default = {
    bn_no_ioinfo, bn_no_osfhandle, bn_no_set_file_pointer, bn_no_last_error, bn_no_dosmaperr,
};

// 1.13c BNClient+0x05B47 (116 B, caller 0x5BBB __lseek): __lseek_lk. Resolve the OS handle through
// __get_osfhandle; -1 means EBADF (9) in _terrno and -1 back. Otherwise SetFilePointer(h, offset, NULL,
// origin); a -1 result that also posted a last error goes through __dosmaperr and returns -1. A clean
// move clears bit 1 (the EOF flag) of the handle's ioinfo osfile byte at +0x4 and returns the new offset.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_lseek_lk(int32_t handle, int32_t offset, uint32_t origin,
                                         const BnLseekLkEnv* env) {
    if (env == nullptr) env = &g_bn_lseek_lk_default;
    const intptr_t os_handle = env->get_osfhandle(handle);
    if (os_handle == -1) {
        *bn_errno_ptr() = 9;   // EBADF
        return -1;
    }
    const uint32_t result = env->set_file_pointer((void*)os_handle, offset, origin);
    const uint32_t dos_error = (result == 0xFFFFFFFF) ? env->get_last_error() : 0;
    if (dos_error == 0) {
        uint8_t* const osfile = env->ioinfo_entry(handle) + 0x4;
        *osfile &= 0xFD;   // clear FEOFLAG
        return (int32_t)result;
    }
    env->dosmaperr(dos_error);
    return -1;
}

// 1.13c BNClient+0x0790D (148 B, callers 0x5BBB, 0x5E22, 0x7D84, 0x7EF7): __lock_fhandle. Take the
// handle's ioinfo record. When its lockinitflag (+0x8) is still zero, take CRT lock 10, initialise the
// record's critical section (4000 spins) if still unset, bump the flag when that succeeded, and release
// lock 10 in every case; an initialisation failure returns 0 without entering the section. Otherwise
// (or after the first pass set the flag) enter the record's section at +0xC and return 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_lock_fhandle(int32_t handle, const BnIoinfoEnv* env) {
    if (env == nullptr) env = &g_bn_ioinfo_default;
    uint8_t* const pio = env->ioinfo_entry(handle);
    int32_t* const lockinitflag = (int32_t*)(pio + 0x8);
    void* const cs = pio + 0xC;
    if (*lockinitflag == 0) {
        env->lock(10);
        int32_t ready = 1;
        if (*lockinitflag == 0) {
            if (env->init_crit_sec(cs, 4000) == 0)
                ready = 0;
            else
                *lockinitflag += 1;
        }
        env->unlock(10);
        if (ready == 0) return 0;
    }
    env->enter_critical_section(cs);
    return 1;
}
