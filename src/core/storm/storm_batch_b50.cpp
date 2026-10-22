// src/core/storm/storm_batch_b50.cpp -- Storm.dll 0x279F0 and 0x27A70, transcribed from the cached
// decompiles. Stage A batch lane: implementation + marker only. Register-passed values the
// decompiler shows as unaff_EBX are explicit trailing parameters. 0x04290, 0x26ED0 and 0x28E80 are
// skipped: no decompile / unknown callee register conventions.
#include <cstdint>
#include <Windows.h>

namespace {
CRITICAL_SECTION g_lock;      // 0x6FC45988
uint32_t g_list45978;         // 0x6FC45978 request list head (32-bit node addresses)
uint32_t g_list45970;         // 0x6FC45970 request node pool/list
uint32_t g_event4312C;        // 0x6FC4312C
int32_t g_lastError;          // 0x6FC45388
uint8_t* at(uint32_t p, uintptr_t off) { return (uint8_t*)(uintptr_t)(p + off); }
uint32_t& u32(uint32_t p, uintptr_t off) { return *(uint32_t*)at(p, off); }

// Win32 not in compat/Windows.h: deterministic no-op seams (StormLib replaces Storm)
void StormSetLastError(uint32_t) {}
void SetEvent(HANDLE) {}
void Storm_Sub271D0();                                              // wake/dispatch
uint32_t* Storm_Sub4150(void* list, uint32_t a, uint32_t b);        // node alloc
}

// 1.13c Storm+0x279F0
void Storm_Sub279F0(int32_t param_1, int32_t newVal /*EBX*/) {
    if (param_1 == 0) return;
    EnterCriticalSection(&g_lock);
    uint32_t uVar3 = 0;
    for (uint32_t n = ((int32_t)g_list45978 < 1) - 1 & g_list45978; 0 < (int32_t)n; n = u32(n, 4)) {
        if ((int32_t)u32(n, 0x24) == param_1) {
            int32_t old = (int32_t)u32(n, 0x1c);
            bool had = uVar3 != 0;
            u32(n, 0x1c) = newVal;
            uVar3 = n;
            if (had && 0 < newVal && old < 1) Storm_Sub271D0();
        }
    }
    LeaveCriticalSection(&g_lock);
}

// 1.13c Storm+0x27A70
uint32_t* Storm_Sub27A70(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6,
                         uint32_t p7, int32_t p8, uint32_t p9, uint32_t p10, uint32_t p11, int32_t p12,
                         uint32_t p13, uint32_t ebx) {
    if (p2 == 0 && p4 == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return nullptr;
    }
    EnterCriticalSection(&g_lock);
    uint32_t* n = Storm_Sub4150(&g_list45970, 0, 0);
    DWORD tick = GetTickCount();
    n[2] = p10;
    n[3] = ebx;
    n[4] = p1;
    if ((int32_t)u32(ebx, 0x10c) == -1) p1 = (p1 >> 1) + *(uint32_t*)(uintptr_t)u32(ebx, 0x114);
    n[5] = p1;
    n[6] = tick + p8;
    n[7] = p9;
    n[8] = p2;
    n[9] = p3;
    n[10] = p4;
    n[0xb] = p5;
    n[0xd] = p7;
    n[0xc] = p6;
    n[0xe] = p11;
    n[0x10] = p13;
    u32(ebx, 0x140) += 1;
    if (p6 != 0) u32(p6, 0x4c) += 1;
    LeaveCriticalSection(&g_lock);
    if (p12 != 0) SetEvent((HANDLE)(uintptr_t)g_event4312C);
    return n;
}
