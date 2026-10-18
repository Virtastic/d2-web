// src/core/d2gfx/d2gfx_batch_535e.cpp -- 1.13c D2Gfx CRT pair, reimplemented from the cached decompile
// (ghidra-d2gfx/decomp/0x535E.c and 0x4262.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector. Host model follows d2gfx_getptd.cpp: callees whose own rows are UNMAPPED are
// seams on a caller-supplied struct; the retail call order and argument values are kept verbatim.
#include <cstdint>

// 0x1A71 __lock, 0x529D ___updatetlocinfo_lk, 0x5390 unlock, 0x608C message write, kernel32 ExitProcess.
struct D2GfxBatch535eHost {
    void  (*lock)(int32_t locknum);
    void* (*updatetlocinfo_lk)(void);
    void  (*unlock)(int32_t locknum);
    void  (*nmsg_write)(int32_t rterrnum);   // 0x608C, retail name unproven (sub_608C)
    void  (*exit_process)(uint32_t code);
};

// 1.13c D2Gfx+0x0535E ___updatetlocinfo (50 bytes): pthreadlocinfo __cdecl ___updatetlocinfo(void).
// Takes the CRT's locale lock (index 0xC), rebuilds the calling thread's locale info via
// ___updatetlocinfo_lk, releases the lock, returns the rebuilt block. Callers: _wctomb (0x423B),
// _atol (0x7231).
// 1.13c D2Gfx+0x0535E
extern "C"
void* d2gfx_updatetlocinfo(const D2GfxBatch535eHost* host) {
    host->lock(0xC);
    void* ptl = host->updatetlocinfo_lk();
    host->unlock(0xC);
    return ptl;
}

// 1.13c D2Gfx+0x04262 report_failure (41 bytes): void __cdecl report_failure(void).
// Writes runtime error message 1, then terminates the process with exit code 3 (the decompile marks
// ExitProcess no-return, so that call ends the path). Caller: __security_check_cookie (0x4293).
// 1.13c D2Gfx+0x04262
extern "C"
void d2gfx_report_failure(const D2GfxBatch535eHost* host) {
    host->nmsg_write(1);
    host->exit_process(3);
}

// 0x28FE __getptd, 0x51CD ___freetlocinfo; the two locale blocks are host state (the port has no retail
// image): 0x1074C holds the process default locale pointer, 0x106F8 is the image's static locale block.
struct D2GfxUpdatetlocinfoLkHost {
    void*    (*getptd)(void);
    void     (*freetlocinfo)(void* ptloci);
    uint32_t default_tlocinfo;   // ds:0x6fa9074c (RVA 0x1074C), the process default locale block
    uint32_t static_tlocinfo;    // 0x6fa906f8 (RVA 0x106F8), static locale, never freed
};

// 1.13c D2Gfx+0x0529D ___updatetlocinfo_lk (193 bytes): int __cdecl ___updatetlocinfo_lk(void).
// ptd = __getptd(); old = ptd->ptlocinfo (+0x64). When old is not the process default, release it: its
// own refcount and the refcounts of its non-null sub-records at +0x2C/+0x34/+0x30/+0x40, then the
// lconv at +0x4C/+0xB4 (unconditional -- the image has no null check there). Install the default, bump
// its refcount and the same non-null sub-records, then +0x4C/+0xB4. Free the old block through
// ___freetlocinfo when it is not the static 0x106F8 block and its refcount reached zero. Return the
// thread's (now default) locale value. Runs under lock 0xC, its only caller 0x535E having taken it.
// 1.13c D2Gfx+0x0529D
extern "C"
uint32_t d2gfx_updatetlocinfo_lk(const D2GfxUpdatetlocinfoLkHost* host) {
    uint8_t* ptd = (uint8_t*)host->getptd();
    uint32_t* old = (uint32_t*)(uintptr_t)*(uint32_t*)(ptd + 0x64);
    uint32_t* def = (uint32_t*)(uintptr_t)host->default_tlocinfo;
    if (old != def) {
        if (old != nullptr) {
            uint32_t* p = (uint32_t*)(uintptr_t)old[0x0B];
            *old -= 1;
            if (p != nullptr) *p -= 1;
            p = (uint32_t*)(uintptr_t)old[0x0D];
            if (p != nullptr) *p -= 1;
            p = (uint32_t*)(uintptr_t)old[0x0C];
            if (p != nullptr) *p -= 1;
            p = (uint32_t*)(uintptr_t)old[0x10];
            if (p != nullptr) *p -= 1;
            *(uint32_t*)(uintptr_t)(old[0x13] + 0xB4) -= 1;
        }
        *(uint32_t*)(ptd + 0x64) = host->default_tlocinfo;
        *def += 1;
        if (def[0x0B] != 0) *(uint32_t*)(uintptr_t)def[0x0B] += 1;
        if (def[0x0D] != 0) *(uint32_t*)(uintptr_t)def[0x0D] += 1;
        if (def[0x0C] != 0) *(uint32_t*)(uintptr_t)def[0x0C] += 1;
        if (def[0x10] != 0) *(uint32_t*)(uintptr_t)def[0x10] += 1;
        *(uint32_t*)(uintptr_t)(def[0x13] + 0xB4) += 1;
        if (old != nullptr && *old == 0 && old != (uint32_t*)(uintptr_t)host->static_tlocinfo)
            host->freetlocinfo(old);
    }
    return *(uint32_t*)(ptd + 0x64);
}
