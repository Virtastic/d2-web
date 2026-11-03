// D2Common 0x4EBF0: a slot word of the table the 0x1E1B0 lookup returns, for a record tagged 0x1020304.
#pragma once
#include <cstdint>
struct D2Common4EBF0Env {
	uint32_t (*table)(uint32_t pGlobal); // 0x1E1B0: EAX = 0, stack = module offset 0x9D378 (ret 4); 0 = none
};
// nMode = EAX (5 selects word +0x10, anything else +0x14), pRec = ECX.
extern "C" uint32_t D2Common_4EBF0_SlotWord(uint32_t nMode, uint32_t pRec, const D2Common4EBF0Env* env);
