// Mutual orth link of two rooms: EAX = direction, ECX = first room, EBX = second room. The first room gets the second as a neighbour in `direction`, the second gets the
// first in (direction - 2) & 3, both through 0x44950 (ESI = owner, EAX = neighbour, stack = direction); EAX is the second call's. D2Common_44BA0 is the retail layout
// (32-bit tokens); D2Common_44BA0_Engine is the engine form on D2MOO structs.
#include <cstdint>

struct D2DrlgRoomStrc;
extern "C" uint32_t D2Common_44950(uint8_t*, uint32_t, int32_t, uint8_t* (*)(uint32_t), uint32_t (*)(uint32_t, uint32_t, uint32_t));

// 1.13c D2Common+0x44BA0
extern "C" uint32_t D2Common_44BA0(int32_t direction, uint32_t first, uint32_t second, uint8_t* (*resolve)(uint32_t),
                                   uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line))
{
    D2Common_44950(resolve(first), second, direction, resolve, alloc);
    return D2Common_44950(resolve(second), first, static_cast<int32_t>((static_cast<uint32_t>(direction) - 2u) & 3u), resolve, alloc);
}

extern "C" void D2Common_44950_Engine(D2DrlgRoomStrc* owner,
    D2DrlgRoomStrc* neighbour, int32_t direction);

// engine form of 0x44BA0
extern "C" void D2Common_44BA0_Engine(D2DrlgRoomStrc* first,
    D2DrlgRoomStrc* second, int32_t direction)
{
    D2Common_44950_Engine(first, second, direction);
    D2Common_44950_Engine(second, first,
        static_cast<int32_t>((static_cast<uint32_t>(direction) - 2u) & 3u));
}
