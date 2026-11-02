#pragma once
#include <cstdint>
#include <cstddef>
#include "storm_state.h"
struct StormState3b1e0 {
    uint32_t lastError, nextLogId;
    StormLog3b1e0* logs[4];
};
struct StormEventNode3b1e0 {
    StormEventNode3b1e0* next;
    StormEventNode3b1e0* previous;
    const void* messageData;
};
// List and lock values below are module-relative retail identities, never pointers.
// Internal callees are explicit ABI seams, including hidden register arguments.
// State belongs to the caller, so all twelve entries observe the same module.
struct StormCalls3b1e0 {
    void (*error)(uint32_t);
    int32_t (*strnicmp)(const char*, const char*, uint32_t);
    char* (*strupr)(char*);
    void (*enter)(uint32_t); void (*leave)(uint32_t);
    StormEventNode3b1e0* (*eventNode)(uint32_t list, uint32_t arg2, uint32_t flags, uint32_t insertionMode); // 0x1560; EDI is consumed by 0x4390
    const void* (*resolveMessageData)(uint32_t retailToken);
    void (*event)(uint32_t command, uint32_t key, uint32_t, uint32_t); // 0x34410, ESI
    uint32_t (*regionId)(uint32_t list); // 0x24f0, list 0x458f0
    uint32_t* (*regionCreate)(uint32_t list, uint32_t, uint32_t, uint32_t); // 0x25f0
    void* (*regionFind)(uint32_t list, uint32_t); // 0x2690, ESI handle
    void (*regionClear)(void*); // 0x3000, EAX node
    void (*regionRelease)(uint32_t list, void*); // list vtable slot 0
    void* (*createFile)(const char*, uint32_t access, uint32_t sharing, void* security,
                        uint32_t disposition, uint32_t attributes, void* templateFile);
    StormLog3b1e0* (*logAllocate)(uint32_t id, uint32_t* bucket, uint32_t create); // 0x347b0
    void (*write)(void*, const void*, uint32_t, uint32_t*, void* overlapped);
    void (*flush)(void*); void (*close)(void*);
    void (*logRelease)(StormLog3b1e0*, uint32_t bucket); // 0x346e0 also releases bucket lock
};
extern "C" int32_t Storm_Sub3B1E0(const char* a, const char* b, uint32_t count,
                                  StormState3b1e0& s, const StormCalls3b1e0& c);
extern "C" void Storm_Sub3A860(char* text, const StormCalls3b1e0& c);
