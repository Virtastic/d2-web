// 1.13c D2Game+0x75900 -- hand a unit to 0x75680 after checking it can be placed.
// EDI = a record whose +4 is the unit; stack = param_1 (ret 4). Fatal Fog asserts (never return): unit null 0x1ED, class 0x158 0x1F8,
// no room 0x1FF, class record missing 0x204. Unit flag bit 0x11 of +0xC4 set returns at once. Otherwise y = 0x1130(ECX = unit),
// x = 0x1100(ECX = unit), room = 0xA478(unit), 0xAFE8(room, x, y, 0x801) with a nonzero low word returns; room 0xA478(unit) == 0 is fatal,
// 0x1220(EAX = unit+4) == 0 is fatal, else 0x75680(param_1, unit, 0, EDI) (ret 0x10). No 1.10f counterpart.
#include <cstdint>
#include <cstring>

struct D2Game75900Env {
    uint32_t (*unitY)(uint32_t unit);                                          // 0x1130 (ECX = unit)
    uint32_t (*unitX)(uint32_t unit);                                          // 0x1100 (ECX = unit)
    uint32_t (*unitRoom)(uint32_t unit);                                       // 0xA478 (stdcall 1)
    uint32_t (*roomTest)(uint32_t room, uint32_t x, uint32_t y, uint32_t m);   // 0xAFE8 (stdcall 4; the low word is tested)
    uint32_t (*classRec)(uint32_t classId);                                    // 0x1220 (EAX = class id)
    void (*place)(uint32_t p1, uint32_t unit, uint32_t zero, uint32_t edi);    // 0x75680 (stdcall 4)
    void (*fatal)(uint32_t code);
};

static inline uint32_t r32(uint32_t a, uint32_t o)
{
    uint32_t v;
    std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a) + o), 4);
    return v;
}

// 1.13c D2Game+0x75900
void D2Game_75900(uint32_t edi, uint32_t param1, const D2Game75900Env& e)
{
    const uint32_t unit = r32(edi, 4);
    if (unit == 0) { e.fatal(0x1ED); return; }
    if ((r32(unit, 0xC4) >> 0x11) & 1) return;
    if (r32(unit, 4) == 0x158) { e.fatal(0x1F8); return; }
    const uint32_t y = e.unitY(unit);
    const uint32_t x = e.unitX(unit);
    const uint32_t room = e.unitRoom(unit);
    if ((uint16_t)e.roomTest(room, x, y, 0x801) != 0) return;
    if (e.unitRoom(unit) == 0) { e.fatal(0x1FF); return; }
    if (e.classRec(r32(unit, 4)) == 0) { e.fatal(0x204); return; }
    e.place(param1, unit, 0, edi);
}
