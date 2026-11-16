// 1.13c D2Common+0x663B0 (#10127) -- stdcall(room): empty the room's unit list.
// Head at room+0x1C; each unit's chain-next at +0xE0 is cleared and bit 0x2000 of its flags dword +0xC4 dropped; then the head
// is cleared. A null room is retail-fatal; we return. Addresses are 32-bit.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(uintptr_t(a)), &v, 4); }
}

// 1.13c D2Common+0x663B0
extern "C" void D2Common_663B0_ClearRoomUnitList(uint32_t room)
{
    if (!room) return;
    for (uint32_t u = rd32(room + 0x1C); u;) {
        const uint32_t next = rd32(u + 0xE0);
        wr32(u + 0xE0, 0);
        wr32(u + 0xC4, rd32(u + 0xC4) & 0xFFFFDFFFu);
        u = next;
    }
    wr32(room + 0x1C, 0);
}
