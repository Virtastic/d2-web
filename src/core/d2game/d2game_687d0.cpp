// 1.13c D2Game+0x687D0 -- A1Q4 quest-record initializer. ECX = the quest record.
// Zeroes the 15 callback dwords at +0xA0, fills the callback slots, then allocates the 0x1BC-byte per-quest data (pool [[q+4]+0x1C],
// file a1q4.cpp line 0x95F) and zeroes it. Pure rule + seams: cb(rva) yields the code address of a D2Game function (or the data
// address 0x1093C8 for slot +0xDC), alloc is the Fog pool allocator (D2Game+0xA2E6).
#include <cstdint>
#include <cstring>

struct D2Game687D0Env {
    uint32_t (*cb)(uint32_t rva);
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line, uint32_t zero);
};

static inline void w32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

void D2Game_687D0(uint8_t* q, const D2Game687D0Env& e) {
    std::memset(q + 0xa0, 0, 0x3c);
    w32(q, 0xa0, e.cb(0x66960));
    w32(q, 0xac, e.cb(0x684b0));
    w32(q, 0xb0, e.cb(0x63570));
    w32(q, 0xb8, e.cb(0x65440));
    w32(q, 0xd8, e.cb(0x618d0));
    w32(q, 0xc4, e.cb(0x654a0));
    w32(q, 0xc0, e.cb(0x65a70));
    w32(q, 0xdc, e.cb(0x1093c8));
    w32(q, 0xe0, 4);
    q[0xb] = 0; q[0xa] = 1;
    w32(q, 0xe8, 0);
    q[0xd] = 6;
    w32(q, 0xec, e.cb(0x61750));
    w32(q, 0xcc, e.cb(0x66ce0));
    w32(q, 0xc8, e.cb(0x61880));
    w32(q, 0xa8, e.cb(0x635d0));
    w32(q, 0xd4, e.cb(0x61540));
    w32(q, 0xf0, e.cb(0x646d0));
    w32(q, 0x10, 3);
    uint32_t pool; { uint32_t g; std::memcpy(&g, q + 4, 4); std::memcpy(&pool, (uint8_t*)(uintptr_t)g + 0x1c, 4); }
    const uint32_t d = e.alloc(pool, 0x1bc, 0x95f, 0);
    w32(q, 0x18, d);
    std::memset((void*)(uintptr_t)d, 0, 0x6f * 4);   // the field stores that follow in retail are zero over the zeroed block
}
