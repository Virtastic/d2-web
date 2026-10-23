// Rules from cached 1.13c body. Fog alloc, fatal (message + _exit(-1)) and 0x12F20 are injected seams.
#include <cstdint>
#include <cstring>

// 1.13c D2CMP+0x15C70
// ESI=lib. Allocs a zeroed 0x200-byte index (Fog 10042 size 0x2a), then calls 0x12F20(index)
// lib+0x10c times. Null alloc -> fatal(0x2b); null lib -> fatal(0x3ba).
extern "C" uint32_t* d2cmp_15c70(const uint8_t* lib, void* (*alloc)(int line),
                                 void (*fatal)(int id), void (*add)(uint32_t* index)) {
    uint32_t* idx = (uint32_t*)alloc(0x2a);
    if (!idx) { fatal(0x2b); return nullptr; }
    memset(idx, 0, 0x200);
    if (!lib) { fatal(0x3ba); return nullptr; }
    for (int32_t i = 0; i < *(const int32_t*)(lib + 0x10c); i++) add(idx);
    return idx;
}
