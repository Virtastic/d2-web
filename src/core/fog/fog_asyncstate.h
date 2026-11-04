// src/core/fog/fog_asyncstate.h -- 1.13c Fog AsyncData.cpp event-slot table (+0x1D7F0 / +0x1D870 / +0x1DF00) and the
// shared fatal path: module globals as a state struct, module-internal and Win32 callees as seams. Retail slot = 12 bytes
// {u32 event, u32 busy, u32 req}; the table is u8* and every slot word is read as u32.
#pragma once
#include <cstdint>

// Shared fatal path: capture ip, WSOCK32 ordinal 116, log at level 6, abort, _exit(-1).
struct FogN41Fatal {
    uint32_t (*capture_ip)(void);                                                         // Fog+0xFC20
    void (*ordinal116)(void);
    void (*fatal)(int level, const char* file, int line, const char* fmt, ...);      // Fog+0x10F30
    void (*abort)(void);                                                             // Fog+0x184A0
    void (*exit_process)(int code);                                                  // _exit
};
// --- AsyncData event table: 256 entries of 12 bytes (HANDLE, 1, pad) at DAT_6ff9af70 -----------
struct FogAsyncState {
    uint8_t* table;        // DAT_6ff9af70
    uint32_t count;        // DAT_6ff9af6c
    uint32_t initialized;  // DAT_6ff9af68
    int dataBase;     // DAT_6ff9af60
    uint8_t* pending;      // DAT_6ff9af64 (list head)
};
struct FogAsyncApi {
    void* (*alloc)(void* ctx);                                                       // Fog+0x1B560
    void (*free_)(void* p);                                                          // Fog+0x1B0C0
    void (*closeHandle)(uint32_t h);
    uint32_t (*createEvent)(void);                                                        // CreateEventA(0,1,0,0)
    void (*setLimit)(uint32_t v);                                                         // Ordinal_284 / 285 / 282 seam
    void (*setHandleLimit)(uint32_t handle, int v);                                       // Ordinal_282
    FogN41Fatal die;
    void (*logAlloc)(int level, const char* file, int line, const char* fmt);        // Fog+0x10F30
    void (*setLimit285)(uint32_t v);                                                  // Storm Ordinal_285
    void* (*allocSized)(uint32_t size, void* pool, const char* file, int line);       // Fog+0x1B560 (EAX size, ECX pool)
    void (*freeBlock)(void** slot, const char* file, int line);                       // Fog+0x1B0C0 (ECX = slot, EAX = 0)
    uint32_t (*waitZero)(uint32_t h);                                                 // WaitForSingleObject(h, 0)
    void (*complete)(uint32_t req);                                                   // Fog+0x1D740 (EAX = the slot's request)
};
constexpr const char* kFogAsyncFile = "..\\Source\\Fog\\Src\\AsyncData.cpp";
