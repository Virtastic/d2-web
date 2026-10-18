// src/core/storm/storm_batch_123b0.cpp -- 12 1.13c Storm.dll exported ordinals (104-117)
// reimplemented from the decompiles in build-wasm/ghidra-storm/decomp/ (0x<RVA>.c).
//
// Storm is the MPQ/network layer; these twelve are the SFile* archive-info family plus two
// SNet read-config exports. Every function is the literal retail body: same lock (0x6FC45A24),
// same error code stored at 0x6FC45388 *and* passed to SetLastError, same early-outs. The
// unmapped internal callees are declared below as seams with their retail argument order; the
// globals keep their retail addresses in the comments.
//
// Two Ghidra artifacts are noted where they occur:
//   * 0x10720 case 4/6 copy a fixed 4-byte value (pcVar2 = 4), not a string;
//   * 0x10720's "*param_4 = extraout_EDX" is the length copied, modelled as min(size, srcLen).
#include <cstdint>
#include <cstring>
#include <Windows.h>

namespace {

// ---- retail module state (all zero-initialised unless noted) -----------------------------------
int32_t g_lastError;              // 0x6FC45388  last Storm error, mirrored to SetLastError
CRITICAL_SECTION g_lock;          // 0x6FC45A24  guards every field below
void* g_smack;                    // 0x6FC45834  SNet/MPQ interface; +0x18/+0x20 are callbacks
uint8_t* g_archive0;              // 0x6FC45840  first archive object (record at +0x214)
uint8_t* g_listHead;              // 0x6FC45A6C  head of the archive list, next at +0x04
intptr_t g_45a50;                 // 0x6FC45A50  list anchor (used as pointer and as count test)
int32_t g_45844;                  // 0x6FC45844  loop-continue flag for 0x16D90
int32_t g_45830;                  // 0x6FC45830  first archive handle/index
uint8_t g_41b4c;                  // 0x6FC41B4C  0xFF = archive set not available
uint8_t g_45240[0x100];           // 0x6FC45240  case-1 config string
uint8_t g_452c0[0x100];           // 0x6FC452C0  case-2 config string
uint8_t g_451b8[0x100];           // 0x6FC451B8  case-3 config string
uint32_t g_45344;                 // 0x6FC45344  case-4 4-byte value
uint32_t g_45350;                 // 0x6FC45350  case-5 length
void* g_45358;                    // 0x6FC45358  case-5 buffer
uint32_t g_45170;                 // 0x6FC45170  case-6 4-byte value
uint32_t g_45354;                 // 0x6FC45354  saved by 0x170B0
uint32_t g_45340;                 // 0x6FC45340  saved by 0x170B0
uint32_t g_4516c;                 // 0x6FC4516C  saved by 0x170B0
uint32_t g_table[0x10][4];        // 0x6FC41A38  per-index {value0,value1,value2,callbackIdx}

// ---- unmapped internal callees, retail argument order -----------------------------------------
int32_t Storm_Sub11C90(uint32_t* a, uint32_t b);
int32_t Storm_SubFF80(void* a);
void Storm_Sub169A0(void);
void Storm_Sub12740(void* a);
int32_t* Storm_Sub12690(void);
void Storm_Sub14F20(void);
int32_t* Storm_SubFEB0(int32_t index);
void Storm_Sub3B080(uint8_t* out, uint8_t* src, int32_t n);
int32_t Storm_Sub11F10(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d, uint32_t* e, uint32_t* f);
void Storm_Sub36020(uint32_t a, uint32_t b);
void Storm_Sub36290(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);
int32_t Storm_Sub161E0(void);
int32_t Storm_Sub16E70(uint8_t* out, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t d);

uint32_t* rd32p(uint8_t* p, uintptr_t off) { return (uint32_t*)(p + off); }
uint32_t rd32(const uint8_t* p, uintptr_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; }

// kernel32 seams: compat/Windows.h carries no TEB. SetLastError only mirrors the error the retail
// bodies already store at 0x6FC45388, and the system clock is deterministic-zero until a platform
// layer supplies one (same policy as compat/Windows.h GetLocalTime).
void SetLastError(uint32_t e) { g_lastError = (int32_t)e; }
void GetSystemTime(SYSTEMTIME* t) { std::memset(t, 0, sizeof(*t)); }
int32_t SystemTimeToFileTime(const SYSTEMTIME*, FILETIME* out) {
    out->dwLowDateTime = 0;
    out->dwHighDateTime = 0;
    return 1;
}

// 1.13c Storm+0x123B0 (ordinal 104): open a file inside an archive; returns 0/1.
// param_3 must be non-null (0x57 otherwise); param_4 (if given) receives 0 on entry.
uint32_t Storm_Sub123B0(uint32_t param_1, uint32_t* param_2, void* param_3, uint32_t* param_4) {
    if (param_3 == nullptr) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    if (param_4 != nullptr) *param_4 = 0;
    if (g_smack == nullptr) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
    } else if (Storm_Sub11C90(param_2, param_1) != 0) {
        LeaveCriticalSection(&g_lock);
        return 1;
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x16D90 (ordinal 105): walk the archive list and invoke the caller's callback.
// Callback signature from the retail call: (field+0x110, name+0x114, blob+0x194, ecx).
typedef int32_t (*Storm_Enum105)(int32_t, const char*, void*, uint32_t);

uint32_t Storm_Sub16D90(int* param_1, Storm_Enum105 param_2) {
    uint32_t uVar4 = (param_1 == nullptr || *param_1 == 0x24) ? 0xFFFFFFFFu : 0u;
    if ((-(uint32_t)(param_2 != nullptr) & uVar4) == 0) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    Storm_Sub169A0();
    intptr_t iVar5 = g_45a50;
    uint8_t* iVar6 = (uint8_t*)g_45a50;
    if (iVar5 > 0) {
        for (;;) {
            int32_t iVar3 = (int32_t)rd32(iVar6, 0x110);
            char* pcVar1 = (char*)(iVar6 + 0x114);
            if (iVar3 != 0 && pcVar1 != nullptr && *pcVar1 != '\0') {
                if (Storm_SubFF80(iVar6 + 0x214) != 0) {
                    iVar3 = param_2(iVar3, pcVar1, iVar6 + 0x194, 0 /* extraout_ECX */);
                    iVar5 = g_45a50;
                    if (iVar3 == 0) iVar6 = nullptr;
                }
            }
            if (g_45844 == 0 || iVar5 < 1 || iVar6 == nullptr) break;
            iVar6 = (uint8_t*)(uintptr_t)rd32(iVar6, 0x04);
            if ((intptr_t)iVar6 < 1 || iVar6 == nullptr) break;
        }
    }
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x10720 (ordinal 107): fetch a named config value (cases 1-6) into a buffer.
// Cases 1-3 are NUL-terminated strings and force dst[size-1]=0; cases 4/6 are 4 raw bytes.
uint32_t Storm_Sub10720(uint32_t param_1, void* param_2, uint32_t param_3, uint32_t* param_4) {
    EnterCriticalSection(&g_lock);
    if (param_2 != nullptr && param_3 != 0) std::memset(param_2, 0, param_3);
    if (param_4 != nullptr) *param_4 = 0;
    if (g_smack == nullptr) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    if (g_41b4c == 0xFF) {
        g_lastError = 0x85100070;
        SetLastError(0x85100070);
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    bool bVar1 = false;
    uint8_t* puVar5 = nullptr;
    uint32_t srcLen = 0;
    switch (param_1) {
        case 1: puVar5 = g_45240; srcLen = (uint32_t)std::strlen((char*)g_45240) + 1; bVar1 = true; break;
        case 2: puVar5 = g_452c0; srcLen = (uint32_t)std::strlen((char*)g_452c0) + 1; bVar1 = true; break;
        case 3: puVar5 = g_451b8; srcLen = (uint32_t)std::strlen((char*)g_451b8) + 1; bVar1 = true; break;
        case 4: puVar5 = (uint8_t*)&g_45344; srcLen = 4; break;
        case 5:
            srcLen = g_45350;
            puVar5 = (uint8_t*)g_45358;
            if (g_45358 == nullptr) {
                g_lastError = 0x57;
                SetLastError(0x57);
                LeaveCriticalSection(&g_lock);
                return 0;
            }
            break;
        case 6: puVar5 = (uint8_t*)&g_45170; srcLen = 4; break;
        default:
            g_lastError = 0x57;
            SetLastError(0x57);
            LeaveCriticalSection(&g_lock);
            return 0;
    }
    if (param_2 == nullptr || param_3 == 0) {
        if (param_4 != nullptr) *param_4 = srcLen;
    } else {
        uint32_t n = (param_3 < srcLen) ? param_3 : srcLen;
        std::memcpy(param_2, puVar5, n);
        if (param_4 != nullptr) *param_4 = (param_3 < srcLen) ? param_3 : srcLen;
        if (bVar1) ((char*)param_2)[param_3 - 1] = '\0';
    }
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x10610 (ordinal 108): query a size class (1 = archive0 field, 2/4 = list max).
uint32_t Storm_Sub10610(uint32_t param_1, uint32_t* param_2) {
    if ((-(uint32_t)(param_2 != nullptr) & (uint32_t)((param_1 == 0) - 1) &
         (uint32_t)((4 < param_1) - 1)) == 0) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    bool bVar3 = g_smack != nullptr;
    *param_2 = 0;
    if (bVar3 && g_archive0 != nullptr) {
        if (param_1 == 1) {
            *param_2 = rd32(g_archive0, 0x22c);
        } else {
            if (param_1 != 2 && param_1 != 4) {
                g_lastError = 0x57;
                SetLastError(0x57);
                LeaveCriticalSection(&g_lock);
                return 0;
            }
            uint8_t* iVar2 = g_listHead;
            if ((intptr_t)iVar2 > 0) {
                do {
                    if ((uint8_t)*(iVar2 + 0x212) != 0xFF) {
                        uint32_t uVar1 = 0;
                        if (param_1 == 2) uVar1 = rd32(iVar2, 300);
                        uint32_t v = rd32(iVar2, 0x128);
                        if (uVar1 < v) uVar1 = v;
                        if (*param_2 < uVar1) *param_2 = uVar1;
                    }
                    iVar2 = (uint8_t*)(uintptr_t)rd32(iVar2, 0x04);
                } while ((intptr_t)iVar2 > 0 && iVar2 != nullptr);
            }
        }
        LeaveCriticalSection(&g_lock);
        return 1;
    }
    g_lastError = 0x4b4;
    SetLastError(0x4b4);
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x104F0 (ordinal 109): report archive index bounds and per-node extents.
uint32_t Storm_Sub104F0(uint32_t* param_1, uint32_t* param_2, int32_t* param_3) {
    EnterCriticalSection(&g_lock);
    int32_t iVar2 = g_45830;
    if (param_1 != nullptr) *param_1 = (uint32_t)(g_45830 - 1);
    if (param_2 != nullptr) *param_2 = (uint32_t)(iVar2 - 1);
    if (param_3 != nullptr) *param_3 = 0;
    uint8_t bVar1 = g_41b4c;
    if (g_smack == nullptr) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
    } else {
        if (g_41b4c != 0xFF) {
            if (param_1 != nullptr) *param_1 = (uint32_t)g_41b4c + (uint32_t)iVar2;
            if (param_2 != nullptr) *param_2 = (uint32_t)bVar1 + (uint32_t)iVar2;
            if (param_3 != nullptr) *param_3 = 1;
            uint8_t* iVar4 = g_listHead;
            if ((intptr_t)iVar4 > 0) {
                do {
                    if ((uint8_t)*(iVar4 + 0x212) != 0xFF) {
                        if (param_1 != nullptr) {
                            uint32_t uVar3 = (uint32_t)*(uint8_t*)(iVar4 + 0x212) + (uint32_t)iVar2;
                            if (*param_1 <= uVar3) uVar3 = *param_1;
                            *param_1 = uVar3;
                        }
                        if (param_2 != nullptr) {
                            uint32_t uVar3 = (uint32_t)*(uint8_t*)(iVar4 + 0x212) + (uint32_t)iVar2;
                            if (uVar3 <= *param_2) uVar3 = *param_2;
                            *param_2 = uVar3;
                        }
                        if (param_3 != nullptr) *param_3 = *param_3 + 1;
                    }
                    iVar4 = (uint8_t*)(uintptr_t)rd32(iVar4, 0x04);
                } while ((intptr_t)iVar4 > 0 && iVar4 != nullptr);
            }
            LeaveCriticalSection(&g_lock);
            return 1;
        }
        g_lastError = 0x85100070;
        SetLastError(0x85100070);
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x16030 (ordinal 110): count the entries of the first non-empty archive node.
uint32_t Storm_Sub16030(int32_t* param_1) {
    if (param_1 == nullptr) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    bool bVar3 = g_smack == nullptr;
    *param_1 = 0;
    if (bVar3) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
    } else {
        if (g_41b4c != 0xFF) {
            Storm_Sub14F20();
            int32_t* piVar1 = Storm_Sub12690();
            if (piVar1 != nullptr) {
                if (piVar1[0x80] == 0) {
                    piVar1 = (int32_t*)g_listHead;
                    if ((intptr_t)piVar1 > 0) {
                        do {
                            if (piVar1[0x80] != 0) goto found;
                            piVar1 = (int32_t*)(uintptr_t)piVar1[1];
                        } while ((intptr_t)piVar1 > 0 && piVar1 != nullptr);
                    }
                } else {
                    /* piVar1[0x80] != 0 falls straight through to found */
                }
                if (piVar1 != nullptr && piVar1[0x80] != 0) {
                found:
                    Storm_Sub12740(piVar1);
                    intptr_t iVar2 = piVar1[0x60];
                    if (iVar2 > 0) {
                        do {
                            *param_1 = *param_1 + 1;
                            iVar2 = (intptr_t)(uintptr_t)rd32((uint8_t*)iVar2, 0x04);
                            if (iVar2 < 1) break;
                        } while (iVar2 != 0);
                    }
                    LeaveCriticalSection(&g_lock);
                    return 1;
                }
            }
        }
        g_lastError = 0x85100070;
        SetLastError(0x85100070);
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x103F0 (ordinal 111): read table entry param_1 (1..15) and optionally the time.
uint32_t Storm_Sub103F0(uint32_t param_1, uint32_t* param_2, uint32_t* param_3, uint32_t* param_4,
                        FILETIME* param_5, uint32_t* param_6) {
    if (param_1 == 0 || param_1 > 0xF || param_2 == nullptr) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    uintptr_t iVar1 = (uintptr_t)param_1 * 0x10;
    *param_2 = rd32((uint8_t*)g_table, iVar1 + 0);
    if (param_3 != nullptr) *param_3 = rd32((uint8_t*)g_table, iVar1 + 4);
    if (param_4 != nullptr) *param_4 = rd32((uint8_t*)g_table, iVar1 + 8);
    if (param_5 != nullptr) {
        SYSTEMTIME local_10;
        GetSystemTime(&local_10);
        SystemTimeToFileTime(&local_10, param_5);
    }
    if (param_6 != nullptr) {
        *param_6 = 10000000;
        param_6[1] = 0;
    }
    if (rd32((uint8_t*)g_table, iVar1 + 0xC) != 0 && g_smack != nullptr) {
        FILETIME local_20;
        uint32_t local_18[2] = {0, 0};
        if (param_5 == nullptr) param_5 = &local_20;
        if (param_6 == nullptr) param_6 = local_18;
        auto cb = (void (*)(uint32_t, uint32_t*, FILETIME*, uint32_t*)) *
            (void**)((uint8_t*)g_smack + 0x18);
        cb(param_1, param_2, param_5, param_6);
    }
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x102E0 (ordinal 112): fill a 9-dword file record for a handle offset.
uint32_t Storm_Sub102E0(int32_t param_1, int32_t* param_2) {
    if (param_2 == nullptr || *param_2 == 0x24) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    param_2[1] = param_2[2] = param_2[3] = param_2[4] = 0;
    param_2[5] = param_2[6] = param_2[7] = param_2[8] = 0;
    uint8_t* iVar1 = g_archive0;
    if (g_smack == nullptr || g_archive0 == nullptr) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
    } else if (g_41b4c == 0xFF) {
        g_lastError = 0x85100070;
        SetLastError(0x85100070);
    } else {
        int32_t* iVar2 = Storm_SubFEB0(param_1 - g_45830);
        if (iVar2 != nullptr) {
            std::memcpy(param_2, iVar1 + 0x214, 9 * 4);
            if (*(int32_t*)((uint8_t*)iVar2 + 0x128) != 0)
                param_2[6] = *(int32_t*)((uint8_t*)iVar2 + 0x128);
            LeaveCriticalSection(&g_lock);
            return 1;
        }
        g_lastError = 0x8510006a;
        SetLastError(0x8510006a);
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x10200 (ordinal 113): fetch a string/name for a handle offset into a buffer.
uint32_t Storm_Sub10200(int32_t param_1, uint8_t* param_2, int32_t param_3) {
    if (param_2 == nullptr || param_3 == 0) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    bool bVar2 = g_smack == nullptr;
    *param_2 = 0;
    if (bVar2) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
    } else if (g_41b4c == 0xFF) {
        g_lastError = 0x85100070;
        SetLastError(0x85100070);
    } else {
        uint8_t* iVar1 = (uint8_t*)(uintptr_t)Storm_SubFEB0(param_1 - g_45830);
        if (iVar1 != nullptr && (*(uint8_t*)(iVar1 + 0x118) & 4) == 0) {
            Storm_Sub3B080(param_2, iVar1 + 8, param_3);
            LeaveCriticalSection(&g_lock);
            return 1;
        }
        g_lastError = 0x8510006a;
        SetLastError(0x8510006a);
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x10140 (ordinal 114): copy the archive's 9-dword record into the caller struct.
uint32_t Storm_Sub10140(int32_t* param_1) {
    if (param_1 == nullptr || *param_1 == 0x24) {
        g_lastError = 0x57;
        SetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    param_1[1] = param_1[2] = param_1[3] = param_1[4] = 0;
    param_1[5] = param_1[6] = param_1[7] = param_1[8] = 0;
    if (g_smack != nullptr && g_archive0 != nullptr) {
        std::memcpy(param_1, g_archive0 + 0x214, 9 * 4);
        LeaveCriticalSection(&g_lock);
        return 1;
    }
    g_lastError = 0x4b4;
    SetLastError(0x4b4);
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x12170 (ordinal 116): normalise three config blocks and call the main callback.
uint32_t Storm_Sub12170(uint32_t param_1, uint32_t param_2, uint32_t* param_3, uint32_t* param_4,
                        uint32_t* param_5) {
    EnterCriticalSection(&g_lock);
    uint32_t local_b0[4];
    uint32_t local_a0[5];
    uint8_t local_8c[60];
    uint32_t local_50[20];
    if (g_smack == nullptr) {
        g_lastError = 0x4b4;
        SetLastError(0x4b4);
    } else if (Storm_Sub11F10(param_3, param_4, param_5, local_b0, local_50, local_a0) == 0) {
        g_lastError = 0x57;
        SetLastError(0x57);
    } else {
        auto cb = (int32_t (*)(uint32_t, uint8_t*, uint32_t*, uint32_t*, uint32_t*)) *
            (void**)((uint8_t*)g_smack + 0x20);
        if (cb(param_1, local_8c, local_b0, local_50, local_a0) != 0) {
            LeaveCriticalSection(&g_lock);
            return 1;
        }
        /* retail falls through with no new error on this path */
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

// 1.13c Storm+0x170B0 (ordinal 117): register the SNet event table and open the network.
// local_8c is the 0x2C-byte output record written by 0x16E70; the three saved globals are
// fields at +0x0C, +0x10 and +0x28 of that record.
uint32_t Storm_Sub170B0(uint32_t param_1, uint32_t param_2, uint32_t* param_3, uint32_t* param_4,
                        uint32_t* param_5) {
    EnterCriticalSection(&g_lock);
    uint32_t local_b0[4];
    uint32_t local_a0[5];
    uint8_t local_8c[0x2C] = {0};
    uint32_t local_50[20];
    if (Storm_Sub11F10(param_3, param_4, param_5, local_b0, local_50, local_a0) == 0) {
        g_lastError = 0x57;
        SetLastError(0x57);
    } else {
        Storm_Sub36020(0x534e4554, 1);
        Storm_Sub36020(0x534e4554, 2);
        Storm_Sub36290(0x534e4554, 2, 0x2, 0, 0x6fc04ef0);
        Storm_Sub36290(0x534e4554, 2, 0xc, 0, 0x6fc00980);
        Storm_Sub36290(0x534e4554, 2, 0xe, 0, 0x6fbffe90);
        Storm_Sub36290(0x534e4554, 2, 0xd, 0, 0x6fc00f30);
        Storm_Sub36290(0x534e4554, 2, 0xf, 0, 0x6fbffe70);
        Storm_Sub36290(0x534e4554, 2, 0x4, 0, 0x6fc04ec0);
        Storm_Sub36290(0x534e4554, 2, 0x5, 0, 0x6fc02f00);
        Storm_Sub36290(0x534e4554, 2, 0x6, 0, 0x6fc02d40);
        Storm_Sub36290(0x534e4554, 2, 0x7, 0, 0x6fc04a80);
        Storm_Sub36290(0x534e4554, 2, 0x8, 0, 0x6fc02c90);
        Storm_Sub36290(0x534e4554, 2, 0x9, 0, 0x6fc02bf0);
        Storm_Sub36290(0x534e4554, 2, 0xa, 0, 0x6fbffe60);
        Storm_Sub36290(0x534e4554, 2, 0xb, 0, 0x6fc02bb0);
        if (Storm_Sub161E0() != 0) {
            if (Storm_Sub16E70(local_8c, local_b0, local_50, local_a0, 0) != 0) {
                g_45354 = rd32(local_8c, 0x28);
                g_45340 = rd32(local_8c, 0x0C);
                g_4516c = rd32(local_8c, 0x10);
                LeaveCriticalSection(&g_lock);
                return 1;
            }
        } else {
            g_lastError = 0xA4;
            SetLastError(0xA4);
        }
    }
    LeaveCriticalSection(&g_lock);
    return 0;
}

}  // namespace
