#pragma once
#include <cstdint>
#include "d2game_sbh_state.h"

// Native heap handles stay native-width; the CRT's platform words and mode stay 32-bit.
struct D2GameHeapInitEnv {
    void* (*heap_create)(uint32_t options, uint32_t initial_commit, uint32_t maximum_size);
    int32_t (*heap_destroy)(void* heap);
    int32_t (*sbh_heap_init)(uint32_t threshold);
    uint32_t os_platform;
    uint32_t os_major;
    void* heap;          // retail global 0x6FD3A20C
    uint32_t heap_mode;  // retail global 0x6FD3A210
    D2GameSbhState sbh{};
    D2GameSbhHeapAlloc heap_alloc{};
};

extern "C" int32_t d2game_heap_init_674a(int32_t mtflag, D2GameHeapInitEnv* env);
