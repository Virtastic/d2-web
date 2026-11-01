// Orth-list insert of a room: ESI = owning room, EAX = neighbour room, one stack word = direction (callee pops it). A node of the owner's list (head at
// room +0, link at node +0x14, neighbour at node +0) already naming the neighbour ends it with that node in EAX; otherwise a 0x18-byte node is allocated from the
// DRLG's pool (room +0x58 -> level, level +0x1B4 -> drlg, drlg +0x478 -> pool; file DrlgRoom.cpp line 0xD4), zeroed, filled (+0 neighbour, +4 direction, +0xC 1,
// +0x10 = neighbour + 0x34) and pushed on the head. D2Common_44950 is the retail layout (32-bit tokens, `resolve` and `alloc` seams); D2Common_44950_Engine is the
// engine form on D2MOO structs.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgDrlgRoom.h"

// 1.13c D2Common+0x44950
extern "C" uint32_t D2Common_44950(uint8_t* pOwner, uint32_t room, int32_t direction, uint8_t* (*resolve)(uint32_t),
                                   uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line))
{
    auto rd = [](const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; };
    auto wr = [](uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); };
    for (uint32_t node = rd(pOwner, 0); node; node = rd(resolve(node), 0x14))
        if (rd(resolve(node), 0) == room) return node;
    const uint32_t pool = rd(resolve(rd(resolve(rd(pOwner, 0x58)), 0x1B4)), 0x478);
    const uint32_t node = alloc(pool, 0x18, 0xD4);
    uint8_t* n = resolve(node);
    std::memset(n, 0, 0x18);
    wr(n, 0x14, rd(pOwner, 0));
    wr(pOwner, 0, node);
    wr(n, 0, room);
    wr(n, 4, static_cast<uint32_t>(direction));
    wr(n, 0xC, 1);
    wr(n, 0x10, room + 0x34);
    return node;
}

// 1.13c D2Common+0x44950
extern "C" void D2Common_44950_Engine(D2DrlgRoomStrc* owner, D2DrlgRoomStrc* room, int32_t direction)
{
    // ESI is the owning room, EAX the neighbour, stack+4 the direction.
    auto** head = &owner->pDrlgOrth;
    for (auto* node = *head; node; node = node->pNext) if (node->pDrlgRoom == room) return;
    auto* node = D2_CALLOC_STRC_POOL(owner->pLevel->pDrlg->pMempool, D2DrlgOrthStrc);
    node->pDrlgRoom = room; std::memcpy(&node->nDirection, &direction, sizeof(direction));
    node->bInit = 1; node->pBox = &room->pDrlgCoord;
    node->pNext = *head; *head = node;
}

