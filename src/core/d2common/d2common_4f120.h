// D2Common 0x4F120: 1 when the unit check passes, the current record exists and the record check answers 0.
#pragma once
#include <cstdint>
struct D2Common4F120Env {
	uint32_t (*unitCheck)(uint32_t nEax);   // 0x2FE10 stdcall(eax)
	uint32_t (*current)(void);              // 0x1330 (plain ret): current record, 0 = none
	uint32_t (*recordCheck)(uint32_t nEax); // 0x2D320, EAX = the word at record +0x10, then +0x58, then +0x1D0
};
extern "C" uint32_t D2Common_4F120_CheckCurrentRecord(uint32_t nEax, const D2Common4F120Env* env);
