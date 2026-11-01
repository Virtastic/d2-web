// 1.13c D2Common+0x1DAD0 (retail-layout, 32-bit image): visit every cell of a unit's path with a callback.
// stdcall(unit, mask, context, callback), ret 0x10. Path (unit +0x2C): flags +0x34 keep their low byte and top 13 bits, +0x3C := 0x10,
// +0x98 := 0, step count +0x28, mode +0x40 (8 and 11 are fatal in retail), cell list at +0x9C (x, y as 16-bit pairs). Then 0x36230 refreshes
// the path, the unit's room comes from 0x2FE10, and for each cell where 0x4CE00 (room, x, y, context, mask) answers non-zero in its low 16
// bits and 0x7F6A0 finds an object, the callback runs with ECX = unit, EDX = the object.
#include <cstdint>
struct D2Common1DAD0Env {
    void (*step)(uint32_t path, uint32_t unit);                               // 0x36230 (path, unit, 0)
    uint32_t (*roomOf)(uint32_t unit);                                        // 0x2FE10
    uint32_t (*collides)(uint32_t room, uint32_t x, uint32_t y, uint32_t context, uint32_t mask);   // 0x4CE00, low 16 bits tested
    uint32_t (*find)(uint32_t room, uint32_t x, uint32_t y, uint32_t context);                       // 0x7F6A0 (with the 0x6FD6D700 filter)
    void (*callback)(uint32_t unit, uint32_t obj);
};
extern "C" void D2Common_1DAD0_VisitPathCellsRaw(const D2Common1DAD0Env* e, uint32_t unit, uint32_t mask, uint32_t context)
{
    auto P = [](uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); };
    auto W = [&](uint32_t a) -> uint32_t& { return *reinterpret_cast<uint32_t*>(P(a)); };
    const uint32_t path = W(unit + 0x2C);
    W(path + 0x34) &= 0xFFF800FFu;
    W(path + 0x3C) = 0x10;
    W(path + 0x98) = 0;
    if (W(path + 0x40) == 8 || W(path + 0x40) == 11) return;   // retail logs and exits the process
    e->step(path, unit);
    const int32_t count = static_cast<int32_t>(W(path + 0x28));
    const uint32_t room = e->roomOf(unit);
    for (int32_t i = 0; i < count; ++i) {
        const uint32_t x = *reinterpret_cast<uint16_t*>(P(path + 0x9C + 4 * i)), y = *reinterpret_cast<uint16_t*>(P(path + 0x9E + 4 * i));
        if (!(e->collides(room, x, y, context, mask) & 0xFFFF)) continue;
        if (const uint32_t obj = e->find(room, x, y, context)) e->callback(unit, obj);
    }
}
