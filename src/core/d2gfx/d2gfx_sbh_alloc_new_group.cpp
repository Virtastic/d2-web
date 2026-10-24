// D2Gfx VC7.1 small-block heap: new group. Guest 32-bit addresses are
// resolved through host->base so the layout stays 1.13c's.
#include <cstdint>


extern "C" void* D2Gfx_VirtualAlloc(void* addr, uint32_t n, uint32_t type, uint32_t prot);
static inline uint32_t* W(uint32_t a) { return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); }

// 1.13c D2Gfx+0x047A2 ___sbh_alloc_new_group(region)
extern "C" int32_t D2Gfx_47A2_SbhAllocNewGroup(uint32_t region) {
    uint32_t hdr = *W(region + 0x10);
    int32_t idx = 0;
    for (int32_t m = (int32_t)*W(region + 8); m >= 0; m <<= 1) idx++;
    uint32_t list = idx * 0x204 + 0x144 + hdr;
    uint32_t p = list;
    for (int i = 0; i < 0x3f; i++, p += 8) { *W(p + 8) = p; *W(p + 4) = p; }
    uint32_t addr = idx * 0x8000 + *W(region + 0xc);
    if (!D2Gfx_VirtualAlloc((void*)(uintptr_t)addr, 0x8000, 0x1000, 4)) return -1;
    uint32_t q = addr + 0x10;
    if (addr <= addr + 0x7000u)  // retail unsigned address-wrap gate
    for (int i = 0; i < 8; i++, q += 0x1000) {
        *W(q - 8) = (uint32_t)-1;
        *W(q + 0xfec) = (uint32_t)-1;
        *W(q) = q + 0xffc;
        *W(q - 4) = 0xff0;
        *W(q + 4) = q - 0x1004;
        *W(q + 0xfe8) = 0xff0;
    }
    *W(list + 0x1fc) = addr + 0xc;
    *W(addr + 0x14) = list + 0x1f8;
    *W(list + 0x200) = addr + 0x700c;
    *W(addr + 0x7010) = list + 0x1f8;
    *W(hdr + 0x44 + idx * 4) = 0;
    *W(hdr + 0xc4 + idx * 4) = 1;
    uint8_t* c = (uint8_t*)(uintptr_t)(hdr + 0x43);
    if ((*c)++ == 0) *W(region + 4) |= 1;
    *W(region + 8) &= ~(0x80000000u >> (idx & 0x1f));
    return idx;
}

