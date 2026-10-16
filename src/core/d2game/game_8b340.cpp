// 1.13c D2Game+0x8B340 (AL = b, stack = client, ret 4) over D2Game+0x8A3E0 (EAX = client): appends the two bytes {0x77, b} to the client's
// SCmd block list. Retail 32-bit layout; the pool allocation (Ordinal 10045) is a seam.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#pragma pack(push, 1)
struct D2G8B340Client { uint8_t pad[0x1B8]; uint32_t head, tail, freeList; };
struct D2G8B340Block { uint32_t count; uint8_t data[0x200]; uint32_t next; };
#pragma pack(pop)
static_assert(offsetof(D2G8B340Client, head) == 0x1B8 && offsetof(D2G8B340Client, tail) == 0x1BC && offsetof(D2G8B340Client, freeList) == 0x1C0, "client fields");
static_assert(offsetof(D2G8B340Block, data) == 4 && offsetof(D2G8B340Block, next) == 0x204 && sizeof(D2G8B340Block) == 0x208, "block fields");

template <class Ptr, class Alloc> static void GAME_8A3E0(D2G8B340Client* c, const uint8_t* src, uint32_t len, Ptr blk, Alloc alloc)
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

// ponytail: the 1.13c client pointers are 32-bit; the replay runs them as wasm pointers, which fit in the low 32 bits here.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8b340(uint32_t* r)
{
    const uint8_t msg[2] = { 0x77, (uint8_t)r[0] };
    GAME_8A3E0((D2G8B340Client*)(uintptr_t)r[1], msg, 2, [](uint32_t p) { return (D2G8B340Block*)(uintptr_t)p; }, []() -> D2G8B340Block* { return nullptr; });
    r[4] = 0;
}
