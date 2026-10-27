// src/core/d2game/d2game_batch_crt_b56.cpp -- twelve UNMAPPED 1.13c D2Game.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order. Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// All twelve are MSVC 2003 (VC7.1) Release CRT leaves linked into the DLL: __close_lk, two 1.13c
// item/composit helpers, __dosmaperr, __lseeki64_lk, __unlock_fhandle, the __unlock(0x10) thunk,
// __get_osfhandle, __heap_alloc, __callnewh, the security-failure report and ___updatetlocinfo_lk.
//
// Host model follows src/core/d2game/d2game_batch_74eb.cpp and d2game_batch_c2150.cpp: retail module
// state arrives as caller-owned pointers, the CRT lock table and critical sections are the compat no-op,
// and callees src/ does not own yet are seams on D2GameBatchB56Env in retail argument order (a seam the
// decompile shows without arguments is taken to have none). Register-only arguments (Ghidra's
// unaff_EDI) are explicit parameters and named at each function. Callees already implemented are called
// through their extern "C" definitions (0x3B93 __getptd family: errno at +0x08, _tdoserrno at +0x0C;
// 0x3F76 __unlock).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION, LeaveCriticalSection: host state, a no-op under wasm

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Host-model pointer fields (the locale block below is host memory, so the slots hold host pointers).
// 1.13c D2Game+0x03A5E: the locale record is the retail 32-bit layout (pointer fields 4 bytes apart), so fields read as 32 bits
static inline void*    ldptr(const void* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }
static inline void     stptr(void* p, uint32_t o, void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

extern "C" void d2game_unlock_3f76(int32_t lockNum);   // 0x3F76 __unlock(lockNum)

// Host seams for the callees and module state the cached decompiles name but src/ does not implement.
// Offsets 0x6FD3A214 (_nfile), 0x6FD3A220 (_pioinfo), 0x6FD3A210 (heap mode), 0x6FD39FC0 (sbh
// threshold), 0x6FD3A20C (heap handle), 0x6FD300D8 (new handler) and 0x6FD301B8 (security handler)
// are the retail globals; their host values are the caller's.
struct D2GameBatchB56Env {
    // 0x5AFE / 0x871C / 0x889A file handles.
    uint32_t  (*getOsfHandle)(int32_t fileHandle);                    // 0x889A __get_osfhandle
    int32_t   (*closeHandle)(uint32_t handle);                        // kernel32 CloseHandle
    uint32_t  (*getLastError)(void);                                  // kernel32 GetLastError
    void      (*freeOsfHandle)(int32_t fileHandle);                   // 0x881B __free_osfhnd
    void      (*dosmaperr)(uint32_t dosError);                        // 0x4E77 __dosmaperr
    uint8_t** pioinfo;                                                // 0x6FD3A220 _pioinfo
    uint32_t  nfile;                                                  // 0x6FD3A214 _nfile
    // 0x4E77 / 0x3A5E thread data.
    void*     (*getptd)(void);                                        // 0x3B93 __getptd
    // 0x871C file kernel.
    uint32_t  (*setFilePointer)(uint32_t handle, int32_t lo, int32_t* hi, uint32_t origin);
    // 0x5ED1 heap.
    int32_t   heapMode;                                               // 0x6FD3A210
    uint32_t  sbhThreshold;                                           // 0x6FD39FC0
    uint32_t  heapHandle;                                             // 0x6FD3A20C
    void      (*lock)(int32_t slot);                                  // 0x402B __lock
    void*     (*sbhAllocBlock)(uint32_t size);                        // 0x93FC ___sbh_alloc_block
    void*     (*heapAlloc)(uint32_t heap, uint32_t flags, uint32_t size); // kernel32 HeapAlloc
    // 0x9710 new handler.
    int32_t   (*newHandler)(uint32_t size);                           // 0x6FD300D8
    // 0x9C16 security failure.
    void      (*securityHandler)(void);                               // 0x6FD301B8
    // 0xE860 / 0xF700 D2Common ordinal imports.
    void*     (*ordinal10695)(uint32_t value);
    void      (*ordinal10887)(void);                                  // no arguments in 0xE860
    void      (*ordinal10887Stat)(void* unit, uint32_t stat, uint32_t value, uint32_t layer);
    uint32_t  (*rollSeed)(uint32_t* seed);                            // 0x1480
    // 0x3A5E locale.
    void      (*freetlocinfo)(void* loc);                             // 0x398E ___freetlocinfo
};

// 1.13c D2Game+0x05AFE
// __close_lk(fh): a valid handle is closed unless it duplicates the handle of 1 on fd 2 (stdout/stderr
// sharing). A failed CloseHandle maps GetLastError through __dosmaperr. Either way __free_osfhnd runs and
// the ioinfo FOPEN byte at _pioinfo[fh>>5] + 4 + (fh&0x1F)*0x24 is cleared; a mapped error returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_5AFE(int32_t fileHandle, const D2GameBatchB56Env* env) {
    uint32_t dwError = 0;
    if (env->getOsfHandle(fileHandle) != 0xFFFFFFFFu) {
        bool closeIt = true;
        if (fileHandle == 1 || fileHandle == 2) {
            const uint32_t h2 = env->getOsfHandle(2);
            const uint32_t h1 = env->getOsfHandle(1);
            if (h1 == h2) closeIt = false;
        }
        if (closeIt && env->closeHandle(env->getOsfHandle(fileHandle)) == 0)
            dwError = env->getLastError();
    }
    env->freeOsfHandle(fileHandle);
    uint8_t* io = env->pioinfo[(int32_t)fileHandle >> 5] + 4 + (fileHandle & 0x1F) * 0x24;
    *io = 0;
    if (dwError == 0) return 0;
    env->dosmaperr(dwError);
    return -1;
}

// 1.13c D2Game+0x0e860
// FUN_6fc2e860(unit in EDI): take the dword at unit+4 (or -1 for a null unit) through D2Common #10695; a
// null result is the retail 0x1DB fatal. Then for each set byte of the result -- 0xFF, 0xFE, 0x102, 0x103
// and 0x101 -- call D2Common #10887 (0x101 twice). The decompile shows that call without arguments.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E860(uint32_t unit, const D2GameBatchB56Env* env) {
    const uint32_t value = unit != 0 ? rd32((const void*)(uintptr_t)unit, 4) : 0xFFFFFFFFu;
    void* pRecord = env->ordinal10695(value);
    if (pRecord == nullptr) d2game_fatal(0x1DB);
    if (rd8(pRecord, 0xFF)  != 0) env->ordinal10887();
    if (rd8(pRecord, 0xFE)  != 0) env->ordinal10887();
    if (rd8(pRecord, 0x102) != 0) env->ordinal10887();
    if (rd8(pRecord, 0x103) != 0) env->ordinal10887();
    if (rd8(pRecord, 0x101) != 0) { env->ordinal10887(); env->ordinal10887(); }
}

// 1.13c D2Game+0x0F700
// FUN_6fc2f700(param_1): the same dword at param_1+4 (or -1) through D2Common #10695; null is the 0x1C9
// fatal. Take the limit at result+0xD0 and a roll over the 64-bit state at param_1+0x20; a roll within the
// limit stores stat 0x1F through D2Common #10887, anything larger is the 0x1CE fatal.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_F700(uint32_t param_1, const D2GameBatchB56Env* env) {
    const uint32_t value = param_1 != 0 ? rd32((const void*)(uintptr_t)param_1, 4) : 0xFFFFFFFFu;
    void* pRecord = env->ordinal10695(value);
    if (pRecord == nullptr) d2game_fatal(0x1C9);
    const uint32_t limit = rd32(pRecord, 0xD0);
    const uint32_t roll = env->rollSeed((uint32_t*)(uintptr_t)(param_1 + 0x20));
    if (roll > limit) d2game_fatal(0x1CE);
    env->ordinal10887Stat((void*)(uintptr_t)param_1, 0x1F, roll, 0);
}

// 1.13c D2Game+0x04e77
// __dosmaperr(osError): record the DOS error in the thread block (+0x0C), then map it to errno (+0x08)
// through the 0x2D-entry table (the MSVC 2003 table at 0x6FD1F5B8; first match wins). The ranges
// 0x13..0x24 and 0xBC..0xCA map to 0x0D and 8; everything else maps to EINVAL (0x16).
static const uint32_t kDosErrorMap[0x2D][2] = {
    { 0x0001, 0x16 }, { 0x0002, 0x02 }, { 0x0003, 0x02 }, { 0x0004, 0x18 }, { 0x0005, 0x0D },
    { 0x0006, 0x09 }, { 0x0007, 0x0C }, { 0x0008, 0x0C }, { 0x0009, 0x0C }, { 0x000A, 0x07 },
    { 0x000B, 0x08 }, { 0x000C, 0x16 }, { 0x000D, 0x16 }, { 0x000F, 0x02 }, { 0x0010, 0x0D },
    { 0x0011, 0x12 }, { 0x0012, 0x02 }, { 0x0021, 0x0D }, { 0x0035, 0x02 }, { 0x0041, 0x0D },
    { 0x0043, 0x02 }, { 0x0050, 0x11 }, { 0x0052, 0x0D }, { 0x0053, 0x0D }, { 0x0057, 0x16 },
    { 0x0059, 0x0B }, { 0x006C, 0x0D }, { 0x006D, 0x20 }, { 0x0070, 0x1C }, { 0x0072, 0x09 },
    { 0x0006, 0x16 }, { 0x0080, 0x0A }, { 0x0081, 0x0A }, { 0x0082, 0x09 }, { 0x0083, 0x16 },
    { 0x0084, 0x0D }, { 0x0091, 0x29 }, { 0x009E, 0x0D }, { 0x00A1, 0x02 }, { 0x00A4, 0x0B },
    { 0x00A7, 0x0D }, { 0x00B7, 0x11 }, { 0x00CE, 0x02 }, { 0x00D7, 0x0B }, { 0x0718, 0x0C },
};

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_4E77(uint32_t dosError, const D2GameBatchB56Env* env) {
    void* ptd = env->getptd();
    wr32(ptd, 0x0C, dosError);
    for (uint32_t i = 0; i < 0x2D; ++i) {
        if (dosError == kDosErrorMap[i][0]) { wr32(ptd, 0x08, kDosErrorMap[i][1]); return; }
    }
    if (dosError > 0x12 && dosError < 0x25) { wr32(ptd, 0x08, 0x0D); return; }
    if (dosError > 0xBB && dosError < 0xCB) { wr32(ptd, 0x08, 0x08); return; }
    wr32(ptd, 0x08, 0x16);
}

// 1.13c D2Game+0x0871C
// __lseeki64_lk(fh, lo, hi, origin): a bad handle sets errno 9 and returns -1. Otherwise
// SetFilePointer(handle, lo, &hi, origin); a -1 result with a nonzero last error maps through
// __dosmaperr and returns -1. In every non-bad-handle path the ioinfo FEOFLAG bit (0x02 at +4) is
// cleared. The 64-bit result is {hi,result}.
extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_d2game_871C(int32_t fileHandle, int32_t lo, int32_t hi,
                                                uint32_t origin, const D2GameBatchB56Env* env) {
    int32_t hiOut = hi;
    uint32_t result;
    const uint32_t handle = env->getOsfHandle(fileHandle);
    if (handle == 0xFFFFFFFFu) {
        wr32(env->getptd(), 0x08, 9);
        result = 0xFFFFFFFFu;
        hiOut = -1;
    } else {
        result = env->setFilePointer(handle, lo, &hiOut, origin);
        if (result == 0xFFFFFFFFu) {
            const uint32_t dwError = env->getLastError();
            if (dwError != 0) {
                env->dosmaperr(dwError);
                result = 0xFFFFFFFFu;
                hiOut = -1;
            }
        }
        uint8_t* io = env->pioinfo[(int32_t)fileHandle >> 5] + 4 + (fileHandle & 0x1F) * 0x24;
        *io &= 0xFD;
    }
    return ((uint64_t)(uint32_t)hiOut << 32) | result;
}

// 1.13c D2Game+0x0897B
// __unlock_fhandle(fh): LeaveCriticalSection on the ioinfo record's section at
// _pioinfo[fh>>5] + 0xC + (fh&0x1F)*0x24. The compat critical section is host state and a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_897B(int32_t fileHandle, const D2GameBatchB56Env* env) {
    LPCRITICAL_SECTION cs = (LPCRITICAL_SECTION)(env->pioinfo[(int32_t)fileHandle >> 5] + 0xC +
                                                 (fileHandle & 0x1F) * 0x24);
    LeaveCriticalSection(cs);
}

// 1.13c D2Game+0x08972
// Nine-byte thunk: __unlock(10).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8972(void) {
    d2game_unlock_3f76(10);
}

// 1.13c D2Game+0x0889a
// __get_osfhandle(fh): a handle below _nfile whose ioinfo FOPEN bit is set returns the OS handle at
// record+0. Anything else sets errno 9, _doserrno 0 and returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_889A(int32_t fileHandle, const D2GameBatchB56Env* env) {
    if ((uint32_t)fileHandle < env->nfile) {
        uint8_t* io = env->pioinfo[(int32_t)fileHandle >> 5] + (fileHandle & 0x1F) * 0x24;
        if ((io[4] & 1) != 0) return rd32(io, 0);
    }
    void* ptd = env->getptd();
    wr32(ptd, 0x08, 9);
    wr32(ptd, 0x0C, 0);
    return 0xFFFFFFFFu;
}

// 1.13c D2Game+0x5ed1 (0x05ED1)
// __heap_alloc(size): the small-block heap (mode 3) takes sizes up to the sbh threshold under __lock(4),
// released by the 0x5F43 __unlock(4) thunk; a null small-block result falls through. A zero size becomes
// one byte, sizes are rounded up to 16 unless the heap is in mode 1, and HeapAlloc(heap, 0, size) is the
// fallback.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_5ED1(uint32_t size, const D2GameBatchB56Env* env) {
    if (env->heapMode == 3 && size <= env->sbhThreshold) {
        env->lock(4);
        void* p = env->sbhAllocBlock(size);
        d2game_unlock_3f76(4);
        if (p != nullptr) return p;
    }
    uint32_t bytes = size == 0 ? 1 : size;
    if (env->heapMode != 1) bytes = (bytes + 0x0F) & 0xFFFFFFF0u;
    return env->heapAlloc(env->heapHandle, 0, bytes);
}

// 1.13c D2Game+0x09710
// __callnewh(size): 1 when an installed new handler accepts the request, 0 otherwise.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_9710(uint32_t size, const D2GameBatchB56Env* env) {
    if (env->newHandler != nullptr && env->newHandler(size) != 0) return 1;
    return 0;
}

// 1.13c D2Game+0x09C16
// Security-failure reporter: an installed handler at 0x6FD301B8 is called with no arguments, otherwise
// the retail code fetches the module name and shows the Buffer-overrun/Unknown-security-failure text in
// a ___crtMessageBoxA box. Both paths end in __exit(3). ponytail: the dialog text and module path are
// host state with no wasm analogue and no protocol byte, so only the handler call and the exit remain --
// add the message assembly if a vector ever needs the box text.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_9C16(int32_t failureKind, const D2GameBatchB56Env* env) {
    (void)failureKind;                       // selects the box text only (1 = overrun, else unknown)
    if (env->securityHandler != nullptr) env->securityHandler();
    _exit(3);
}

// 1.13c D2Game+0x03A5E
// ___updatetlocinfo_lk: take the thread's _tfpecode (thread block +0x64). When it is not the process
// default locale, release the old one -- its own count and the counts of the pointers at +0x2C, +0x34,
// +0x30, +0x40 and the +0xB4 count of the +0x4C object -- install the default, add the same references
// back, and free the old locale when its count reaches zero unless it is the static record at
// 0x6FD1F078. Returns the installed locale. The default's owned tables are the host statics below.
//
// Host model: _tfpecode is a 4-byte token (0 = null, 1 = the process default block, 2 = the static
// block) because the field is 4 bytes and a wasm64 host pointer is 8. The port creates no foreign
// locale, so the release walk only sees those two blocks; their child fields are null and the sub-object
// walks do nothing. The process default's initial refcount is 1, matching the shipped .data record.
// The shipped 1.13c default locale record starts with refcount 1 (the record at 0x6FD1F078).
// Model that initialized .data dword explicitly; __freefls drops each thread's reference from it.
static uint8_t g_defaultTlocinfo[0x90] = { 1 }; // 0x6FD1F0CC process default locale (host model)
static uint8_t g_staticTlocinfo[0x90];       // 0x6FD1F078 static locale never freed

static uint8_t* b56_locale(uint32_t token) {
    if (token == 1) return g_defaultTlocinfo;
    if (token == 2) return g_staticTlocinfo;
    return nullptr;
}

// Share the modeled CRT locale records with __freefls@4. The token is the 32-bit value stored in
// _tiddata+0x64; token 1 and 2 resolve to the same records this module updates in ___updatetlocinfo_lk.
extern "C" void* d2game_crt_locale_from_token(uint32_t token) {
    return b56_locale(token);
}

extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_3A5E(const D2GameBatchB56Env* env) {
    void* ptd = env->getptd();
    const uint32_t token = rd32(ptd, 0x64);                // _tfpecode
    uint8_t* def = g_defaultTlocinfo;
    if (token != 1) {
        uint8_t* loc = b56_locale(token);
        if (loc != nullptr) {
            void* p = ldptr(loc, 0x2C);
            --*(uint32_t*)loc;
            if (p != nullptr) --*(uint32_t*)p;
            p = ldptr(loc, 0x34);
            if (p != nullptr) --*(uint32_t*)p;
            p = ldptr(loc, 0x30);
            if (p != nullptr) --*(uint32_t*)p;
            p = ldptr(loc, 0x40);
            if (p != nullptr) --*(uint32_t*)p;
            p = ldptr(loc, 0x4C);
            if (p != nullptr) --*(uint32_t*)((uint8_t*)p + 0xB4);
        }
        wr32(ptd, 0x64, 1);
        ++*(uint32_t*)def;
        void* p;
        if ((p = ldptr(def, 0x2C)) != nullptr) ++*(uint32_t*)p;
        if ((p = ldptr(def, 0x34)) != nullptr) ++*(uint32_t*)p;
        if ((p = ldptr(def, 0x30)) != nullptr) ++*(uint32_t*)p;
        if ((p = ldptr(def, 0x40)) != nullptr) ++*(uint32_t*)p;
        if ((p = ldptr(def, 0x4C)) != nullptr) ++*(uint32_t*)((uint8_t*)p + 0xB4);
        if (loc != nullptr && *(uint32_t*)loc == 0 && loc != g_staticTlocinfo)
            env->freetlocinfo(loc);
    }
    return b56_locale(rd32(ptd, 0x64));
}

