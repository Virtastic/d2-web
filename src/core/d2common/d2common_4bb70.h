#pragma once
#include <cstdint>
// 0x1330 (EAX = room; EBX = x, EDI = y also live): the room's collision record, 0 for none.
using D2RoomCollisionFn = uint32_t (*)(uint32_t pRoom, int32_t nX, int32_t nY);
void D2Common_4BB70_ClearSubtileMask(uint32_t pRoom, uint32_t nMask, int32_t nY, int32_t nX, D2RoomCollisionFn pfnCollision);
// 0x68DF0 (EAX = owner, ESI = slot, ECX = b, EDX = a; stack c, d): fills the slot 0x69170 just took.
using D2SlotFillFn = void (*)(uint32_t pOwner, uint32_t pSlot, int32_t nB, int32_t nA, int32_t nC, int32_t nD);
uint32_t D2Common_69170_TakeSlot(uint32_t pHead, uint32_t pOwner, int32_t nA, int32_t nB, int32_t nC, int32_t nD, D2SlotFillFn pfnFill);
