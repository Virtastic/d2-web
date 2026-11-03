// 1.13c D2Game+0x6A220: object-mode switch by unit id. Register convention: EAX = game, ECX = unit id, no stack args, no result.
// The game's id table (game+0x1520, 128 chain heads, index id & 0x7F, next link unit+0xE4) yields the unit whose id (unit+0xC) matches.
// A null game (Fog 10265 line 0x2D3) or an id with no unit (line 0xD37) is the fatal path. Then, whatever the mode: room = #10331(unit),
// {x,y} = #10141(unit), a = #10485(unit), b = #10575(unit); the mode (unit+0x10) 0 calls the 0xAE38 helper(room,x,y,a,b,0x8180) and stops;
// 2 and 5 call it too and a zero low word moves the unit to mode 2 (unless already 2), nonzero to mode 5 (unless already 5) through #11090.
#include <cstdint>
#include <cstring>

struct D2Game6A220Env {
    uint8_t* (*unitRoom)(uint8_t* pUnit);                       // 0xA478
    void (*unitCoords)(uint8_t* pUnit, uint32_t* pXY);          // 0xA544 (writes x, y)
    uint32_t (*unitA)(uint8_t* pUnit);                          // 0xA71E
    uint32_t (*unitB)(uint8_t* pUnit);                          // 0xB13E
    uint16_t (*helper)(uint8_t* pRoom, uint32_t x, uint32_t y, uint32_t a, uint32_t b, uint32_t nFlags);   // 0xAE38 (6 stack words)
    void (*setMode)(uint8_t* pUnit, uint32_t nMode);            // 0xA4A2
    void (*fatal)(uint32_t nLine);
};

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

// 1.13c D2Game+0x6A220
void D2Game_6A220(uint8_t* pGame, uint32_t nId, const D2Game6A220Env* pEnv)
{
    if (pGame == nullptr) { pEnv->fatal(0x2D3); return; }
    uint8_t* pUnit = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rd32(pGame, 0x1520 + (nId & 0x7F) * 4)));
    while (pUnit != nullptr && rd32(pUnit, 0xC) != nId)
        pUnit = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rd32(pUnit, 0xE4)));
    if (pUnit == nullptr) { pEnv->fatal(0xD37); return; }
    uint8_t* pRoom = pEnv->unitRoom(pUnit);
    uint32_t xy[2];
    pEnv->unitCoords(pUnit, xy);
    const uint32_t a = pEnv->unitA(pUnit);
    const uint32_t b = pEnv->unitB(pUnit);
    const uint32_t nMode = rd32(pUnit, 0x10);
    if (nMode == 0) { pEnv->helper(pRoom, xy[0], xy[1], a, b, 0x8180); return; }
    if (nMode != 2 && nMode != 5) return;
    const uint16_t r = pEnv->helper(pRoom, xy[0], xy[1], a, b, 0x8180);
    if (r == 0) { if (nMode != 2) pEnv->setMode(pUnit, 2); }
    else if (nMode != 5) pEnv->setMode(pUnit, 5);
}
