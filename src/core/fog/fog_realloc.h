// src/core/fog/fog_realloc.h -- the Fog pool realloc seam, for its FUNCTION probe.
// The definitions mirror src/core/fog/fog_n50.cpp lines 7 and 205-216 token for token (fog_n50.cpp is a shared
// batch file and stays as it is); keep the two in step. Host pointers are wasm64-width, so the probe builds these
// objects from the retail 32-bit pool and info blobs.
#pragma once
#include <cstdint>

struct QSLock { void* os; };
struct FogPoolInfo { QSLock lock; int32_t elemSize; int32_t elemCount; };
struct FogPool { uint8_t* base; uint32_t* bitmap; FogPoolInfo* info; };
struct FogReallocHost {
    void* user;
    uint8_t* (*allocFrom)(void*, void* arg);        // Fog+0xB560
    uint8_t* (*allocSize)(void*, int size);         // Fog+0xB9F0
    FogPool* (*findPool)(void*, uint8_t* p);        // Fog+0xA9F0
    void     (*freeBlk)(void*, uint8_t* p);         // Fog+0xB0C0
    void     (*enter)(void*, QSLock*);
    void     (*leave)(void*, QSLock*);
    void     (*fatal)(void*, int line);             // never returns
};

uint8_t* fog_realloc_bdd0(const FogReallocHost* h, void* arg, uint8_t* p, int size);
