// 1.13c D2Game+0x5FB50 -- A5Q5 quest record init (Quests/a5q5.cpp). Retail is 32-bit: offsets are bytes,
// handler slots hold retail addresses (base 0x6FC20000). Allocator is a seam (Fog #10045 @0xA2E6, unverified).
#include <cstdint>
#include <cstring>

static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2GameBatch5FB50Env {
    void* (*allocPool)(const char* file, uint32_t line, uint32_t zero); // 0xA2E6, Fog #10045 (unverified)
};

// 1.13c D2Game+0x5FB50
void D2Game_5FB50_A5Q5Init(uint8_t* q, const D2GameBatch5FB50Env* env) {
    std::memset(q + 0xA0, 0, 0xF * 4);
    q[0x0C] = 0;
    wr32(q, 0xA0, 0x6FC775F0);
    wr32(q, 0xC8, 0x6FC775D0);
    wr32(q, 0xAC, 0x6FC7AB60);
    wr32(q, 0xC0, 0x6FC7EED0);
    wr32(q, 0xA8, 0x6FC79BE0);
    wr32(q, 0xCC, 0x6FC7F200);
    wr32(q, 0xD4, 0x6FC77540);
    wr32(q, 0xDC, 0x6FD2CAC0);
    q[0x0A] = 1;
    void* p = env->allocPool("..\\Source\\D2Game/Quests/a5q5.cpp", 0x41D, 0);
    wr32(q, 0x18, (uint32_t)(uintptr_t)p);
    wr32(q, 0xE0, 0x27);
    wr32(q, 0xE8, 0);
    q[0x0D] = 4;
    wr32(q, 0xEC, 0x6FC76810);
    wr32(q, 0xF0, 0x6FC767A0);
    wr32(q, 0x10, 0x24);
    std::memset(p, 0, 0x19 * 4);
}
