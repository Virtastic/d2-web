// src/core/storm/storm_batch_26e20.cpp -- nine 1.13c Storm.dll records transcribed from the cached
// decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c): the SFile archive/handle list nodes
// (0x26E20, 0x26F30, 0x281F0, 0x266C0), the two SNet config setters (0x265B0, 0x25D70), the
// process teardown (0x27020), file size (0x262D0) and the archive-open wrapper (0x28DA0).
// Stage A batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Every function is the literal retail body: same lock (0x6FC45988), same 32-bit node layout and
// relative-link unlink (the 0x4C20 body, already ours), same error 0x85100072/0x57 stored at
// 0x6FC45388 *and* passed to SetLastError. Callees whose rows are not mapped yet stay seams with
// retail argument order. Win32 thread/event/file calls are not in compat/Windows.h, so the local
// seams below are deterministic no-ops (StormLib replaces Storm; nothing in the engine calls these).
#include <cstdint>
#include <cstring>
#include <Windows.h>

// 0x4C20 relative-list unlink (src/core/fn/storm_leaves_b23.cpp)
void Storm_Sub4C20(uint8_t* node);

namespace {

// ---- retail module state (all zero-initialised unless noted) -----------------------------------
int32_t g_lastError;              // 0x6FC45388  last Storm error, mirrored to SetLastError
CRITICAL_SECTION g_lock;          // 0x6FC45988  guards every field below
uint32_t g_dummyNode[2];          // 0x6FC45968  fallback {link,tag} node for a null argument
uint32_t g_flag4311C;             // 0x6FC4311C  teardown-in-progress flag
uint32_t g_thread43110;           // 0x6FC43110  worker thread handle
uint32_t g_obj43108;              // 0x6FC43108  archive object; +0x28 is its bound handle id
uint32_t g_list45978;             // 0x6FC45978  request list head
uint32_t g_list459CC;             // 0x6FC459CC  handle-record list head
uint32_t g_list4596C;             // 0x6FC4596C  HSFILE list head
uint32_t g_list45984;             // 0x6FC45984  HSARCHIVE list head
uint32_t g_dword42FFC;            // 0x6FC42FFC  value set by 0x265B0, cleared by 0x26F30
uint32_t g_flag43104;             // 0x6FC43104  shutdown-request flag
uint32_t g_event4312C;            // 0x6FC4312C  shutdown event handle
uint32_t g_ptr43120;              // 0x6FC43120  SFILE.CPP scratch buffers
uint32_t g_ptr43118;              // 0x6FC43118
uint32_t g_ptr43128;              // 0x6FC43128
uint32_t g_dword43130;            // 0x6FC43130  archive-open flags (set by 0x25D70)

constexpr const char* kSFileCpp = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";

// ---- unmapped internal callees, retail argument order -----------------------------------------
int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);        // SMemFree
void Storm_Sub3EF0(uint8_t* p);                                         // file-object destructor
void Storm_Sub3FC0(uint8_t* p);                                         // handle-record destructor
void Storm_Sub3E20(uint8_t* p);                                         // archive-object destructor
void Storm_Sub4040(uint8_t* p);                                         // request-record destructor
int32_t* Storm_Sub25DC0(uint8_t* node);                                 // node in ESI
void Storm_Sub27EF0(int32_t a, int32_t b);
int32_t Storm_Sub28960(void* a, uint32_t* b, uint8_t flags, uint32_t* d);
void Storm_Sub2FB30(const char* type, const char* name);                // "handle never released"

uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void wr32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }
uint8_t* at(uint32_t p, uintptr_t off) { return (uint8_t*)(uintptr_t)(p + off); }

// kernel32 seams (compat/Windows.h carries no thread, event or file-size API)
void SetLastError(uint32_t e) { g_lastError = (int32_t)e; }
bool SetEvent(uint32_t) { return true; }
bool CloseHandle(uint32_t) { return true; }
uint32_t WaitForSingleObject(uint32_t, uint32_t) { return 0x102; }      // WAIT_TIMEOUT
uint32_t GetFileSize(uint32_t, uint32_t* hi) { if (hi) *hi = 0; return 0; }

}  // namespace

// 1.13c Storm+0x26E20: unlink the node (or the 0x6FC45968 fallback), drop its +0x140 refcount and
// destroy it through 0x3EF0/SMemFree when that reaches zero; always returns 1.
extern "C"
uint32_t Storm_Sub26E20(uint32_t* param_1) {
    EnterCriticalSection(&g_lock);
    Storm_Sub4C20((uint8_t*)(param_1 ? (void*)param_1 : (void*)g_dummyNode));
    if (param_1 != nullptr) {
        uint32_t* refs = param_1 + 0x50;
        *refs -= 1;
        if (*refs == 0) {
            Storm_Sub3EF0((uint8_t*)param_1);
            Storm_Sub1B960(param_1, "delete", -1);
        }
    }
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x26F30: set the teardown flag, wait out the worker thread, then drain the
// 0x6FC459CC list, unlinking and freeing each node whose +0x4C refcount reaches zero.
extern "C"
uint32_t Storm_Sub26F30(void) {
    EnterCriticalSection(&g_lock);
    g_flag4311C = 1;
    LeaveCriticalSection(&g_lock);
    while (g_thread43110 != 0) {
        if ((g_obj43108 == 0 && (int32_t)g_list45978 < 1) ||
            WaitForSingleObject(g_thread43110, 10) != 0x102) break;
    }
    EnterCriticalSection(&g_lock);
    uint32_t p = g_list459CC;
    if ((int32_t)p > 0) {
        do {
            uint32_t tag = rd32(at(p, 4));
            uint32_t next = ((int32_t)tag < 1) ? 0 : tag;
            Storm_Sub4C20((uint8_t*)(uintptr_t)p);
            uint32_t* refs = (uint32_t*)at(p, 0x4C);
            *refs -= 1;
            if (*refs == 0) {
                Storm_Sub3FC0((uint8_t*)(uintptr_t)p);
                Storm_Sub1B960((void*)(uintptr_t)p, "delete", -1);
            }
            p = next;
        } while (p != 0);
    }
    g_dword42FFC = 0;
    g_flag4311C = 0;
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x281F0: drop every 0x6FC459CC record keyed on param_1: clear the 0x6FC43108+0x28
// binding when it names the record, run 0x27EF0(0, handle), unlink and free on a zero refcount;
// finally clear the archive's +0x130 field. Always returns 1.
extern "C"
uint32_t Storm_Sub281F0(uint32_t param_1) {
    EnterCriticalSection(&g_lock);
    uint32_t p = g_list459CC;
    if ((int32_t)p > 0) {
        do {
            uint32_t tag = rd32(at(p, 4));
            uint32_t next = ((int32_t)tag < 1) ? 0 : tag;
            if (rd32(at(p, 8)) == param_1) {
                uint32_t handle = rd32(at(p, 0x3C));
                if (g_obj43108 != 0 && rd32(at(g_obj43108, 0x28)) != 0 &&
                    rd32(at(g_obj43108, 0x28)) == handle) {
                    wr32(at(g_obj43108, 0x28), 0);
                }
                Storm_Sub27EF0(0, (int32_t)handle);
                Storm_Sub4C20((uint8_t*)(uintptr_t)p);
                uint32_t* refs = (uint32_t*)at(p, 0x4C);
                *refs -= 1;
                if (*refs == 0) {
                    Storm_Sub3FC0((uint8_t*)(uintptr_t)p);
                    Storm_Sub1B960((void*)(uintptr_t)p, "delete", -1);
                }
            }
            p = next;
        } while (p != 0);
    }
    wr32(at(param_1, 0x130), 0);
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x266C0: find the 0x6FC459CC record keyed on param_1. A type-4 (or absent) record is
// error 0x85100072 -> 0; otherwise 1, with +0x24 copied to param_3 and, for types outside {0,1},
// 0x25DC0 run and +0x2C copied to param_2.
extern "C"
uint32_t Storm_Sub266C0(uint32_t param_1, uint32_t* param_2, uint32_t* param_3) {
    uint32_t ok = 0, out2 = 0, out3 = 0;
    EnterCriticalSection(&g_lock);
    uint32_t p = g_list459CC;
    if ((int32_t)p > 0) {
        do {
            if (rd32(at(p, 8)) == param_1) {
                uint32_t type = rd32(at(p, 0x18));
                out3 = rd32(at(p, 0x24));
                if (type != 4) {
                    ok = 1;
                    if (type != 0 && type != 1) {
                        Storm_Sub25DC0((uint8_t*)(uintptr_t)p);
                        out2 = rd32(at(p, 0x2C));
                    }
                }
                break;
            }
            p = rd32(at(p, 4));
        } while ((int32_t)p > 0 && p != 0);
    }
    if (!ok) {
        g_lastError = 0x85100072;
        SetLastError(0x85100072);
    }
    if (param_2 != nullptr) *param_2 = out2;
    if (param_3 != nullptr) *param_3 = out3;
    LeaveCriticalSection(&g_lock);
    return ok;
}

// 1.13c Storm+0x265B0: store the value at 0x6FC42FFC; zero is error 0x57 -> 0.
extern "C"
uint32_t Storm_Sub265B0(uint32_t param_1) {
    if (param_1 == 0) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    g_dword42FFC = param_1;
    return 1;
}

// 1.13c Storm+0x27020: teardown. 0x26F30 first, then signal/close the worker thread and event,
// destroy the 0x6FC43108 object, drain the four lists (destructor + SMemFree each head), and free
// the three SFILE.CPP scratch buffers. Always returns 1.
extern "C"
uint32_t Storm_Sub27020(void) {
    Storm_Sub26F30();
    if (g_thread43110 != 0) {
        g_flag43104 = 1;
        SetEvent(g_event4312C);
        WaitForSingleObject(g_thread43110, 1000);
        if (g_event4312C != 0) {
            CloseHandle(g_event4312C);
            g_event4312C = 0;
        }
        if (g_thread43110 != 0) {
            CloseHandle(g_thread43110);
            g_thread43110 = 0;
        }
    }
    if (g_obj43108 != 0) {
        Storm_Sub4040((uint8_t*)(uintptr_t)g_obj43108);
        Storm_Sub1B960((void*)(uintptr_t)g_obj43108, "delete", -1);
        g_obj43108 = 0;
    }
    while ((int32_t)g_list45978 > 0) {
        uint32_t p = g_list45978;
        Storm_Sub4040((uint8_t*)(uintptr_t)p);
        Storm_Sub1B960((void*)(uintptr_t)p, "delete", -1);
    }
    while ((int32_t)g_list459CC > 0) {
        uint32_t p = g_list459CC;
        Storm_Sub2FB30("AUDIOSTREAM", nullptr);
        Storm_Sub3FC0((uint8_t*)(uintptr_t)p);
        Storm_Sub1B960((void*)(uintptr_t)p, "delete", -1);
    }
    while ((int32_t)g_list4596C > 0) {
        uint32_t p = g_list4596C;
        Storm_Sub2FB30("HSFILE", (const char*)(uintptr_t)(g_list4596C + 2));
        Storm_Sub3EF0((uint8_t*)(uintptr_t)p);
        Storm_Sub1B960((void*)(uintptr_t)p, "delete", -1);
    }
    while ((int32_t)g_list45984 > 0) {
        uint32_t p = g_list45984;
        Storm_Sub2FB30("HSARCHIVE", (const char*)(uintptr_t)(g_list45984 + 2));
        Storm_Sub3E20((uint8_t*)(uintptr_t)p);
        Storm_Sub1B960((void*)(uintptr_t)p, "delete", -1);
    }
    if (g_ptr43120 != 0) {
        Storm_Sub1B960((void*)(uintptr_t)g_ptr43120, kSFileCpp, 0xE94);
        g_ptr43120 = 0;
    }
    if (g_ptr43118 != 0) {
        Storm_Sub1B960((void*)(uintptr_t)g_ptr43118, kSFileCpp, 0xE9A);
        g_ptr43118 = 0;
    }
    if (g_ptr43128 != 0) {
        Storm_Sub1B960((void*)(uintptr_t)g_ptr43128, kSFileCpp, 0xEA0);
        g_ptr43128 = 0;
    }
    return 1;
}

// 1.13c Storm+0x25D70: store the archive-open flags at 0x6FC43130; returns 1.
extern "C"
uint32_t Storm_Sub25D70(uint32_t param_1) {
    g_dword43130 = param_1;
    return 1;
}

// 1.13c Storm+0x262D0: file size. A real handle (+0x10C != -1) goes to GetFileSize outside the
// lock; otherwise the dword at (+0x114)+8. param_2 (high size) is zeroed on entry when given.
extern "C"
uint32_t Storm_Sub262D0(uint32_t param_1, uint32_t* param_2) {
    if (param_2 != nullptr) *param_2 = 0;
    EnterCriticalSection(&g_lock);
    uint32_t handle = rd32(at(param_1, 0x10C));
    if (handle != 0xFFFFFFFF) {
        LeaveCriticalSection(&g_lock);
        return GetFileSize(handle, param_2);
    }
    uint32_t size = rd32(at(rd32(at(param_1, 0x114)), 8));
    LeaveCriticalSection(&g_lock);
    return size;
}

// 1.13c Storm+0x28DA0: build open flags from 0x6FC43130 (bit0/bit1, plus bit0 of the result when
// the HSARCHIVE list is empty and no flags were set) and forward to 0x28960.
extern "C"
void Storm_Sub28DA0(uint32_t* param_1, uint32_t* param_2) {
    uint8_t flags = (g_dword43130 & 1) != 0;
    if (g_dword43130 & 2) flags = (uint8_t)(flags | 2);
    if ((int32_t)g_list45984 < 1 && g_dword43130 == 0) flags = (uint8_t)(flags | 1);
    Storm_Sub28960(nullptr, param_1, flags, param_2);
}
