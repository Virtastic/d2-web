// src/core/d2game/d2game_batch_74eb.cpp -- twelve UNMAPPED 1.13c D2Game.dll rows transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order: the MSVC 2003
// (VC7.1) Release CRT leaves the linker pulled into the DLL -- report_failure, ___updatetlocinfo,
// __getptd, ___wctomb_mt, strcat, ___crtMessageBoxA, the SEH abnormal-termination stub, the no-spin
// critical-section init, __sopen and the two __lock_file2/__unlock_file2 pair plus __unlock(1). Alpha
// batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Host model follows src/core/d2game/d2game_batch_7de3.cpp and d2game_crt_b23.cpp: retail module
// globals are the statics below (retail RVA in the comment), the CRT lock table is the compat no-op,
// and callees src/ does not own yet are seam pointers on an env struct passed last, in retail argument
// order. Register-only arguments (Ghidra's in_EAX / unaff_EBP) are explicit parameters and named at
// each function. Callees already implemented are called through their extern "C" definitions (0x3B51,
// 0x3F76, 0x4E65). SEH prolog/epilog and the frame scratch they manage are compiler injection, not
// logic, so they are not seams.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION, Enter/Leave/InitializeCriticalSection

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t  v) { std::memcpy((uint8_t*)p + o, &v, 1); }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EBP, in_EAX, ...).
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { wr32((void*)(uintptr_t)a, o, v); }

// Defined elsewhere in src/core/d2game/ (marker there).
extern "C" void     d2_fn_d2game_3b51(void);              // 0x3B51 __unlock(0xC)
extern "C" void     d2_fn_d2game_3f76(int32_t lockNum);   // 0x3F76 __unlock
extern "C" int32_t* d2_fn_d2game_4e65(void);              // 0x4E65 __getptd()->_terrno

// ---- 0x74EB report_failure -----------------------------------------------------------------------
struct D2GameBatch74ebEnv {
    void (*sub9C16)(int32_t);   // 0x9C16 _invoke_watson-style handler
};

// _report_failure: call 0x9C16(1), then ExitProcess(3). ExitProcess is the host _exit, whose only
// observable is the exit code 3.
// 1.13c D2Game+0x074EB
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_74eb(const D2GameBatch74ebEnv* env) {
    env->sub9C16(1);
    _exit(3);
}

// ---- 0x3B1F ___updatetlocinfo --------------------------------------------------------------------
struct D2GameBatch3b1fEnv {
    void  (*lock)(int32_t);        // 0x402B __lock
    void* (*updatetlocinfoLk)(void);   // 0x3A5E ___updatetlocinfo_lk
};

// __lock(0xC), install the new locale through ___updatetlocinfo_lk, release lock 0xC (0x3B51), and
// return the locale pointer the locked update produced.
// 1.13c D2Game+0x03B1F
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_3b1f(const D2GameBatch3b1fEnv* env) {
    env->lock(0xC);
    void* result = env->updatetlocinfoLk();
    d2_fn_d2game_3b51();
    return result;
}

// ---- 0x3B93 __getptd -----------------------------------------------------------------------------
// The thread data block is 0x8C bytes: _initaddr at +0x54 (retail 0x6FD1F760, stored as RVA 0xFF760),
// _holdrand +0x14 = 1, _thandle +0x04 = -1, _tid +0x00 = GetCurrentThreadId(). The TLS slot is retail
// 0x6FD1F204; the host is single-threaded, so one static stands in.
struct D2GameBatch3b93Env {
    uint32_t (*getLastError)(void);
    void     (*setLastError)(uint32_t);
    uint32_t (*getCurrentThreadId)(void);
    void*    (*tlsGetValue)(uint32_t index);
    int32_t  (*tlsSetValue)(uint32_t index, void* value);
    void*    (*callocCrt)(uint32_t count, uint32_t size);   // 0x6018 __calloc_crt
    void     (*amsgExit)(int32_t code);                     // 0x3820 __amsg_exit
};
static uint32_t g_tlsIndex = 0xFFFFFFFFu;   // 0x6FD1F204 DAT_6fd1f204 (TLS_OUT_OF_INDEXES)

// __getptd: return the TLS block. A missing block is allocated (1, 0x8C) and registered; its initial
// _initaddr, _holdrand, _thandle and _tid are set. Allocation or registration failure aborts through
// __amsg_exit(0x10). The caller's last-error value is preserved across the whole path.
// 1.13c D2Game+0x03B93
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2game_3b93(const D2GameBatch3b93Env* env) {
    const uint32_t dwErrCode = env->getLastError();
    void* ptd = env->tlsGetValue(g_tlsIndex);
    if (ptd == nullptr) {
        ptd = env->callocCrt(1, 0x8C);
        if (ptd != nullptr && env->tlsSetValue(g_tlsIndex, ptd) != 0) {
            wr32(ptd, 0x54, 0xFF760u);       // _initaddr, retail 0x6FD1F760
            wr32(ptd, 0x14, 1);              // _holdrand
            const uint32_t tid = env->getCurrentThreadId();
            wr32(ptd, 0x04, 0xFFFFFFFFu);    // _thandle
            wr32(ptd, 0x00, tid);            // _tid
        }
        else {
            env->amsgExit(0x10);
        }
    }
    env->setLastError(dwErrCode);
    return ptd;
}

// ---- 0x8224 ___wctomb_mt -------------------------------------------------------------------------
struct D2GameBatch8224Env {
    // WideCharToMultiByte(codepage, flags, &wch, 1, dest, destSize, defaultChar, &usedDefault)
    int32_t (*wideCharToMultiByte)(uint32_t codepage, uint32_t flags, const uint16_t* src,
                                   int32_t count, char* dest, int32_t destSize,
                                   const char* defaultChar, int32_t* usedDefault);
};

// Multibyte encode of one wide character into param_2. A null destination is 0. In the locale block,
// +0x14 == 0 means "no multibyte codepage": a character below 0x100 stores one byte and answers 1.
// Otherwise WideCharToMultiByte runs with the block's codepage (+0x04) and buffer size (+0x28); a
// non-zero length with no default character used is the result. Every other path sets errno 0x2A
// (EILSEQ) and answers -1.
// 1.13c D2Game+0x08224
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_8224(uint32_t localeBlock, char* dest, uint16_t wch,
                                                const D2GameBatch8224Env* env) {
    if (dest == nullptr) return 0;
    if (rd32(localeBlock, 0x14) == 0) {
        if (wch < 0x100) {
            *dest = (char)wch;
            return 1;
        }
    }
    else {
        int32_t usedDefault = 0;
        const int32_t n = env->wideCharToMultiByte(rd32(localeBlock, 0x04), 0, &wch, 1, dest,
                                                   (int32_t)rd32(localeBlock, 0x28), nullptr,
                                                   &usedDefault);
        if (n != 0 && usedDefault == 0) return n;
    }
    *d2_fn_d2game_4e65() = 0x2A;
    return -1;
}

// ---- 0x72C0 strcat ------------------------------------------------------------------------------
// Concatenation: scan `dst` for its terminating NUL -- byte-by-byte until it is word aligned, then one
// word at a time while the 0x7EFEFEFF trick says no zero byte is inside, and pick the byte position the
// zero occupies -- then copy `src` onto that byte with the same word trick, storing the exact 1/2/3/4
// bytes the source word covers. `dst` is returned.
// 1.13c D2Game+0x072C0
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t* d2_fn_d2game_72c0(uint8_t* dst, const uint8_t* src) {
    uint8_t* d = dst;
    for (;;) {
        if (((uintptr_t)d & 3) != 0) {
            const uint32_t v = *(const uint32_t*)d;
            ++d;
            if ((uint8_t)v == 0) { --d; break; }              // LAB_6fc27313: terminator at d-1
            continue;
        }
        uint8_t* word;
        uint32_t v;
        for (;;) {
            word = d;
            d = word + 4;
            v = *(const uint32_t*)word;
            if (((v ^ 0xFFFFFFFFu ^ (v + 0x7EFEFEFFu)) & 0x81010100u) != 0) break;
        }
        if ((uint8_t)v == 0)         { d = word + 0; break; }
        if ((uint8_t)(v >> 8) == 0)  { d = word + 1; break; }
        if ((v & 0x00FF0000u) == 0)  { d = word + 2; break; }
        d = word + 3;                                         // byte 3 is the zero
        break;
    }

    uint32_t* out = (uint32_t*)d;
    for (;;) {
        if (((uintptr_t)src & 3) == 0) {
            for (;;) {
                const uint32_t v = *(const uint32_t*)src;
                src += 4;
                if (((v ^ 0xFFFFFFFFu ^ (v + 0x7EFEFEFFu)) & 0x81010100u) != 0) {
                    if ((uint8_t)v == 0)               { *(uint8_t*)out = (uint8_t)v; return dst; }
                    if ((uint8_t)(v >> 8) == 0)        { *(uint16_t*)out = (uint16_t)v; return dst; }
                    if ((v & 0x00FF0000u) == 0)        { *(uint16_t*)out = (uint16_t)v; *((uint8_t*)out + 2) = 0; return dst; }
                    if ((v & 0xFF000000u) == 0)        { *out = v; return dst; }
                }
                *out++ = v;
            }
        }
        const uint8_t b = *src++;
        if (b == 0) { *(uint8_t*)out = 0; return dst; }
        *(uint8_t*)out = b;
        out = (uint32_t*)((uint8_t*)out + 1);
    }
}

// ---- 0x972B ___crtMessageBoxA --------------------------------------------------------------------
// user32 is resolved once (MessageBoxA, GetActiveWindow, GetLastActivePopup; on platform word 2 also
// GetUserObjectInformationA and GetProcessWindowStation). The box is parented to the last active popup
// of the active window only when the process window station is visible (user-object info index 1, bit
// 0 of the byte at +8); otherwise the type gains MB_TOPMOST-ish 0x40000 below OS version 4 or 0x200000
// from it. MessageBoxA(hwnd, text, caption, type) is the answer; a failed load answers 0.
typedef int32_t (*D2GameMsgBoxFn)(int32_t hwnd, const char* text, const char* caption, uint32_t type);
typedef int32_t (*D2GameNoArgFn)(void);
typedef int32_t (*D2GameHwndFn)(int32_t hwnd);
typedef int32_t (*D2GameUserObjectFn)(int32_t hwnd, int32_t index, void* info, uint32_t len,
                                      uint32_t* needed);

struct D2GameBatch972bEnv {
    void* (*loadLibraryA)(const char* name);                          // IAT LoadLibraryA
    void* (*getProcAddress)(void* module, const char* name);          // IAT GetProcAddress
};
static D2GameMsgBoxFn   g_messageBoxA;          // 0x6FD300E0 DAT_6fd300e0
static D2GameNoArgFn    g_getActiveWindow;      // 0x6FD300E4 DAT_6fd300e4
static D2GameHwndFn     g_getLastActivePopup;   // 0x6FD300E8 DAT_6fd300e8
static D2GameNoArgFn    g_getProcessWindowStation;   // 0x6FD300EC DAT_6fd300ec
static D2GameUserObjectFn g_getUserObjectInformationA;  // 0x6FD300F0 DAT_6fd300f0
static uint32_t g_osPlatform = 2;               // 0x6FD2FD24 DAT_6fd2fd24
static uint32_t g_osVersion;                    // 0x6FD2FD30 DAT_6fd2fd30

// 1.13c D2Game+0x0972B
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_972b(const char* text, const char* caption,
                                               uint32_t type, const D2GameBatch972bEnv* env) {
    int32_t hwnd = 0;
    uint8_t info[0xC] = {};              // local_14 .. local_c: USEROBJECTFLAGS, +8 is the bit byte
    uint32_t needed = 0;                 // local_8
    if (g_messageBoxA == nullptr) {
        void* module = env->loadLibraryA("user32.dll");
        if (module == nullptr) return 0;
        g_messageBoxA = (D2GameMsgBoxFn)env->getProcAddress(module, "MessageBoxA");
        if (g_messageBoxA == nullptr) return 0;
        g_getActiveWindow = (D2GameNoArgFn)env->getProcAddress(module, "GetActiveWindow");
        g_getLastActivePopup = (D2GameHwndFn)env->getProcAddress(module, "GetLastActivePopup");
        if (g_osPlatform == 2) {
            g_getUserObjectInformationA =
                (D2GameUserObjectFn)env->getProcAddress(module, "GetUserObjectInformationA");
            if (g_getUserObjectInformationA != nullptr) {
                g_getProcessWindowStation =
                    (D2GameNoArgFn)env->getProcAddress(module, "GetProcessWindowStation");
            }
        }
    }

    if (g_getProcessWindowStation == nullptr ||
        ((hwnd = g_getProcessWindowStation()) != 0 &&
         ((hwnd = g_getUserObjectInformationA(hwnd, 1, info, 0xC, &needed)) != 0) &&
         (info[8] & 1) != 0)) {
        if (g_getActiveWindow != nullptr && (hwnd = g_getActiveWindow()) != 0 &&
            g_getLastActivePopup != nullptr) {
            hwnd = g_getLastActivePopup(hwnd);
        }
    }
    else if (g_osVersion < 4) {
        type |= 0x40000u;
    }
    else {
        type |= 0x200000u;
    }
    return g_messageBoxA(hwnd, text, caption, type);
}

// ---- 0x78C6 SEH abnormal termination -------------------------------------------------------------
// The abnormal-termination stub of __local_unwind2 (0x7832) records its three retail operands:
// 0x6FD1F9A4 = [EBP+8], 0x6FD1F9A0 = EAX, 0x6FD1F9A8 = EBP.
static uint32_t g_78c6_1f9a4;   // 0x6FD1F9A4 DAT_6fd1f9a4
static uint32_t g_78c6_1f9a0;   // 0x6FD1F9A0 DAT_6fd1f9a0
static uint32_t g_78c6_1f9a8;   // 0x6FD1F9A8 DAT_6fd1f9a8

// 1.13c D2Game+0x078C6
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_78c6(uint32_t eaxValue, uint32_t stackArg, uint32_t ebp) {
    g_78c6_1f9a4 = stackArg;   // *(EBP + 8)
    g_78c6_1f9a0 = eaxValue;   // in_EAX
    g_78c6_1f9a8 = ebp;        // unaff_EBP
}

// ---- 0x7DD3 ___crtInitCritSecNoSpinCount@8 ------------------------------------------------------
// 1.13c D2Game+0x07DD3
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_7dd3(void* cs) {
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

// ---- 0x86C7 __sopen ------------------------------------------------------------------------------
struct D2GameBatch86c7Env {
    // 0x83E0 __tsopen_lk(share flag as `this`, two out slots, name, open flags, mode byte)
    uint32_t (*tsopenLk)(void* share, void* outA, void* outB, const char* filename,
                         uint32_t openFlag, uint8_t mode);
    void     (*sub870C)(void);   // 0x870C: unlock the frame's handle when its flag is set
};

// __sopen(name, openFlags, shareFlags, mode): the share value is passed as __tsopen_lk's `this` and
// the two out slots are this frame's locals; the returned handle/length is the result. The cleanup
// tail (0x870C) then releases the handle the call registered.
// 1.13c D2Game+0x086C7
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_86c7(const char* filename, int32_t openFlag,
                                               int32_t shareFlag, int32_t mode,
                                               const D2GameBatch86c7Env* env) {
    uint32_t local_24[6] = {};          // out slots for __tsopen_lk
    local_24[1] = 0;
    uint32_t local_8 = 0;
    const uint32_t result = env->tsopenLk((void*)(intptr_t)shareFlag, &local_24[1], &local_24[0],
                                          filename, (uint32_t)openFlag, (uint8_t)mode);
    local_8 = 0xFFFFFFFFu;
    env->sub870C();
    (void)local_8;
    return (int32_t)result;
}

// ---- 0x4C88 / 0x4CDA __lock_file2 / __unlock_file2 ------------------------------------------------
struct D2GameBatch4c88Env {
    void (*lock)(int32_t);   // 0x402B __lock
};

// Index below 0x14 takes CRT lock index+0x10; any other stream enters the critical section at +0x20.
// 1.13c D2Game+0x04C88
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_4c88(int32_t index, void* file,
                                            const D2GameBatch4c88Env* env) {
    if (index < 0x14) {
        env->lock(index + 0x10);
        return;
    }
    EnterCriticalSection((LPCRITICAL_SECTION)((uint8_t*)file + 0x20));
}

// Index below 0x14 releases CRT lock index+0x10; any other stream leaves the section at +0x20.
// 1.13c D2Game+0x04CDA
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_4cda(int32_t index, void* file) {
    if (index < 0x14) {
        d2_fn_d2game_3f76(index + 0x10);
        return;
    }
    LeaveCriticalSection((LPCRITICAL_SECTION)((uint8_t*)file + 0x20));
}

// 1.13c D2Game+0x05003: __unlock(1), the stream-table release.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5003(void) {
    d2_fn_d2game_3f76(1);
}
