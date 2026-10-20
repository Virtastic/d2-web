#pragma once
#include <cstdint>

// D2Game VC7.1 small-block-heap globals. Region records retain retail's 20-byte
// serialized layout; addresses of the table and its cursors are native pointers.
struct D2GameSbhState {
    void* pending_region; // +0x119FB4
    uint32_t region_count; // +0x119FB8
    void* regions; // +0x119FBC
    uint32_t maximum_request; // +0x119FC0
    void* cursor; // +0x119FC4
    uint32_t capacity; // +0x119FC8
};
using D2GameSbhHeapAlloc = void* (*)(void* heap, uint32_t flags, uint32_t size);
extern "C" int32_t d2game_sbh_heap_init_8bd5(uint32_t maximum, void* heap,
    D2GameSbhState* state, D2GameSbhHeapAlloc heap_alloc);
