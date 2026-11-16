// 1.13c D2Common+0x66410 (#10681) -- stdcall(unit): unlink the unit from its room's unit list (head room+0x1C, next unit+0xE0).
// The room comes from 0x2FE10 inlined here: unit type (+0) 2, 4 or 5 -> the first dword of the static path at +0x2C; any other
// type -> the dynamic path's room (+0x2C, then +0x1C), 0 without a path. A missing room with a non-null +0xE0 is retail-fatal;
// we do nothing. A unit not on the list leaves everything untouched. Addresses are 32-bit.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(uintptr_t(a)), &v, 4); }
}

// 1.13c D2Common+0x66410
extern "C" void D2Common_66410_UnlinkUnitFromRoom(uint32_t unit)
{
    if (!unit) return;
    const uint32_t type = rd32(unit);
    const uint32_t path = rd32(unit + 0x2C);
    uint32_t room;
    if (type == 2 || type == 4 || type == 5) room = rd32(path);
    else room = path ? rd32(path + 0x1C) : 0;
    if (!room) return;
    uint32_t prev = 0, cur = rd32(room + 0x1C);
    while (cur && cur != unit) { prev = cur; cur = rd32(cur + 0xE0); }
    if (!cur) return;
    if (prev) wr32(prev + 0xE0, rd32(unit + 0xE0));
    else wr32(room + 0x1C, rd32(unit + 0xE0));
    wr32(unit + 0xE0, 0);
}
