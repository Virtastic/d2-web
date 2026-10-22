// src/core/storm/storm_batch_29be0.cpp -- eight 1.13c Storm.dll SFile exports transcribed from the
// cached decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c): SFileReadFile (0x29BE0),
// SFileSetBasePath (0x26190), SFileSetFilePointer (0x26030), SFileSetIoErrorMode (0x25D30),
// SFileGetFileName (0x26330), SFileLoadFile (0x29D90), SFileUnloadFile (0x26C40) and SFileLoadFileEx
// (0x29D60). Stage A batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Every function is the literal retail body: same lock (0x6FC45988), same 0x57/0xA1 error stored at
// 0x6FC45388 *and* passed to SetLastError, same 32-bit handle layout. Callees whose rows are not
// mapped yet stay seams with retail argument order. Win32 file calls are not in compat/Windows.h, so
// the local seam below is a deterministic no-op (StormLib replaces Storm; nothing in the engine calls
// these), same policy as storm_batch_26e20.cpp.
#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace {

// ---- retail module state (all zero-initialised unless noted) -----------------------------------
int32_t g_lastError;              // 0x6FC45388  last Storm error, mirrored to SetLastError
CRITICAL_SECTION g_lock;          // 0x6FC45988  guards the base path and the cached read block
char g_basePath[0x104];           // 0x6FC43000  SFileSetBasePath output, NUL-terminated
const char g_sep[] = "\\";        // 0x6FC370D4  base-path separator
uint32_t g_ioErrorMode = 2;       // 0x6FC41A04  SFileSetIoErrorMode value (retail .data = 2)
uint32_t g_ioErrorCallback;       // 0x6FC4310C  SFileSetIoErrorMode callback
uint32_t g_dword43130;            // 0x6FC43130  archive-open flags (set by 0x25D70)
uint32_t g_list45984;             // 0x6FC45984  HSARCHIVE list head

// ---- unmapped internal callees, retail argument order -----------------------------------------
uint32_t Storm_Sub3AFD0(const char* s);                             // 0x3AFD0 strlen
void Storm_Sub3B080(char* dst, const char* src, uint32_t n);        // 0x3B080 strncpy
void Storm_Sub3AE00(char* dst, const char* src, uint32_t cap);      // 0x3AE00 append, cap = buffer
uint32_t Storm_Sub295B0(uint32_t h, uint32_t* buf, uint32_t n, int32_t* read, int32_t ov,
                        uint32_t a, uint32_t b);                    // 0x295B0 read core
bool Storm_Sub29C10(void* a, uint32_t* b, int32_t* outPtr, uint32_t* outSize, uint32_t extra,
                    uint32_t flags, int32_t param7, uint32_t param8);  // 0x29C10 load core
int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);    // 0x1B960 SMemFree

uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void wr32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }

// kernel32 seams (compat/Windows.h carries no file-pointer API)
void StormSetLastError(uint32_t e) { g_lastError = (int32_t)e; }
uint32_t SetFilePointer(HANDLE, int32_t, int32_t*, uint32_t) { return 0; }

}  // namespace

// 1.13c Storm+0x29BE0: SFileReadFile -- forward to the 0x295B0 read core with both optional words
// zero. Retail leaves the core's EAX as the export's return, so it is returned here too.
extern "C"
uint32_t Storm_Sub29BE0(uint32_t param_1, uint32_t* param_2, uint32_t param_3, int32_t* param_4,
                        int32_t param_5) {
    return Storm_Sub295B0(param_1, param_2, param_3, param_4, param_5, 0, 0);
}

// 1.13c Storm+0x26190: SFileSetBasePath -- null is 0x57, empty clears the path, a path too long for
// 0x104 bytes (reserving one for the separator when the last char is not '\') is 0xA1. Otherwise
// copy it under the lock and append the separator when missing.
extern "C"
uint32_t Storm_Sub26190(uint32_t* param_1) {
    if (param_1 == nullptr) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    if ((char)*param_1 == '\0') {
        wr32(g_basePath, 0);
        return 1;
    }
    uint32_t len = Storm_Sub3AFD0((const char*)param_1);
    char last = ((const char*)param_1)[len - 1];
    if (len + (last != '\\') + 1 > 0x104) {
        g_lastError = 0xa1;
        StormSetLastError(0xa1);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    Storm_Sub3B080(g_basePath, (const char*)param_1, 0x104);
    if (last != '\\') Storm_Sub3AE00(g_basePath, g_sep, 0x104);
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x26030: SFileSetFilePointer -- a non-zero high word is 0x57. A real OS handle goes
// straight to SetFilePointer outside the lock. An archive file adjusts the cached position (+0x11C)
// for SEEK_CUR (1) / SEEK_END (2) or sets it for SEEK_SET (0), clamps it to size-1, and drops the
// cached read block (+0x138/+0x13C).
extern "C"
uint32_t Storm_Sub26030(int32_t param_1, int32_t param_2, int32_t* param_3, uint32_t param_4) {
    uint8_t* h = (uint8_t*)(uintptr_t)param_1;
    if (param_3 != nullptr && *param_3 != 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0xFFFFFFFFu;
    }
    EnterCriticalSection(&g_lock);
    if ((int32_t)rd32(h + 0x10C) != -1) {
        LeaveCriticalSection(&g_lock);
        return SetFilePointer((HANDLE)(uintptr_t)rd32(h + 0x10C), param_2, nullptr, param_4);
    }
    uint32_t size = rd32((uint8_t*)(uintptr_t)(rd32(h + 0x114) + 8));
    if (param_4 != 0) {
        if (param_4 == 1) {
            uint32_t pos = rd32(h + 0x11C);
            if (param_2 < 0 && pos < (uint32_t)-param_2) wr32(h + 0x11C, 0);
            else wr32(h + 0x11C, pos + (uint32_t)param_2);
            goto clamped;
        }
        if (param_4 != 2) goto clamped;
        if (param_2 < 0 && size < (uint32_t)-param_2) {
            wr32(h + 0x11C, 0);
            goto clamped;
        }
        param_2 = (int32_t)size + param_2;
    }
    wr32(h + 0x11C, (uint32_t)param_2);
clamped:
    uint32_t maxPos = size - 1;
    uint32_t pos = rd32(h + 0x11C);
    if (maxPos <= pos) pos = maxPos;
    wr32(h + 0x11C, pos);
    wr32(h + 0x138, 0);
    wr32(h + 0x13C, 0);
    LeaveCriticalSection(&g_lock);
    return pos;
}

// 1.13c Storm+0x25D30: SFileSetIoErrorMode -- store the mode and callback; always returns 1.
extern "C"
uint32_t Storm_Sub25D30(uint32_t param_1, uint32_t param_2) {
    g_ioErrorMode = param_1;
    g_ioErrorCallback = param_2;
    return 1;
}

// 1.13c Storm+0x26330: SFileGetFileName -- a null buffer is 0x57. Otherwise clear the first byte,
// copy the name at handle+8 (bounded by param_3) under the lock, and return 1.
extern "C"
uint32_t Storm_Sub26330(int32_t param_1, char* param_2, uint32_t param_3) {
    if (param_2 != nullptr) {
        *param_2 = 0;
        EnterCriticalSection(&g_lock);
        Storm_Sub3B080(param_2, (const char*)((uintptr_t)param_1 + 8), param_3);
        LeaveCriticalSection(&g_lock);
        return 1;
    }
    g_lastError = 0x57;
    StormSetLastError(0x57);
    return 0;
}

// 1.13c Storm+0x29D90: SFileLoadFile -- build the open flags from 0x6FC43130 (bit0/bit1, plus bit0
// when the HSARCHIVE list is empty and no flags were set) and forward to the 0x29C10 load core.
// Retail leaves the core's EAX as the export's return, so it is returned here too.
extern "C"
uint32_t Storm_Sub29D90(uint32_t* param_1, int32_t* param_2, uint32_t* param_3, uint32_t param_4,
                        int32_t param_5) {
    uint32_t flags = (g_dword43130 & 1) != 0;
    if (g_dword43130 & 2) flags |= 2;
    if ((g_list45984 < 1 || g_list45984 == 0) && g_dword43130 == 0) flags |= 1;
    return Storm_Sub29C10(nullptr, param_1, param_2, param_3, param_4, flags, param_5, 0);
}

// 1.13c Storm+0x26C40: SFileUnloadFile -- null is 0x57; otherwise SMemFree the handle (0x1B960 with
// SFILE.CPP line 0x1366) and return 1.
extern "C"
uint32_t Storm_Sub26C40(void* param_1) {
    if (param_1 == nullptr) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    Storm_Sub1B960(param_1, "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP", 0x1366);
    return 1;
}

// 1.13c Storm+0x29D60: SFileLoadFileEx -- forward all seven arguments to the 0x29C10 load core with
// the zero word retail passes last. Retail leaves the core's EAX as the export's return.
extern "C"
uint32_t Storm_Sub29D60(void* param_1, uint32_t* param_2, int32_t* param_3, uint32_t* param_4,
                        uint32_t param_5, uint32_t param_6, int32_t param_7) {
    return Storm_Sub29C10(param_1, param_2, param_3, param_4, param_5, param_6, param_7, 0);
}
