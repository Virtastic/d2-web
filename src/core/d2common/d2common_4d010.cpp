// 1.13c D2Common+0x4D010 -- size and zero a room's collision grid, then stamp each entry's three spans.
// Stdcall, two stack arguments (ret 8). Argument 0 is not read. Argument 1 is the room: 0x3BA60 fills an
// 8-dword rect from it (rect[2] = h, rect[3] = w); the block is 0x24 + 4*w + 2*w*h bytes (allocator 0x9222,
// size in EDX, file 0x185 and zero on the stack). The first 8 dwords of the block are the rect, block+0x20 is
// the data pointer (block+0x24), and 2*w*h bytes are zeroed there. The room's grid word (+0x20) is the block.
// Each entry then gets three stamps through 0x4CA90 (ECX = a span word, ESI = the block, stack: the entry's
// flag at +0x20, a count, 0), in the order span+8/+0xC, span+0/+4, span+0x10/+0x14.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2Common4D010Room {    // retail layout, 4-byte pointers
    uint32_t entries;         // +0x00: entry pointer array
    uint32_t _04[7];          // +0x04..0x1F
    uint32_t grid;            // +0x20: the block (written here)
    int32_t  count;           // +0x24: entry count
};
struct D2Common4D010Entry {
    uint32_t _00[2];          // +0x00..0x07
    uint32_t span;            // +0x08: D2Common4D010Span*
    uint32_t _0c[5];          // +0x0C..0x1F
    uint32_t flag;            // +0x20
};
struct D2Common4D010Span {
    uint32_t p0;              // +0x00 (stamp 2 word)
    int32_t  n4;              // +0x04 (stamp 2 count)
    uint32_t p8;              // +0x08 (stamp 1 word)
    int32_t  n12;             // +0x0C (stamp 1 count)
    uint32_t p16;             // +0x10 (stamp 3 word)
    int32_t  n20;             // +0x14 (stamp 3 count)
};
static_assert(offsetof(D2Common4D010Room, grid) == 0x20, "grid at +0x20");
static_assert(offsetof(D2Common4D010Room, count) == 0x24, "count at +0x24");
static_assert(offsetof(D2Common4D010Entry, span) == 0x08, "span at +0x08");
static_assert(offsetof(D2Common4D010Entry, flag) == 0x20, "flag at +0x20");
static_assert(offsetof(D2Common4D010Span, n20) == 0x14, "span is 0x18 bytes");

struct D2Common4D010Env {
    void     (*rect)(const D2Common4D010Room* room, uint32_t* out8);                  // 0x3BA60
    uint32_t (*alloc)(uint32_t size, uint32_t line, uint32_t zero);                  // 0x9222 (file on the stack)
    void     (*stamp)(uint32_t ptr, uint32_t grid, uint32_t flag, int32_t count, uint32_t zero); // 0x4CA90
    void     (*fatal)(uint32_t code);                                                // 0x2C5 / 0xB1 / 0xBE report
};

extern "C" void D2Common_4D010_Build(uint32_t unusedArg, D2Common4D010Room* room, const D2Common4D010Env* env)
{
    (void)unusedArg;
    if (room == nullptr)
        return;
    uint32_t rect[8] = {};
    env->rect(room, rect);
    const uint32_t h = rect[2];
    const uint32_t w = rect[3];
    const uint32_t size = 0x24u + 4u * w + 2u * w * h;
    uint8_t* blk = (uint8_t*)(uintptr_t)env->alloc(size, 0x185u, 0u);
    std::memcpy(blk, rect, 0x20);
    const uint32_t data = (uint32_t)(uintptr_t)(blk + 0x24);
    std::memcpy(blk + 0x20, &data, 4);
    const uint32_t grid = (uint32_t)(uintptr_t)blk;
    room->grid = grid;
    std::memset(blk + 0x24, 0, 2u * w * h);

    const int32_t n = room->count;
    const uint32_t* ents = (const uint32_t*)(uintptr_t)room->entries;
    for (int32_t i = 0; i < n; ++i) {
        const D2Common4D010Entry* e = (const D2Common4D010Entry*)(uintptr_t)ents[i];
        if (e == nullptr) { env->fatal(0x2C5); return; }
        const uint32_t flag = e->flag;
        const D2Common4D010Span* s = (const D2Common4D010Span*)(uintptr_t)e->span;
        if (s != nullptr) env->stamp(s->p8, grid, flag, s->n12, 0u);
        else              env->stamp(0u, grid, flag, 0, 0u);
        // retail reads the span without a null test for the next two stamps
        env->stamp(s->p0, grid, flag, s->n4, 0u);
        env->stamp(s->p16, grid, flag, s->n20, 0u);
    }
}
