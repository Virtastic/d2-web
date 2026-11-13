// src/core/d2game/d2game_9BCB0.cpp: 1.13c D2Game.dll sub_9BCB0, linked-list membership test.
// head (EAX) points to a record whose +4 is the first node; node+0 key, node+4 next. Null head is fatal (#10265(0xe5)/#10024, _exit(-1)).
// The record is retail's 32-bit layout (dword links), read as dwords: a pointer-typed walk would be 8 bytes a link in wasm64.
#include <cstdint>
#include <cstring>
#include <unistd.h>

static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x9BCB0
uint32_t d2game_9BCB0(void* head, int key) {
    if (!head) _exit(-1);
    for (uint32_t n = rd32((uint32_t)(uintptr_t)head + 4); n; n = rd32(n + 4))
        if ((int)rd32(n) == key) return 1;
    return 0;
}
