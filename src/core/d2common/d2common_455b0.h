// D2Common 0x455B0: word +0x1D0 of the item-data record behind the unit a lookup resolves (callee seam: the 0x1A0D0 unit lookup).
#pragma once
#include <cstdint>
struct D2Common455B0Env {
	uint32_t (*lookup)(uint32_t nEcx, uint32_t nEdx, uint32_t pSlot, uint32_t pArg); // 0x1A0D0: ECX = argument, EDX through, stack (&slot, &argument), ret 8
};
// nEcx, nEdx = register inputs (ECX only seeds a stack slot), nArg = the stack argument (ret 4).
extern "C" uint32_t D2Common_455B0_UnitWord1D0(uint32_t nEcx, uint32_t nEdx, uint32_t nArg, const D2Common455B0Env* env);
