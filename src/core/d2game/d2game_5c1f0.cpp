// Tyrael's spawn beside a unit on retail's 32-bit layout (the vendored typed form is ACT5Q6_SpawnTyrael in
// d2moo/src/D2Game/QUESTS/ACT5/A5Q6.cpp). ECX = the unit, stack: game, room (ret 8).
// The unit's coordinates minus 5 on each axis seed a free-position search (radius 5, flags 0x400,
// 0x64 tries) from the room argument; when it finds a room, a 10-dword spawn request (game, room, 0, monster 0x209, 1, 0, x, y, 4, 0x42) is
// handed to the spawner in ECX.
#include <cstdint>

struct D2G5C1F0Env {
    void (*getCoords)(uint32_t unit, uint32_t* coord);
    void (*freePos)(uint32_t room, uint32_t* coord, uint32_t r, uint32_t flags, uint32_t* outRoom, uint32_t n);
    void (*spawn)(uint32_t* req);
};

void ACT5Q6_SpawnTyrael32(uint32_t unit, uint32_t game, uint32_t room, const D2G5C1F0Env& env)
{
    uint32_t outRoom = 0, coord[2];
    env.getCoords(unit, coord);
    coord[0] -= 5; coord[1] -= 5;
    env.freePos(room, coord, 5, 0x400, &outRoom, 0x64);
    if (!outRoom) return;
    uint32_t req[10] = { game, outRoom, 0, 0x209, 1, 0, coord[0], coord[1], 4, 0x42 };
    env.spawn(req);
}
