// A unit's insertion into a room's unit list on retail's 32-bit layout (the vendored typed form is the room
// unit-list add in d2moo/src/Units/UnitRoom.cpp). stdcall (unit, room, one unused word), ret 0xC.
// A null unit, a null room, or a failed room check (the check takes ESI = room, EDI = unit and one word, 1) is fatal.
// Otherwise the unit links in at the head (unit+0xE8 = the old head at room+0x74), the unit is marked, and a player
// (type 0) or a monster (type 1) whose class test answers 2 bumps the room's count at +0x28. Always answers 1.
#include <cstdint>
#include <cstring>

struct D2C66800Env {
    uint32_t (*check)(uint8_t* room, uint8_t* unit, uint32_t one);
    void (*mark)(uint8_t* unit);
    uint32_t (*monClass)(uint8_t* unit);
    void (*fatal)(uint32_t code);
    uint32_t (*tag)(const uint8_t* p);   // the pointer as stored in the room's list head
};

uint32_t D2Common_AddUnitToRoom32(uint8_t* unit, uint8_t* room, const D2C66800Env& e)
{
    auto rd = [](const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; };
    auto wr = [](uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); };
    if (!unit) { e.fatal(0x42); return 0; }
    if (!room) { e.fatal(0x43); return 0; }
    if (!e.check(room, unit, 1)) { e.fatal(0x4C); return 0; }
    wr(unit + 0xE8, rd(room + 0x74));
    wr(room + 0x74, e.tag(unit));
    e.mark(unit);
    const uint32_t type = rd(unit);
    if (type == 0 || (type == 1 && e.monClass(unit) == 2)) wr(room + 0x28, rd(room + 0x28) + 1);
    return 1;
}
