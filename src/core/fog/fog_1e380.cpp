// 1.13c Fog+0x1E380 (#10078): stores three words at +0x118/+0x11c/+0x120 of a struct (stdcall, void).
#include <cstdint>
// 1.13c Fog+0x1E380
void fog_sub_1e380(uint8_t* p, uint32_t a, uint32_t b, uint32_t c) {
    if (p) { *(uint32_t*)(p + 0x118) = a; *(uint32_t*)(p + 0x11c) = b; *(uint32_t*)(p + 0x120) = c; }
}
