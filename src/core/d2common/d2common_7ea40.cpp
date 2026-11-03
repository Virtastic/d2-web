// 1.13c D2Common+0x7EA40 -- sub_7ea40. Raw-pointer port: EAX=level, EDI=x,
// two stack out-pointers. Scans rows i=1..count-2 for table[B[i]+x]==0x1c with the flag low nibble 1 at row i.
#include <cstdint>

namespace {
inline uint32_t rd(uintptr_t p) { return *reinterpret_cast<const uint32_t*>(p); }
}

extern "C" void d2c_7ea40_port(uintptr_t level, uint32_t edi, int32_t* out1, int32_t* out2) {
    const uintptr_t od = rd(level + 0x14);
    const int32_t count = (int32_t)rd(od + 0x5c) - 1;
    const uintptr_t A = rd(od + 4);
    const uintptr_t B = rd(od + 8);
    const uintptr_t flags = rd(od + 0x2c);
    const uintptr_t rows = rd(od + 0x30);
    const int32_t x = (int32_t)edi;
    for (int32_t i = 1; i < count; ++i) {
        const int32_t t = (int32_t)rd(A + 4u * (uint32_t)((int32_t)rd(B + 4u * (uint32_t)i) + x));
        if (t == 0x1c) {
            const int32_t row = (int32_t)rd(rows + 4u * (uint32_t)i);
            if ((*reinterpret_cast<const uint8_t*>(flags + 2 + 4u * (uint32_t)(row + x))) & 0xf) {
                if (((*reinterpret_cast<const uint8_t*>(flags + 2 + 4u * (uint32_t)(row + x))) & 0xf) == 1) {
                    *out1 = x;
                    *out2 = i;
                    return;
                }
            }
        }
    }
    *out1 = -1;
    *out2 = -1;
}
