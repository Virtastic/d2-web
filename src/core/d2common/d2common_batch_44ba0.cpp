// Stage A. Prior art used: D2MOO DrlgDrlgRoom.cpp,
// DRLGROOM_AllocDrlgOrthsForRooms. The cached 1.13c decompile omits the
// register operands; the retail instructions establish ECX=first room,
// EBX=second room, EAX=direction. The existing 44950 implementation owns
// allocation, deduplication and insertion; both calls mutate that real state.
#include <cstdint>

struct D2DrlgRoomStrc;
extern "C" void d2_fn_d2common_44950(D2DrlgRoomStrc* owner,
    D2DrlgRoomStrc* neighbour, int32_t direction);

// 1.13c D2Common+0x44BA0
extern "C" void d2_fn_d2common_44ba0(D2DrlgRoomStrc* first,
    D2DrlgRoomStrc* second, int32_t direction)
{
    d2_fn_d2common_44950(first, second, direction);
    d2_fn_d2common_44950(second, first,
        static_cast<int32_t>((static_cast<uint32_t>(direction) - 2u) & 3u));
}
