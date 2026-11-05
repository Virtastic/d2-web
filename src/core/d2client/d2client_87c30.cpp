// 1.13c D2Client+0x87C30 (129 B, stdcall p1, p2, p3): oldest playing sound entry. List node = {item, next};
// slot = table + item[1] * 0x8E; Ordinal_10053 (thunk 0xD264) is called with ECX = item[0x28], EDX = p2.
// Retail faults on an out-of-range index (slot 0 + 0x60); here that reads as 0 and is not in the vectors.
#include <emscripten.h>
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { return a ? *(const uint32_t*)(uintptr_t)a : 0; }
extern "C" uint32_t d2c_port_87c30(uint32_t list, uint32_t table, int32_t count, int32_t (*o10053)(uint32_t ecx, uint32_t edx),
                                   int32_t p1, int32_t p2, uint32_t p3) {
    uint32_t best = 0;
    for (uint32_t n = list; n; n = rd32(n + 4)) {
        uint32_t it = rd32(n);
        int32_t i = (int32_t)rd32(it + 4);
        uint32_t s = (table == 0 || i < 0 || count <= i) ? 0 : (uint32_t)i * 0x8e + table;
        if ((int32_t)rd32(s + 0x60) == p1 && it != p3 && rd32(it + 0x30) == 1 &&
            (p2 == 0 || o10053(rd32(it + 0x28), (uint32_t)p2) != 0) &&
            (best == 0 || rd32(it + 0x34) < rd32(best + 0x34)))
            best = it;
    }
    return best;
}
