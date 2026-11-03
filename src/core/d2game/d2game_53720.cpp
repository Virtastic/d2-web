// 1.13c D2Game+0x53720: attach a player unit to a game's client record. Register convention: EAX = the record,
// ECX = the unit, then ret 4 (one stack arg, the seed pointer; it reaches only the 0xE0110 seed via ESI, so the port takes no seed). A null record (0x114), a null unit (0x4BB, after clearing record+0x174) or a unit whose type is not
// 0 (0x4BC) is the fatal path (Fog 10265/10024, _exit); otherwise the record takes the unit's type/id/pointer, the unit's player data
// (#10920) points back at the record, #10140 copies the record's name field, 0xE0110 seeds, #11090(unit, 1) runs and the four player-data
// counters at +0x50..+0x5C are cleared. The unit is an opaque 32-bit pointer read by its retail offsets.
#include <cstdint>
#include <cstring>

struct D2Game53720Env {
    uint8_t* (*ordinal10920)(uint8_t* pUnit);               // 0xA4C0 (#10920, stack arg 0)
    void (*ordinal10140)(uint8_t* pUnit, uint8_t* pDest);   // 0xA40C (#10140, stack args 0-1)
    void (*subE0110)();                                     // 0xE0110
    void (*ordinal11090)(uint8_t* pUnit, uint32_t nValue);  // 0xA4A2 (#11090, stack args 0-1)
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// 1.13c D2Game+0x53720
void D2Game_53720(uint8_t* pGame, uint8_t* pUnit, const D2Game53720Env* pEnv)
{
    if (pGame == nullptr) { pEnv->fatal(0x114); return; }
    if (pUnit == nullptr) { wr32(pGame, 0x174, 0); pEnv->fatal(0x4BB); return; }
    wr32(pGame, 0x16C, rd32(pUnit, 0));
    wr32(pGame, 0x170, rd32(pUnit, 0xC));
    wr32(pGame, 0x174, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnit)));
    if (rd32(pUnit, 0) != 0) { pEnv->fatal(0x4BC); return; }
    uint8_t* pData = pEnv->ordinal10920(pUnit);
    wr32(pData, 0x9C, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pGame)));
    pEnv->ordinal10140(pUnit, pGame + 0xD);
    pEnv->subE0110();
    pEnv->ordinal11090(pUnit, 1);
    pData = pEnv->ordinal10920(pUnit);
    wr32(pData, 0x5C, 0); wr32(pData, 0x50, 0); wr32(pData, 0x58, 0); wr32(pData, 0x54, 0);
}
