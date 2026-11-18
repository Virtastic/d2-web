#pragma once
// Retail-layout (1.13c) packet queue append of D2Game.dll on the 32-bit client/block layout: D2Game+0x8A3E0 (the block append, with the pool
// allocation, Ordinal 10045, as a seam) and +0x8B340 (appends the two bytes {0x77, b} through it). The client pointers are the retail 32-bit
// ones; a replay runs them as wasm pointers, which fit in the low 32 bits.
#include <cstddef>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1)
struct D2G8B340Client { uint8_t pad[0x1B8]; uint32_t head, tail, freeList; };
struct D2G8B340Block { uint32_t count; uint8_t data[0x200]; uint32_t next; };
#pragma pack(pop)
static_assert(offsetof(D2G8B340Client, head) == 0x1B8 && offsetof(D2G8B340Client, tail) == 0x1BC && offsetof(D2G8B340Client, freeList) == 0x1C0, "client fields");
static_assert(offsetof(D2G8B340Block, data) == 4 && offsetof(D2G8B340Block, next) == 0x204 && sizeof(D2G8B340Block) == 0x208, "block fields");

// 1.13c D2Game+0x8A3E0 on the 32-bit layout
template <class Ptr, class Alloc> static void GAME_8A3E0_32(D2G8B340Client* c, const uint8_t* src, uint32_t len, Ptr blk, Alloc alloc)
{
    if (!c) return;
    D2G8B340Block* b = blk(c->tail);
    uint32_t n = b ? b->count : 0;
    if ((int32_t)(n + len) > 0x200 || !b) {
        b = blk(c->freeList);
        n = 0;
        if (!b) b = alloc(); else c->freeList = b->next;
        memset(b, 0, sizeof *b);
        if (!c->tail) c->head = (uint32_t)(uintptr_t)b; else blk(c->tail)->next = (uint32_t)(uintptr_t)b;
        c->tail = (uint32_t)(uintptr_t)b;
    }
    memcpy(b->data + n, src, len);
    b->count += len;
}

// 1.13c D2Game+0x8B340 (AL = b, stack = client, ret 4): appends the two bytes {0x77, b} to the client's SCmd block list.
template <class Alloc> static void GAME_8B340(D2G8B340Client* client, uint8_t b, Alloc alloc)
{
    const uint8_t msg[2] = { 0x77, b };
    GAME_8A3E0_32(client, msg, 2, [](uint32_t p) { return (D2G8B340Block*)(uintptr_t)p; }, alloc);
}
