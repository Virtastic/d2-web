// 1.13c D2Common+0x7EE50 on the retail level layout (EAX = level, ECX forwarded and unused). The level struct is read as retail's
// 32-bit blob (the wasm32 host addresses of the info block and grids), the 0x42A70 stamps run through the exact port, and the three
// placement callees that need the whole DRLG (border fixup 0x7ECC0, preset placement 0x42EB0 / 0x42CE0) are seams.
#include <cstdint>
#include <cstring>

extern "C" void D2Common_42A70(void*, int32_t, int32_t, int32_t, int32_t, int32_t);   // d2common_42a70.cpp

struct D2c7EE50Env {
    void (*border)(void* level);            // 0x7ECC0
    int (*placeFixed)(void* level);         // 0x42EB0 (level type 2, via 0x2DD80) / 0x42CE0: nonzero when placed
};
namespace {
inline uint32_t r32(const void* p, int o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void w32(void* p, int o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// 0x7E9F0: the border probe at column px: 0 when any row's two cells there have bit 2 set
bool borderClear(const uint8_t* info, int32_t px)
{
    const uint32_t* cells = (const uint32_t*)(uintptr_t)r32(info, 0x2c);
    const int32_t* ro = (const int32_t*)(uintptr_t)r32(info, 0x30);
    for (int32_t y = 0; y < (int32_t)r32(info, 0x60); ++y)
        if ((cells[ro[y] + px] & 2) || (cells[ro[y] + px + 1] & 2)) return false;
    return true;
}
}

// Returns what retail leaves in EAX: the level pointer (type 0x27), else the flag dword it reloads before the 0x400 test, or
// 0x42A70's EAX (the grid-C cell base at info+0x04) when that stamp runs, or the placement callee's result when it runs.
extern "C" uint32_t D2Common_7EE50_raw(void* level, const D2c7EE50Env* e)
{
    if ((int32_t)r32(level, 0x1d0) == 0x27) return (uint32_t)(uintptr_t)level;
    uint8_t* info = (uint8_t*)(uintptr_t)r32(level, 0x14);
    const int32_t w = (int32_t)r32(info, 0x5c), h = (int32_t)r32(info, 0x60);
    if ((r32(info, 0) & 0x10) && borderClear(info, w / 2 - 1)) e->border(level);
    if ((int8_t)r32(info, 0) < 0) D2Common_42A70(level, 0, 0, 3, 1, 0);
    if (r32(info, 0) & 0x100) D2Common_42A70(level, w - 7, 0, 3, 2, 0);
    if (r32(info, 0) & 0x200) D2Common_42A70(level, 0, 1, 2, 1, 0);
    uint32_t eax = r32(info, 0);
    if (eax & 0x400) { D2Common_42A70(level, 0, h - 6, 2, 1, 0); eax = r32(info, 4); }
    if (!(r32(info, 0) & 0x40)) {
        const int placed = e->placeFixed(level);
        if (!placed) return 0;   // retail: error string 0x1E5, exit(-1)
        w32(info, 0, r32(info, 0) | 0x40);
        eax = (uint32_t)placed;
    }
    return eax;
}
