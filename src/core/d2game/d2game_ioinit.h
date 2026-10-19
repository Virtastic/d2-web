#pragma once
#include <cstddef>
#include <cstdint>

namespace d2game_crt {
// Serialized x86 CRT descriptor; native pointers live only in State and Host.
struct Descriptor {
    uint32_t handle;
    uint8_t flags;
    uint8_t pipeCharacter;
    uint8_t padding[2];
    uint32_t lockInitialized;
    uint8_t criticalSection[24];
};
static_assert(sizeof(Descriptor) == 36);
static_assert(offsetof(Descriptor, flags) == 4);
static_assert(offsetof(Descriptor, pipeCharacter) == 5);
static_assert(offsetof(Descriptor, lockInitialized) == 8);
static_assert(offsetof(Descriptor, criticalSection) == 12);
struct State {
    int32_t capacity;
    Descriptor* blocks[64];
};
struct Host {
    void* context;
    Descriptor* (*allocateBlock)(void*); // 32 descriptors; uninitialized storage
    uint32_t (*fileType)(void*, uint32_t);
    uint32_t (*standardHandle)(void*, uint32_t);
    bool (*initializeLock)(void*, uint8_t*, uint32_t);
    void (*setHandleCount)(void*, uint32_t);
};
struct Startup {
    // Caller decodes STARTUPINFO's reserved blob: count, count flag bytes,
    // then count little-endian 32-bit handles. Keep the original count when
    // locating handles, before the initializer clamps its iteration count.
    bool present;
    int32_t count;
    const uint8_t* flags;
    const uint32_t* handles;
};
}
extern "C" int32_t d2_fn_d2game_60ef(d2game_crt::State*,
    const d2game_crt::Startup*, const d2game_crt::Host*);
