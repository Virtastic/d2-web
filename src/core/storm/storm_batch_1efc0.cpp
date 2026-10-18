// src/core/storm/storm_batch_1efc0.cpp -- 1.13c Storm.dll internal rows reimplemented from the
// cached decompiles in build-wasm/ghidra-storm/decomp/0x<RVA>.c (Stage A: implementation + marker,
// no spec, no vector).
//
// These are Storm's internal SFile/SMem/SCode helpers: the archive open/read family
// (0x28960/0x295B0/0x29C10/0x2A0B0/0x2A650), the sound-stream open setter (0x281A0/0x25BA0), the
// list teardown (0x28370), the chunk-size setter (0x26150), the SFile config setter (0x259C0) and
// the SCode string formatter (0x37ED0). Retail base 0x6FBF0000; the 32-bit object layouts are kept
// verbatim (offsets in the comments), the same convention as storm_batch_123b0.cpp.
//
// StormLib replaces Storm in our build and nothing in the engine calls these internals, so the
// unmapped callees and the kernel objects are seams with retail's argument order: the Storm pool
// (0x1C580 alloc / 0x1B960 and 0x0B960 free / 0x4040 / 0x4290 / 0x40D0 / 0x41D0 / 0x4310 list
// allocations), the host file handle API, and the event the sound thread waits on. SetLastError is
// a seam because the only observable side effect is the error global the bodies also store.
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <Windows.h>

namespace {

int32_t g_lastError = 0;            // 0x6FC45388  last Storm error, mirrored to SetLastError
CRITICAL_SECTION g_lock;            // 0x6FC45988  SFile global lock
void* g_event_4312c = nullptr;      // 0x6FC4312C  sound-thread wake event
int32_t g_list_45978 = 0;           // 0x6FC45978  AUREQUEST list head (relative-list encoding)
uint32_t g_pool_459a0 = 0;          // 0x6FC459A0  AUREQUEST pool anchor
int32_t g_count_458e8 = 0;          // 0x6FC458E8  pending sound-stream count
uint32_t g_chunk_41a00 = 0;         // 0x6FC41A00  read chunk size for the streaming path
uint32_t g_4313c = 0;               // 0x6FC4313C  SFile buffer size (default 0x4000)
uint32_t g_43138 = 0;               // 0x6FC43138  config case 1
uint32_t g_43134 = 0;               // 0x6FC43134  config case 3
uint32_t g_43140 = 0;               // 0x6FC43140  config case 4
uint32_t g_43148 = 0;               // 0x6FC43148  config case 5

void StormSetLastError(uint32_t e) { g_lastError = (int32_t)e; }

// ---- unmapped internals, retail argument order (seams) ----------------------------------------
uint32_t* Storm_Sub4290(uint32_t* anchor, uint32_t, uint32_t);            // 0x4290 pool alloc
int32_t Storm_Sub4040(void* p);                                            // 0x4040 pool free
int32_t Storm_SubB960(void* p, const char* tag, int32_t line);             // 0xB960 operator delete
void Storm_Sub179F0(int32_t h);                                            // 0x179F0 stream close
void Storm_SubD4D0(void);                                                  // 0x1D4D0
void Storm_Sub7D60(void);                                                  // 0x17D60
void Storm_Sub13250(void);                                                 // 0x23250
void Storm_Sub98D0(void);                                                  // 0x198D0
void Storm_SubF20C0(uint32_t);                                             // 0x20C0
void Storm_SubF2080(void);                                                 // 0x2080
void Storm_Sub24150(void);                                                 // 0x34150
void Storm_Sub3E10(void);                                                  // 0x13E10
void Storm_Sub261C0(void);                                                 // 0x361C0
void Storm_Sub28130(uint8_t*);                                             // 0x38130
void Storm_Sub28F90(void);                                                 // 0x38F90
void Storm_Sub14FD0(void);                                                 // 0x24FD0
void Storm_Sub17020(void);                                                 // 0x27020
void Storm_Sub26D40(void);                                                 // 0x36D40

// Host event handle: a no-op wake in ours, as the sound thread does not exist.
void StormSetEvent(void* h) { (void)h; }

// ---- SFile object layouts (retail 32-bit field offsets) ---------------------------------------
// open-file record (0x41D0/0x40D0 allocation): +0x08 name, +0x10C hFile (-1 = archive member),
// +0x110 archive, +0x114 block entry, +0x118 key, +0x11C position, +0x124 sector count,
// +0x128 sector-offset table, +0x134 read buffer, +0x138 buffered bytes, +0x13C decoded bytes,
// +0x140 refs, +0x144 running hash, +0x148/+0x14C hash flags, +0x150 hash state, +0x154 hashed.
// archive record: +0x08 name, +0x120 sector size, +0x134 block table, +0x138 hash table,
// +0x140 open count, +0x300 archive-relative start.

int32_t g_43120 = 0;                // 0x6FC43120  archive system initialised
int32_t g_45984 = 0;                // 0x6FC45984  archive list head, next at +0x04
uint32_t g_4538c = 0;               // 0x6FC4538C  current archive-set search name

uint32_t rd32(const void* p, uintptr_t off) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + off, 4); return v; }
void wr32(void* p, uintptr_t off, uint32_t v) { std::memcpy((uint8_t*)p + off, &v, 4); }

HANDLE kInvalidHandle() { return (HANDLE)(intptr_t)-1; }
constexpr const char* kSFileCpp = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";

// ---- SFile internals, retail argument order (seams) -------------------------------------------
void* Storm_Sub41D0(uint32_t* anchor, uint32_t, uint32_t);                 // 0x41D0 alloc open-file
void* Storm_Sub40D0(uint32_t* anchor, uint32_t, uint32_t);                 // 0x40D0 alloc archive
void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags); // 0x1C580 alloc
void Storm_Sub17FC0(void);                                                 // 0x17FC0 init archive set
void Storm_Sub16BD0(char* out, int32_t which);                             // 0x16BD0 install dir
uint32_t Storm_Sub2B080(uint8_t* dst, const uint32_t* src, uint32_t n);    // 0x2B080 strncpy max
char* Storm_Sub2B2D0(const char* s, int32_t c);                            // 0x2B2D0 strchr
void* Storm_Sub6460(const void* s, const char* find);                      // 0x6460 strstr
int32_t Storm_Sub16AC0(void* archive, void* name);                         // 0x16AC0 find file
uint32_t Storm_Sub15ED0(const char* name, uint32_t type);                  // 0x15ED0 MPQ hash
void Storm_Sub19560(void);                                                 // 0x19560 stream helper
int32_t Storm_Sub18E80(uint32_t pos, void* buf, uint32_t len);             // 0x18E80 pread
uint32_t* Storm_Sub17A70(uint32_t pos, intptr_t buf, uint32_t len, uint32_t, uint32_t, uint32_t,
                         uint32_t chunk, uint32_t, uint32_t prio, uint32_t event, uint32_t,
                         uint32_t last, uint32_t);                         // 0x17A70 async read
uint32_t Storm_Sub12C80(void* dst, const void* src, uint32_t n);           // 0x12C80 memcpy
void Storm_Sub16E20(int32_t* rec);                                         // 0x16E20 close record
void Storm_Sub1F590(uint32_t err, uint32_t* name, int32_t off, void*, uint32_t, uint32_t); // 0x1F590
uint32_t Storm_CryptTable(uint32_t index);                                 // DAT_6FC41600 entry

// kernel32 seams: the host file API of retail's world; ours never opens a retail archive.
DWORD StormGetFileAttributes(const char* path);
HANDLE StormCreateFile(const char* path, uint32_t access, uint32_t share, uint32_t disposition,
                       uint32_t flags);
DWORD StormSetFilePointer(HANDLE h, LONG distance, LONG* high, uint32_t method);
DWORD StormGetFileSize(HANDLE h);
int32_t StormReadFile(HANDLE h, void* buf, uint32_t len, DWORD* got);
HANDLE StormCreateEvent(void);
void StormWaitForSingleObject(HANDLE h, uint32_t ms);
void StormCloseHandle(HANDLE h);

// 1.13c Storm+0x281A0
extern "C"
void Storm_Sub281A0(int32_t param_1, int32_t param_2) {
    if (param_1 == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return;
    }
    Storm_Sub179F0(param_1);
    if (0 < param_2) StormSetEvent(g_event_4312c);
}

// 1.13c Storm+0x28370
extern "C"
void Storm_Sub28370(int32_t param_1) {
    if (param_1 == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return;
    }
    EnterCriticalSection(&g_lock);
    // ((x < 1) - 1) & x: the list head is only followed when it is non-negative.
    int32_t* piVar2 = (int32_t*)(intptr_t)(((int32_t)g_list_45978 < 1) ? (uint32_t)g_list_45978 : 0u);
    while (0 < (int32_t)(intptr_t)piVar2) {
        if (piVar2[9] == param_1) {
            if (piVar2[2] != 0) {
                uint32_t* puVar3 = Storm_Sub4290(&g_pool_459a0, 0, 0);
                puVar3[2] = (uint32_t)piVar2[2];
            }
            uint32_t uVar1 = (uint32_t)piVar2[1];
            Storm_Sub4040(piVar2);
            Storm_SubB960(piVar2, "..\\AUREQUEST", -2);
            piVar2 = (int32_t*)(intptr_t)(((int32_t)uVar1 < 1) ? uVar1 : 0u);
        } else {
            piVar2 = (int32_t*)(intptr_t)(uint32_t)piVar2[1];
        }
    }
    LeaveCriticalSection(&g_lock);
}

// 1.13c Storm+0x26150
extern "C"
void Storm_Sub26150(uint32_t param_1) {
    if (((param_1 - 1) & param_1) != 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return;
    }
    g_chunk_41a00 = param_1;
}

// 1.13c Storm+0x25BA0
extern "C"
uint32_t Storm_Sub25BA0(void) {
    Storm_SubD4D0();
    Storm_Sub7D60();
    Storm_Sub13250();
    Storm_Sub98D0();
    Storm_SubF20C0(0);
    Storm_SubF2080();
    while (0 < g_count_458e8) Storm_Sub24150();
    Storm_Sub3E10();
    Storm_Sub261C0();
    Storm_Sub28130((uint8_t*)(uintptr_t)0x6FC4587Cu);
    Storm_Sub28F90();
    Storm_Sub14FD0();
    Storm_Sub17020();
    Storm_Sub26D40();
    return 1;
}

// 1.13c Storm+0x259C0
extern "C"
uint32_t Storm_Sub259C0(uint32_t param_1, uint32_t* param_2, int32_t param_3) {
    g_lastError = 0x57;
    if (param_2 == nullptr) {
        StormSetLastError(0x57);
        return 0;
    }
    StormSetLastError(0x57);
    uint32_t uVar1 = g_4313c;
    switch (param_1) {
    case 1:
        if (param_3 != 4) return 0;
        g_43138 = *param_2;
        break;
    case 2:
        if (param_3 != 4) return 0;
        uVar1 = *param_2;
        if (uVar1 == 0) return 0;
        if (((uVar1 - 1) ^ uVar1) != uVar1 * 2 - 1) return 0;   // must be a power of two
        break;
    case 3:
        if (param_3 != 4) return 0;
        g_43134 = *param_2;
        break;
    case 4:
        if (param_3 != 4) return 0;
        g_43140 = *param_2;
        break;
    case 5:
        if (param_3 != 4) return 0;
        g_43148 = *param_2;
        break;
    }
    g_4313c = uVar1;
    g_lastError = 0;
    StormSetLastError(0);
    return 1;
}

// 1.13c Storm+0x28960: open one file, from the local directories or from an archive-set member.
extern "C"
bool Storm_SFileOpenFile_28960(void* param_1, uint32_t* param_2, uint8_t param_3, void** param_4) {
    uint32_t uVar1;
    if (param_4 == nullptr) {
        uVar1 = 0;
    } else {
        *param_4 = nullptr;
        uVar1 = 0xFFFFFFFF;
    }
    if ((uVar1 & -(uint32_t)(param_2 != nullptr) & -(uint32_t)(*(char*)param_2 != '\0')) == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return false;
    }
    EnterCriticalSection(&g_lock);
    if (g_43120 == 0) Storm_Sub17FC0();
    bool bVar9 = false;
    char local_104[260];
    uint8_t* arch = (uint8_t*)param_1;
    uint8_t* pLVar8 = nullptr;

    if ((param_3 & 3) != 0) {
        if ((param_3 & 2) == 0) goto LAB_18a14;
        Storm_Sub16BD0(local_104, 1);
        if ((~(StormGetFileAttributes(local_104) >> 4) & 1) == 0) goto LAB_18a14;
        goto LAB_18a7e;
    }
LAB_18a14:
    if ((param_3 & 1) != 0) {
        if (*(char*)param_2 == '\\' || Storm_Sub6460(param_2, ":\\") != nullptr ||
            Storm_Sub6460(param_2, "\\\\") != nullptr) {
            Storm_Sub2B080((uint8_t*)local_104, param_2, 0x104);
        } else {
            Storm_Sub16BD0(local_104, 0);
        }
        if ((~(StormGetFileAttributes(local_104) >> 4) & 1) != 0) goto LAB_18a7e;
    }
    goto LAB_18bb7;

LAB_18a7e:
    {
        HANDLE pvVar4 = StormCreateFile(local_104, 0x80000000, 1, 3, 0x8000000);
        if (pvVar4 != kInvalidHandle()) {
            if ((param_3 & 4) == 0) {
                uint8_t* puVar3 = (uint8_t*)Storm_Sub41D0((uint32_t*)(uintptr_t)0x6FC45964, 0, 0);
                Storm_Sub2B080(puVar3 + 8, param_2, 0x104);
                wr32(puVar3, 0x10c, (uint32_t)(uintptr_t)pvVar4);
                wr32(puVar3, 0x140, 1);
                wr32(puVar3, 0x148, 0);
                wr32(puVar3, 0x14c, 1);
                *param_4 = puVar3;
                bVar9 = true;
            } else {
                *param_4 = pvVar4;
                bVar9 = true;
            }
            goto LAB_18d84;
        }
    }
    if (param_1 == nullptr) {
        if (0 < g_45984 && ((arch = (uint8_t*)(uintptr_t)(uint32_t)g_45984), g_43120 != 0)) {
            do {
                uVar1 = (uint32_t)Storm_Sub16AC0(arch, (void*)(uintptr_t)g_4538c);
                if ((uVar1 == 0xFFFFFFFF &&
                     ((arch = (uint8_t*)(uintptr_t)rd32(arch, 4)), (intptr_t)arch < 1)) ||
                    arch == nullptr)
                    goto LAB_18bb7;
            } while (uVar1 == 0xFFFFFFFF);
            goto LAB_18b5f;
        }
        goto LAB_18bb7;
    }

LAB_18b5f:
    uVar1 = (uint32_t)Storm_Sub16AC0(arch, (void*)(uintptr_t)g_4538c);
    if (uVar1 != 0xFFFFFFFF) {
        uint32_t hashBase = rd32(arch, 0x138);
        int32_t blockIdx = (int32_t)rd32((void*)(uintptr_t)(hashBase + 0xc + uVar1 * 0x10), 0);
        int32_t iVar7 = blockIdx * 0x1c;
        uint32_t blockBase = rd32(arch, 0x134);
        uint32_t flags = rd32((void*)(uintptr_t)(blockBase + iVar7), 0xc);
        pLVar8 = (uint8_t*)(uintptr_t)(blockBase + iVar7);
        if ((int32_t)flags < 0) {   // MPQ_FILE_EXISTS
            if ((param_3 & 4) == 0) {
                // strip drive/path prefixes: walk past every ':' then every '\\'.
                const char* puVar3c = (const char*)param_2;
                char* pcVar5 = Storm_Sub2B2D0(puVar3c, ':');
                while (pcVar5 != nullptr) {
                    pcVar5 = Storm_Sub2B2D0(puVar3c, ':');
                    puVar3c = pcVar5 + 1;
                    pcVar5 = Storm_Sub2B2D0(puVar3c, ':');
                }
                pcVar5 = Storm_Sub2B2D0(puVar3c, '\\');
                while (pcVar5 != nullptr) {
                    pcVar5 = Storm_Sub2B2D0(puVar3c, '\\');
                    puVar3c = pcVar5 + 1;
                    pcVar5 = Storm_Sub2B2D0(puVar3c, '\\');
                }
                uint32_t local_10c = Storm_Sub15ED0(puVar3c, 3);
                uint32_t blockFlags = rd32(pLVar8, 0xc);
                if ((blockFlags & 0x20000u) != 0)   // MPQ_FILE_FIX_KEY
                    local_10c = ((uint32_t)(int32_t)rd32(pLVar8, 0) - rd32(arch, 300) + local_10c) ^
                                rd32(pLVar8, 8);
                uint32_t sectorSize = rd32(arch, 0x120);
                uint32_t uVar1s = (rd32(pLVar8, 8) - 1u + sectorSize) / sectorSize;
                uint8_t* local_110 = nullptr;
                if ((blockFlags & 0xff00u) != 0)
                    local_110 = (uint8_t*)Storm_Sub1C580(uVar1s * 4 + 4, kSFileCpp, 0x1181, 0);
                bVar9 = true;
                uint8_t* rec = (uint8_t*)Storm_Sub41D0((uint32_t*)(uintptr_t)0x6FC45964, 0, 0);
                Storm_Sub2B080(rec + 8, param_2, 0x104);
                wr32(rec, 0x10c, 0xFFFFFFFF);
                wr32(rec, 0x110, (uint32_t)(uintptr_t)param_1);
                wr32(rec, 0x114, (uint32_t)(uintptr_t)pLVar8);
                wr32(rec, 0x118, local_10c);
                wr32(rec, 0x124, uVar1s);
                wr32(rec, 0x128, (uint32_t)(uintptr_t)local_110);
                uint8_t* recbuf = (uint8_t*)Storm_Sub1C580(0x1000, kSFileCpp, 0x118c, 0);
                wr32(rec, 0x134, (uint32_t)(uintptr_t)recbuf);
                wr32(rec, 0x140, 1);
                int32_t iVar7b = (int32_t)rd32(pLVar8, 0x18);
                wr32(rec, 0x14c, 1);
                wr32(rec, 0x150, 1);
                wr32(rec, 0x148, (uint32_t)(iVar7b != 0));
                wr32(arch, 0x140, rd32(arch, 0x140) + 1);
                *param_4 = rec;
                goto LAB_18d84;
            }
            if ((flags & 0x1ff00u) == 0) {
                // an archive member stored whole: hand back the archive file itself, positioned.
                HANDLE pvVar4 = StormCreateFile((const char*)(arch + 8), 0x80000000, 1, 3, 0x8000000);
                bVar9 = pvVar4 != kInvalidHandle();
                *param_4 = pvVar4;
                if (bVar9) {
                    StormSetFilePointer(pvVar4, (LONG)rd32(pLVar8, 0), nullptr, 0);
                    goto LAB_18d84;
                }
                goto LAB_18bb7;
            }
        }
        g_lastError = 0x3ee;
        StormSetLastError(0x3ee);
        goto LAB_18d84;
    }
LAB_18bb7:
    g_lastError = 2;
    StormSetLastError(2);
LAB_18d84:
    LeaveCriticalSection(&g_lock);
    return bVar9;
}

// 1.13c Storm+0x295B0: SFile read; direct-file reads bypass the sector buffer, archive members go
// through the sector table and either a synchronous or an overlapped chunk loop.
extern "C"
int32_t Storm_SFileReadFile_295B0(uint8_t* obj, uint32_t* param_2, uint32_t param_3, int32_t* param_4,
                                  uint8_t* param_5, uint32_t param_6, uint32_t param_7) {
    uint8_t* uObj = obj;
    if (param_4 != nullptr) *param_4 = 0;
    if (param_2 == nullptr) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    if (param_3 == 0) {
        if (param_5 != nullptr && *(HANDLE*)(param_5 + 0x10) != nullptr) StormSetEvent(*(HANDLE*)(param_5 + 0x10));
        LeaveCriticalSection(&g_lock);
        return 1;
    }
    if (rd32(obj, 0x10c) != 0xFFFFFFFF && param_5 == nullptr) {
        HANDLE hFile = (HANDLE)(uintptr_t)rd32(obj, 0x10c);
        DWORD DVar4 = StormSetFilePointer(hFile, 0, nullptr, 1);
        if (rd32(obj, 0x14c) != 0 && DVar4 == 0) {
            wr32(obj, 0x144, 0xFFFFFFFF);
            wr32(obj, 0x154, 0);
            wr32(obj, 0x150, 2);
        }
        int32_t bVar3 = StormReadFile(hFile, param_2, param_3, (DWORD*)param_4);
        if (bVar3 != 0 && rd32(obj, 0x14c) != 0 && rd32(obj, 0x150) == 2) {
            uint32_t h = rd32(obj, 0x144);
            const uint8_t* p = (const uint8_t*)param_2;
            for (uint32_t n = param_3; n != 0; --n, ++p)
                h = (h >> 8) ^ Storm_CryptTable(((uint32_t)*p ^ h) & 0xFF);
            wr32(obj, 0x144, h);
            wr32(obj, 0x154, rd32(obj, 0x154) + param_3);
        }
        DVar4 = StormGetFileSize(hFile);
        StormSetFilePointer(hFile, 0, nullptr, 1);
        if (rd32(obj, 0x14c) != 0 && rd32(obj, 0x150) == 2 && rd32(obj, 0x154) == DVar4) {
            wr32(obj, 0x144, ~rd32(obj, 0x144));
            wr32(obj, 0x150, 4);
        }
        LeaveCriticalSection(&g_lock);
        return bVar3;
    }

    uint8_t* local_28;
    uint32_t local_c;
    if (rd32(obj, 0x134) == 0) {
LAB_197dd:
        local_28 = (uint8_t*)param_2;
        local_c = param_3;
        if (param_5 == nullptr) {
            int32_t iVar2 = (int32_t)rd32((void*)(uintptr_t)rd32(obj, 0x110), 0x120);
            uint32_t uVar10;
            int32_t room = iVar2 - (int32_t)(((uint32_t)(iVar2 - 1)) & rd32(obj, 0x11c));
            if (room < 0x1001) {
                uVar10 = (uint32_t)room;
            } else {
                uVar10 = 0x1000;
            }
            if (param_3 < uVar10) {
                local_28 = (uint8_t*)(uintptr_t)rd32(obj, 0x134);
                local_c = uVar10;
            }
        }
        if (local_c == 0) {
            if (param_5 != nullptr && *(HANDLE*)(param_5 + 0x10) != nullptr)
                StormSetEvent(*(HANDLE*)(param_5 + 0x10));
            goto LAB_19b70;
        }
        DWORD DVar4 = (param_5 == nullptr) ? (DWORD)rd32(obj, 0x11c) : *(DWORD*)(param_5 + 8);
        int32_t result = 0;
        if (rd32(obj, 0x10c) == 0xFFFFFFFF &&
            rd32((void*)(uintptr_t)rd32(obj, 0x110), 0x110) == 0) {
            result = Storm_Sub18E80(DVar4, local_28, local_c);
            if (rd32(obj, 0x14c) != 0 && rd32(obj, 0x150) == 2 &&
                (int32_t)rd32(obj, 0x154) == (int32_t)rd32((void*)(uintptr_t)rd32(obj, 0x114), 8)) {
                wr32(obj, 0x144, ~rd32(obj, 0x144));
                wr32(obj, 0x150, 4);
            }
            if (param_5 != nullptr && *(HANDLE*)(param_5 + 0x10) != nullptr)
                StormSetEvent(*(HANDLE*)(param_5 + 0x10));
        } else {
            uint32_t local_14 = 0x100000;
            if (rd32(obj, 0x110) != 0) {
                int32_t iVar2 = (int32_t)rd32((void*)(uintptr_t)rd32(obj, 0x110), 0x110);
                if (iVar2 == 3 || iVar2 == 2 || (iVar2 == 1 && param_5 != nullptr))
                    local_14 = g_chunk_41a00;
            }
            Storm_Sub19560();
            uint32_t* puVar9 = nullptr;
            HANDLE hHandle = (param_5 == nullptr) ? StormCreateEvent()
                                                  : *(HANDLE*)(param_5 + 0x10);
            uint32_t chunks = ((local_14 - 1) + local_c) / local_14;
            if (chunks != 0) {
                int32_t local_24 = 0;
                for (uint32_t local_18 = 0; local_18 < chunks; ++local_18) {
                    uint32_t uVar7 = (uint32_t)(local_18 == chunks - 1);
                    uint32_t local_1c = local_14;
                    if (local_c - local_24 <= local_14) local_1c = local_c - local_24;
                    uint32_t uVar8, uVar6;
                    if (param_5 == nullptr && uVar7 != 0) {
                        uVar8 = 0;
                        uVar6 = 1;
                    } else {
                        uVar8 = 1;
                        uVar6 = param_6;
                        if (param_5 == nullptr) {
                            uVar8 = 0;
                            uVar6 = 1;
                        }
                    }
                    puVar9 = Storm_Sub17A70(
                        (uint32_t)(local_24 + DVar4),
                        (intptr_t)local_28 + ((local_24 + DVar4) - DVar4),
                        (uint32_t)(uintptr_t)local_28, 0, 0, 0, local_1c,
                        -(uint32_t)(param_5 != nullptr) & 0x7FFFFFFFu, uVar6,
                        -(uint32_t)(uVar7 != 0) & (uint32_t)(uintptr_t)hHandle, uVar8, uVar7,
                        -(uint32_t)(uVar7 != 0) & param_7);
                    if (uVar7 == 0) result += (int32_t)local_1c;
                    local_24 += (int32_t)local_14;
                }
            }
            if (param_5 != nullptr) goto LAB_19b70;
            LeaveCriticalSection(&g_lock);
            StormWaitForSingleObject(hHandle, 0xFFFFFFFF);
            EnterCriticalSection(&g_lock);
            StormCloseHandle(hHandle);
            result += (int32_t)rd32(puVar9, 0x3c);
            Storm_Sub4040(puVar9);
            Storm_SubB960(puVar9, "delete", -1);
        }
        uint32_t uVar10 = param_3;
        if (result <= (int32_t)param_3) uVar10 = (uint32_t)result;
        if (local_28 != (uint8_t*)param_2) {
            std::memcpy(param_2, local_28, uVar10);
            wr32(obj, 0x138, uVar10);
            wr32(obj, 0x13c, (uint32_t)result);
        }
        if (param_5 == nullptr) wr32(obj, 0x11c, rd32(obj, 0x11c) + uVar10);
        if (param_4 != nullptr) *param_4 = *param_4 + (int32_t)uVar10;
        if (uVar10 != param_3) {
            g_lastError = 0x26;
            StormSetLastError(0x26);
            LeaveCriticalSection(&g_lock);
            return 0;
        }
    } else {
        uint32_t uVar10 = rd32(obj, 0x138);
        if (rd32(obj, 0x13c) <= uVar10 || param_5 != nullptr) goto LAB_197dd;
        uint32_t uVar7 = rd32(obj, 0x13c) - uVar10;
        if (param_3 < uVar7) uVar7 = param_3;
        uint32_t copied = Storm_Sub12C80(param_2, (void*)(uintptr_t)(uVar10 + rd32(obj, 0x134)), uVar7);
        wr32(obj, 0x138, rd32(obj, 0x138) + copied);
        param_2 = (uint32_t*)((uint8_t*)param_2 + copied);
        param_3 -= copied;
        wr32(obj, 0x11c, rd32(obj, 0x11c) + copied);
        if (param_4 != nullptr) *param_4 = (int32_t)copied;
        if (param_3 != 0) goto LAB_197dd;
    }
    if (rd32(obj, 0x148) != 0 && rd32(obj, 0x14c) != 0 && rd32(obj, 0x150) == 4 &&
        rd32(obj, 0x144) != rd32((void*)(uintptr_t)rd32(obj, 0x114), 0x18)) {
        Storm_Sub1F590(0x85100083, (uint32_t*)(uObj + 8), -4, nullptr, 0, 1);
    }
LAB_19b70:
    LeaveCriticalSection(&g_lock);
    return 1;
}

// 1.13c Storm+0x29C10: read a whole file into a fresh allocation (size + trailing slack zeroed).
extern "C"
bool Storm_Sub29C10(void* param_1, uint32_t* param_2, int32_t* param_3, uint32_t* param_4,
                    uint32_t param_5, uint32_t param_6, int32_t param_7, uint32_t param_8) {
    uint8_t* rec = nullptr;
    if (param_3 == nullptr) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return false;
    }
    *param_3 = 0;
    if (param_2 == nullptr) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return false;
    }
    if (param_4 != nullptr) *param_4 = 0;
    bool bVar2 = Storm_SFileOpenFile_28960(param_1, param_2, (uint8_t)param_6, (void**)&rec);
    if (bVar2) {
        uint32_t uVar3;
        EnterCriticalSection(&g_lock);
        if (rd32(rec, 0x10c) == 0xFFFFFFFF) {
            uVar3 = rd32((void*)(uintptr_t)rd32(rec, 0x114), 8);
            LeaveCriticalSection(&g_lock);
        } else {
            LeaveCriticalSection(&g_lock);
            uVar3 = StormGetFileSize((HANDLE)(uintptr_t)rd32(rec, 0x10c));
        }
        uint32_t uVar7 = param_5;
        uint8_t* puVar4 = (uint8_t*)Storm_Sub1C580(uVar3 + param_5, kSFileCpp, 0xfc2, 0);
        int32_t iVar5 = Storm_SFileReadFile_295B0(rec, (uint32_t*)puVar4, uVar3, nullptr, nullptr,
                                                 param_7, param_8);
        if (iVar5 == 0) {
            if (puVar4 != nullptr) Storm_SubB960(puVar4, kSFileCpp, 0xfcf);
        } else {
            if (uVar7 != 0) std::memset(puVar4 + uVar3, 0, uVar7);
            *param_3 = (int32_t)(intptr_t)puVar4;
            if (param_4 != nullptr) *param_4 = uVar3;
        }
    }
    if (rec != nullptr) Storm_Sub16E20((int32_t*)rec);
    return *param_3 != 0;
}

// ---- 0x2A0B0 / 0x2A650 module state and seams --------------------------------------------------
uint32_t g_4597c = 0;               // 0x6FC4597C  archive-list header (relative-list anchor)
uint32_t g_45980 = 0;               // 0x6FC45980  archive-list anchor used by the insert
uint32_t g_459c4 = 0;               // 0x6FC459C4  stream-list anchor
void* g_42ffc = nullptr;            // 0x6FC42FFC  DirectSound/provider interface; never set in ours
uint32_t g_43130 = 0;               // 0x6FC43130  archive-open mode flags
uint32_t g_43114 = 0;               // 0x6FC43114  sound buffer size
uint32_t g_43128 = 0;               // 0x6FC43128  sound-read buffer

int32_t Storm_Sub16750(void);                                              // 0x16750 archive mode
void Storm_Sub16D70(uint32_t* state, void* table, uint32_t size, uint32_t* name); // 0x16D70 decrypt
void Storm_Sub15F30(uint32_t key, uint32_t* table);                        // 0x15F30 table init
void Storm_Sub15E50(uint32_t count);                                       // 0x15E50 count init
void Storm_Sub16CB0(uint32_t* rec);                                        // 0x16CB0 teardown
void Storm_Sub19F60(uint8_t* rec, void* blockTable, int32_t count);        // 0x19F60 post-open
void Storm_Sub3E20(void* rec);                                             // 0x3E20 free archive
void Storm_Sub181F0(uint8_t* rec);                                         // 0x181F0 release buffer
int32_t Storm_Sub19DF0(uint32_t magic, int32_t* size);                     // 0x19DF0 find chunk
uint32_t* Storm_Sub4310(uint32_t* anchor, uint32_t, uint32_t);             // 0x4310 alloc stream
// provider interface vtable slots (retail +0x0C/+0x34/+0x3C/+0x40)
int32_t Storm_ProviderCreate(void* iface, void* desc, void** out, uint32_t);
void Storm_ProviderPlay(void* iface, uint32_t);
void Storm_ProviderSetVolume(void* iface, uint32_t v);
void Storm_ProviderSetPan(void* iface, uint32_t v);

// 1.13c Storm+0x2A0B0: SFileOpenArchive -- open an MPQ, read and decrypt its tables, link it into
// the priority-ordered archive list.
extern "C"
uint32_t Storm_SFileOpenArchive_2A0B0(uint32_t* param_1, uint32_t param_2, uint32_t param_3,
                                      void** param_4) {
    uint32_t uVar2;
    if (param_4 == nullptr) {
        uVar2 = 0;
    } else {
        *param_4 = nullptr;
        uVar2 = 0xFFFFFFFF;
    }
    if ((uVar2 & -(uint32_t)(param_1 != nullptr) & -(uint32_t)(*(char*)param_1 != '\0')) == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    if (g_43120 == 0) Storm_Sub17FC0();
    char local_104[260];
    Storm_Sub2B080((uint8_t*)local_104, param_1, 0x104);
    if ((~(StormGetFileAttributes(local_104) >> 4) & 1) == 0) {   // a directory: use it as search path
        if (*(char*)param_1 == '\\' || Storm_Sub6460(param_1, ":\\") != nullptr ||
            Storm_Sub6460(param_1, "\\\\") != nullptr) {
            Storm_Sub2B080((uint8_t*)local_104, param_1, 0x104);
        } else {
            Storm_Sub16BD0(local_104, 0);
        }
    }
    int32_t iVar5 = Storm_Sub16750();
    uint32_t local_110 = param_3 & 1;
    int32_t local_10c = iVar5;
    if (local_110 != 0 && iVar5 == 0) {
        g_lastError = 0xf;
        StormSetLastError(0xf);
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    HANDLE hFile = StormCreateFile(local_104, 0x80000000, 1, 3, 0);
    if (hFile == kInvalidHandle()) {
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    uint8_t* rec = (uint8_t*)Storm_Sub40D0(&g_4597c, 0, 0);
    Storm_Sub2B080(rec + 8, (const uint32_t*)local_104, 0x104);
    wr32(rec, 0x114, param_2);
    wr32(rec, 0x10c, (uint32_t)(uintptr_t)hFile);
    wr32(rec, 0x140, 1);
    if (iVar5 == 0) {
        wr32(rec, 0x110, (param_3 & 2) != 0 ? 1u : 0u);
    } else {
        wr32(rec, 0x110, 3);
    }
    uint8_t* pHdr = (uint8_t*)Storm_Sub1C580(0x20, kSFileCpp, 0x1024, 0);
    wr32(rec, 0x130, (uint32_t)(uintptr_t)pHdr);
    wr32(rec, 0x12c, 0);
    uint8_t* local_118 = (uint8_t*)Storm_Sub1C580(0x1000, kSFileCpp, 0x1029, 0);
    uint32_t uVar2b = 0;
    DWORD local_120 = 0;
    for (;;) {
        uint32_t uVar1 = rd32(rec, 0x12c);
        if (local_120 + uVar2b <= uVar1) {
            uint32_t local_114 = uVar1;
            StormSetFilePointer(hFile, (LONG)uVar1, nullptr, 0);
            DWORD local_11c = 0;
            StormReadFile(hFile, local_118, 0x1000, &local_11c);
            if (local_11c < 0x20) {
                Storm_Sub3E20(rec);
                Storm_SubB960(rec, "delete", -1);
                g_lastError = 0x8510006c;
                StormSetLastError(0x8510006c);
                LeaveCriticalSection(&g_lock);
                Storm_SubB960(local_118, kSFileCpp, 0x1044);
                return 0;
            }
            local_120 = local_11c;
            uVar2b = local_114;
        }
        std::memcpy(pHdr, local_118 + (rd32(rec, 0x12c) - uVar2b), 0x20);
        if ((int32_t)rd32(pHdr, 0) == 0x1A51504D && (int32_t)rd32(pHdr, 4) > 0x1F) break;
        wr32(rec, 0x12c, rd32(rec, 0x12c) + 0x200);
    }
    Storm_SubB960(local_118, kSFileCpp, 0x106b);
    uVar2b = 0x200u << (*(uint8_t*)(pHdr + 0xe) & 0x1F);
    wr32(rec, 0x120, uVar2b);
    uint8_t* puVar6 = (uint8_t*)Storm_Sub1C580(uVar2b, kSFileCpp, 0x1070, 0);
    wr32(rec, 0x124, (uint32_t)(uintptr_t)puVar6);
    uint32_t hashCount = rd32(pHdr, 0x18);
    puVar6 = (uint8_t*)Storm_Sub1C580(hashCount << 4, kSFileCpp, 0x1079, 0);
    wr32(rec, 0x138, (uint32_t)(uintptr_t)puVar6);
    Storm_Sub16D70((uint32_t*)(rec + 0x13c), puVar6, hashCount << 4, (uint32_t*)(rec + 8));
    uVar2b = Storm_Sub15ED0((const char*)(rec + 8), 3);
    Storm_Sub15F30(uVar2b, (uint32_t*)(uintptr_t)rd32(rec, 0x138));
    uint32_t blockCount = rd32(pHdr, 0x1c);
    puVar6 = (uint8_t*)Storm_Sub1C580(blockCount * 0x1c, kSFileCpp, 0x1092, 0);
    wr32(rec, 0x134, (uint32_t)(uintptr_t)puVar6);
    Storm_Sub16D70((uint32_t*)(rec + 0x13c), puVar6, blockCount << 4, (uint32_t*)(rec + 8));
    uVar2b = Storm_Sub15ED0((const char*)(rec + 8), 3);
    Storm_Sub15F30(uVar2b, (uint32_t*)(uintptr_t)rd32(rec, 0x134));
    Storm_Sub15E50(rd32(pHdr, 0x1c));
    if (rd32(rec, 0x12c) != 0 && blockCount != 0) {
        uint32_t tableBase = rd32(rec, 0x134);
        for (uint32_t i = 0; i < blockCount; ++i) {
            uint32_t at = tableBase + i * 0x1c;
            if (rd32((void*)(uintptr_t)at, 0) != 0)
                wr32((void*)(uintptr_t)at, 0, rd32((void*)(uintptr_t)at, 0) + rd32(rec, 0x12c));
        }
    }
    // priority-ordered insert into the archive list
    uint32_t* anchor = nullptr;
    if ((int32_t)g_45984 >= 1) {
        uint32_t node = (uint32_t)g_45984;
        anchor = (uint32_t*)(uintptr_t)node;
        for (;;) {
            if ((int32_t)rd32((void*)(uintptr_t)node, 0x114) <= (int32_t)param_2) break;
            node = rd32((void*)(uintptr_t)node, 4);
            if ((int32_t)node < 1) {
                anchor = nullptr;
                break;
            }
        }
    }
    // unlink the fresh record (all links zero) then splice it after the anchor
    uint32_t link = rd32(rec, 0);
    if (link != 0) {
        uint32_t tag = rd32(rec, 4);
        uint8_t* prev;
        if ((int32_t)tag < 0) prev = (uint8_t*)(uintptr_t)~tag;
        else prev = (uint8_t*)(uintptr_t)((uint32_t)(uintptr_t)rec + (tag - rd32((void*)(uintptr_t)link, 4)));
        wr32(prev, 0, link);
        wr32((void*)(uintptr_t)link, 4, tag);
        wr32(rec, 0, 0);
        wr32(rec, 4, 0);
    }
    if (anchor == nullptr) anchor = &g_45980;
    uint32_t head = *anchor;
    wr32(rec, 0, head);
    wr32(rec, 4, rd32((void*)(uintptr_t)head, 4));
    wr32((void*)(uintptr_t)head, 4, (uint32_t)(uintptr_t)rec);
    *anchor = (uint32_t)(uintptr_t)rec;
    if (rd32(rec, 0x110) == 0) {
        if (local_110 != 0 && local_10c == 0) {
            Storm_Sub16CB0((uint32_t*)rec);
            g_lastError = 0xf;
            StormSetLastError(0xf);
            LeaveCriticalSection(&g_lock);
            return 0;
        }
    } else {
        Storm_Sub19560();
    }
    *param_4 = rec;
    LeaveCriticalSection(&g_lock);
    Storm_Sub19F60(rec, (void*)(uintptr_t)rd32(rec, 0x134), (int32_t)rd32(pHdr, 0x1c));
    return 1;
}

// WAVEFORMATEX as retail reads it (18 bytes, cbSize zeroed by the caller).
struct WaveFormat {
    uint16_t formatTag, channels;
    uint32_t samplesPerSec, avgBytesPerSec;
    uint16_t blockAlign, bitsPerSample, cbSize;
};

// 1.13c Storm+0x2A650: prepare a sound stream on an open file -- parse RIFF/WAVE, create the
// provider buffer, allocate and link the stream record, wake the sound thread.
extern "C"
uint32_t Storm_Sub2A650(uint8_t* param_1, uint32_t param_2, uint32_t param_3, uint32_t param_4,
                        uint32_t param_5, uint32_t param_6, void** param_7) {
    if (param_2 == 0) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return 0;
    }
    if (g_42ffc == nullptr) {
        g_lastError = 0x85100071;
        StormSetLastError(0x85100071);
        return 0;
    }
    EnterCriticalSection(&g_lock);
    if (rd32(param_1, 0x130) != 0) Storm_Sub181F0(param_1);
    if (((g_43130 | param_3) & 1) == 0 &&
        (rd32(param_1, 0x10c) != 0xFFFFFFFF || rd32(param_1, 0x110) == 0)) {
        g_lastError = 0x8510006F;
        StormSetLastError(0x8510006F);
        LeaveCriticalSection(&g_lock);
        return 0;
    }
    if (g_4313c == 0) g_4313c = 0x4000;
    g_43114 = g_4313c;
    if (g_43128 == 0) g_43128 = (uint32_t)(uintptr_t)Storm_Sub1C580(g_4313c, kSFileCpp, 0xcc4, 0);
    uint32_t iVar3 = rd32(param_1, 0x110);
    if (iVar3 != 0 && (rd32((void*)(uintptr_t)iVar3, 0x110) == 0 ||
                       rd32((void*)(uintptr_t)iVar3, 0x110) == 1))
        wr32((void*)(uintptr_t)iVar3, 0x110, 2);
    LeaveCriticalSection(&g_lock);
    uint32_t uVar2 = (g_43114 - 1) & param_2;
    if (uVar2 != 0) param_2 = param_2 + (g_43114 - uVar2);
    if (param_2 < g_43114 * 2) param_2 = g_43114 * 2;
    wr32(param_1, 0x148, 0);
    wr32(param_1, 0x14c, 0);
    EnterCriticalSection(&g_lock);
    if (rd32(param_1, 0x10c) == 0xFFFFFFFF) {
        wr32(param_1, 0x11c, 0);
        uint32_t uVar2c = rd32((void*)(uintptr_t)rd32(param_1, 0x114), 8) - 1;
        wr32(param_1, 0x11c, ~-(uint32_t)(uVar2c != 0) & uVar2c);
        wr32(param_1, 0x138, 0);
        wr32(param_1, 0x13c, 0);
        LeaveCriticalSection(&g_lock);
    } else {
        LeaveCriticalSection(&g_lock);
        StormSetFilePointer((HANDLE)(uintptr_t)rd32(param_1, 0x10c), 0, nullptr, 0);
    }
    uint32_t local_14[3] = {0, 0, 0};
    uint32_t local_54 = 0;
    WaveFormat wfSrc;
    WaveFormat wf;
    if (Storm_SFileReadFile_295B0(param_1, local_14, 0xc, nullptr, nullptr, 0, 0) != 0 &&
        local_14[0] == 0x46464952 && local_14[2] == 0x45564157) {
        int32_t found = Storm_Sub19DF0(0x20746D66, (int32_t*)&local_54);
        uVar2 = local_54;
        if (found != 0 && 0xF < local_54 &&
            Storm_SFileReadFile_295B0(param_1, (uint32_t*)&wfSrc, 0x10, nullptr, nullptr, 0, 0) != 0) {
            iVar3 = uVar2 - 0x10;
            EnterCriticalSection(&g_lock);
            if (rd32(param_1, 0x10c) == 0xFFFFFFFF) {
                int32_t seekBy = (int32_t)iVar3;
                if (seekBy < 0 && rd32(param_1, 0x11c) < (uint32_t)(-seekBy)) {
                    wr32(param_1, 0x11c, 0);
                } else {
                    wr32(param_1, 0x11c, rd32(param_1, 0x11c) + (uint32_t)seekBy);
                }
                uint32_t uVar4 = rd32((void*)(uintptr_t)rd32(param_1, 0x114), 8) - 1;
                uVar2 = rd32(param_1, 0x11c);
                if (uVar4 <= rd32(param_1, 0x11c)) uVar2 = uVar4;
                wr32(param_1, 0x11c, uVar2);
                wr32(param_1, 0x138, 0);
                wr32(param_1, 0x13c, 0);
                LeaveCriticalSection(&g_lock);
            } else {
                LeaveCriticalSection(&g_lock);
                uVar2 = StormSetFilePointer((HANDLE)(uintptr_t)rd32(param_1, 0x10c), (LONG)iVar3,
                                            nullptr, 1);
            }
            if (uVar2 != 0xFFFFFFFF) {
                std::memcpy(&wf, &wfSrc, 0x10);
                wf.cbSize = 0;
                found = Storm_Sub19DF0(0x61746164, (int32_t*)&local_54);
                if (found != 0) {
                    uVar2 = (param_4 != 0) ? param_4 % local_54 : 0;
                    bool noProvider = (param_7 == nullptr);
                    if (noProvider) {
                        uint8_t desc[0x18];
                        std::memset(desc, 0, sizeof(desc));
                        wr32(desc, 0x00, 0x14);   // local_28
                        wr32(desc, 0x04, 0x80);   // local_24
                        if ((param_3 & 0x20) != 0) wr32(desc, 0x04, 0xa0);
                        if ((param_3 & 0x40) != 0) wr32(desc, 0x04, rd32(desc, 4) | 0x40);
                        wr32(desc, 0x08, param_2);   // local_20
                        wr32(desc, 0x0c, 0);         // local_1c
                        wr32(desc, 0x10, (uint32_t)(uintptr_t)&wf);   // local_18
                        Storm_ProviderCreate(g_42ffc, desc, param_7, 0);
                        if (*param_7 == nullptr) return 0;
                        if (param_5 != 0 && param_5 != 0x7FFFFFFF)
                            Storm_ProviderSetVolume(*param_7, param_5);
                        if (param_6 != 0 && param_6 != 0x7FFFFFFF)
                            Storm_ProviderSetPan(*param_7, param_6);
                    }
                    EnterCriticalSection(&g_lock);
                    uint32_t* puVar5 = Storm_Sub4310(&g_459c4, 0, 0);
                    uint32_t riffSize = local_14[1];
                    uint32_t dataSize = local_54;
                    puVar5[0x10] = param_2;
                    puVar5[2] = (uint32_t)(uintptr_t)param_1;
                    puVar5[4] = wf.avgBytesPerSec;
                    puVar5[5] = (param_3 >> 0x12) & 1;
                    puVar5[8] = riffSize;
                    puVar5[9] = dataSize;
                    puVar5[10] = uVar2;
                    puVar5[0xb] = uVar2;
                    puVar5[0xc] = uVar2;
                    puVar5[0xe] = param_6;
                    puVar5[6] = 0;
                    puVar5[0xd] = param_5;
                    puVar5[0xf] = (uint32_t)(uintptr_t)*param_7;
                    puVar5[0x11] = (uint32_t)noProvider;
                    puVar5[0x13] = 1;
                    *(uint8_t*)(puVar5 + 0x12) = (uint8_t)((wf.bitsPerSample != 8) - 1u) & 0x80;
                    wr32(param_1, 0x140, rd32(param_1, 0x140) + 1);
                    Storm_ProviderPlay(*param_7, 0);
                    wr32(param_1, 0x130, 1);
                    LeaveCriticalSection(&g_lock);
                    Storm_Sub19560();
                    StormSetEvent(g_event_4312c);
                    return 1;
                }
            }
        }
    }
    g_lastError = 0xd;
    StormSetLastError(0xd);
    return 0;
}

// ---- 0x37ED0 string formatter state ------------------------------------------------------------
// 0x37ED0 builds the SCode display string for a string id: the id's bits index a control-code
// table (0x6FC369C0) and a padded name template table ("SCDDDDDD" + offset), then it merges the
// resulting control prefix with the template, rewrites '~' as '^1', and substitutes the 'A' slots.
// The two scratch strings sit at Ghidra's frame offsets -0x42 and -0x20 and overlap at -0x32 (the
// kernel buffer's first two bytes, unused), so the transcription uses one arena with those exact
// offsets. The .rdata tables are retail content and stay seams.
const uint8_t* Storm_ScdTemplate(uint32_t offset);                         // "SCDDDDDD" table
uint8_t Storm_Byte369C0(uint32_t i);                                       // 0x6FC369C0 byte table
const uint8_t* Storm_Ptr36938(void);                                       // 0x6FC36938 transform
uint32_t Storm_Dword3692C(void);                                           // 0x6FC3692C prefix
uint8_t Storm_Byte36930(void);                                             // 0x6FC36930 prefix
void Storm_Sub2AE00(uint8_t* dst, const uint8_t* table, uint32_t n);       // 0x2AE00 transform
void Storm_Sub37E80(uint32_t* state, uint32_t flag, uint32_t* out);        // 0x37E80, EAX = state

// 1.13c Storm+0x37ED0: format string id param_1 into param_3 (capacity param_3), using param_2 as
// the destination; param_4 is the hidden 0x37E80 state register (0 when the caller has none).
extern "C"
bool Storm_Sub37ED0(uint32_t param_1, int32_t param_2, char* param_3, int32_t param_4) {
    char* pcVar3 = param_3;
    if (param_1 == 0 || param_2 == 0 || param_3 == nullptr) {
        g_lastError = 0x57;
        StormSetLastError(0x57);
        return false;
    }
    if (param_4 != 0) {
        param_3 = nullptr;
        Storm_Sub37E80(nullptr, -(uint32_t)(param_2 != 0), (uint32_t*)&param_3);
    }
    uint8_t fr[0x200];
    std::memset(fr, 0, sizeof(fr));
    uint8_t* A = fr + 0x40;            // address of Ghidra's -0x42 frame boundary
    uint8_t* b1 = A + 2;               // local_40, the control-code string
    char* tmpl = (char*)(A + 0x10);    // acStack_32, the name template
    uint8_t* b2 = A + 0x22;            // local_20, the output string
    Storm_Sub2B080((uint8_t*)(tmpl + 2),
                   (const uint32_t*)Storm_ScdTemplate((param_1 & 3) + ((param_1 >> 2) & 7) * 0x10),
                   0x10);
    b1[0] = Storm_Byte369C0((param_1 >> 6) & 3);
    b1[1] = Storm_Byte369C0((param_1 >> 8) & 3);
    b1[2] = Storm_Byte369C0((param_1 >> 10) & 3);
    b1[4] = Storm_Byte369C0((param_1 >> 0xE) & 3);
    b1[3] = Storm_Byte369C0((param_1 >> 0xC) & 3);
    b1[5] = 0;
    if ((param_1 & 0x20) != 0) Storm_Sub2AE00(b1, Storm_Ptr36938(), 0x10);
    // strip trailing "~~" pairs
    uintptr_t len = std::strlen((char*)b1);
    if (len > 1) {
        do {
            len = std::strlen((char*)b1);
            int iVar8 = 3;
            bool bVar10 = true;
            uint8_t* p = A + len;
            const char* q = "~~";
            do {
                if (iVar8 == 0) break;
                iVar8--;
                bVar10 = *p == (uint8_t)*q;
                p++;
                q++;
            } while (bVar10);
            if (!bVar10) break;
            len = std::strlen((char*)b1);
            b1[len] = '\0';
            len = std::strlen((char*)b1);
        } while (len > 1);
    }
    // count the visible characters and grow the count for each 'A' slot
    int iVar8 = 0;
    uint8_t* puVar9 = b1;
    if ((int8_t)b1[0] != '\0') {
        int8_t cVar4 = (int8_t)b1[0];
        do {
            if (cVar4 != '~') iVar8++;
            cVar4 = (int8_t)puVar9[1];
            puVar9++;
        } while (cVar4 != '\0');
    }
    iVar8 = iVar8 + 1;
    int iVar6 = 0;
    if (0 < iVar8) {
        do {
            int iVar1 = iVar6 + 2;
            iVar6++;
            if (tmpl[iVar1] == 'A') iVar8++;
        } while (iVar6 < iVar8);
    }
    // output prefix and zero tail
    std::memset(A + 0x27, 0, 4);        // local_1b
    std::memset(A + 0x2B, 0, 4);        // local_17
    std::memset(A + 0x2F, 0, 4);        // local_13
    std::memset(A + 0x33, 0, 4);        // local_f
    std::memset(A + 0x37, 0, 4);        // local_b
    uint32_t prefix = Storm_Dword3692C();
    std::memcpy(A + 0x22, &prefix, 4);
    std::memset(A + 0x3B, 0, 4);        // local_7
    std::memset(A + 0x3F, 0, 2);        // local_3
    A[0x26] = Storm_Byte36930();        // local_1c
    A[0x41] = 0;                        // local_1
    uintptr_t n = std::strlen((char*)b2);
    bool bVar10 = false;
    iVar6 = iVar8 - 1;
    b2[n] = (uint8_t)tmpl[iVar8 + 1];
    uint8_t* pcVar7 = b2 + n + 1;
    uint8_t* pcVar5 = b1;
    int8_t cVar4 = (int8_t)b1[0];
    while (cVar4 != '\0') {
        if (*pcVar5 == '~') {
            *pcVar7 = '^';
            pcVar7[1] = '1';
            pcVar7 += 2;
            pcVar5++;
        } else {
            if (tmpl[iVar6 + 1] != 'A' || bVar10) {
                *pcVar7 = *pcVar5;
                pcVar7++;
                pcVar5++;
                iVar8 = -1;
                *pcVar7 = (uint8_t)tmpl[iVar6 + 1];
            } else {
                cVar4 = tmpl[iVar6];
                *pcVar7 = ' ';
                pcVar7[1] = 'D';
                pcVar7[2] = '=';
                pcVar7 += 3;
                iVar8 = -2;
                *pcVar7 = (uint8_t)cVar4;
                bVar10 = true;
            }
            iVar6 += iVar8;
            pcVar7++;
        }
        cVar4 = (int8_t)*pcVar5;
    }
    *pcVar7 = '\0';
    Storm_Sub2B080((uint8_t*)(uintptr_t)param_2, (const uint32_t*)b2, (uint32_t)(uintptr_t)pcVar3);
    return (bool)(1 - ((uintptr_t)pcVar3 < (uintptr_t)(pcVar7 - b2) + 1));
}

}  // namespace
