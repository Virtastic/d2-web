// D2Game reach test with a collision line check, in the retail 32-bit layout.
#include <cstdint>

struct D2Game38ED0Env {
    // D2Common #11076 (thunk 0xAEB0), stdcall (room, &from, &to, mask)
    uint32_t (*lineBlocked)(uint32_t room, uint32_t* from, uint32_t* to, uint32_t mask);
};

// 1.13c D2Game+0x38ED0
// EAX = dy, ECX = dx, EDX = room; stack: radius squared, x, y (ret 0xC). No room, or dx*dx + dy*dy above
// the squared radius (signed), gives 0. Otherwise the four locals {x+dx, y+dy, x, y} are laid out in that
// order and #11076 checks the line from {x, y} to {x+dx, y+dy} with mask 4; 1 when it reports no hit.
uint32_t UNIT_ReachableNoCollision_38ED0(int32_t dy, int32_t dx, uint32_t room, int32_t radiusSq, int32_t x, int32_t y,
                                         const D2Game38ED0Env& env)
{
    if (room == 0 || dx * dx + dy * dy > radiusSq) return 0;
    uint32_t loc[4] = { (uint32_t)(x + dx), (uint32_t)(y + dy), (uint32_t)x, (uint32_t)y };
    return env.lineBlocked(room, &loc[2], &loc[0], 4) == 0 ? 1u : 0u;
}
