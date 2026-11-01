// 1.13c D2Common+0x44A30
// Grow a room's neighbour array by one: EAX = room, one stack word = the value to append (callee pops it). The count at room +0x2C is
// incremented and stored first, then the array at room +8 is reallocated through Fog's realloc thunk 0x9252 (EDX = old array, stack: new size,
// file DrlgRoom.cpp, line 0x314, 0) and the result stored at room +8; a null result is fatal (line 0x315). The value goes into the last slot
// and the sort 0x445A0 runs with EAX = count, ESI = array; its EAX is the function's. Links are 32-bit retail tokens: `resolve` turns one
// into an address, `realloc` and `sort` are the callee seams.
#include <cstdint>
#include <cstring>

extern "C" uint32_t D2Common_44A30(uint8_t* pRoom, uint32_t value, uint8_t* (*resolve)(uint32_t),
                                   uint32_t (*realloc)(uint32_t old, uint32_t size, uint32_t line),
                                   uint32_t (*sort)(uint32_t count, uint32_t array))
{
    auto rd = [](const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; };
    auto wr = [](uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); };
    const uint32_t count = rd(pRoom, 0x2C) + 1;
    wr(pRoom, 0x2C, count);
    const uint32_t array = realloc(rd(pRoom, 8), count * 4, 0x314);
    wr(pRoom, 8, array);
    if (!array) __builtin_trap();   // retail: fatal error 0x315, then exit(-1)
    wr(resolve(array), rd(pRoom, 0x2C) * 4 - 4, value);
    return sort(rd(pRoom, 0x2C), rd(pRoom, 8));
}
