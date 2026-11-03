// D2Common 0x522C0: find, create or release the stat list of a unit by id (callee seam: the stat-list helpers).
#pragma once
#include <cstdint>
struct D2Common522C0Env {
	uint32_t (*find)(uint32_t pStatList, uint32_t nId);                                              // 0x37F80: ECX = list, EDX = id
	uint32_t (*alloc)(uint32_t pPool, uint32_t nA, uint32_t nB, uint32_t nUnitType, uint32_t nUnitId); // 0x38100 (ret 0x14)
	void (*attach)(uint32_t pUnit, uint32_t pList, uint32_t nFlag);                                  // 0x3A7D0 (ret 0xC)
	void (*detach)(uint32_t pList);                                                                  // 0x3A3B0 (ret 4)
	void (*free)(uint32_t pList);                                                                    // 0x3A9F0: ECX = list
};
// EDI = pUnit, EBX = nId, stack argument = bCreate.
extern "C" uint32_t D2Common_522C0_StatListById(uint32_t pUnit, uint32_t nId, int32_t bCreate, const D2Common522C0Env* env);
