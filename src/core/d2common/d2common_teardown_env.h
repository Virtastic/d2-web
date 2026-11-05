// Injectable seams of the D2Common 1.13c table-teardown routines (0x5DB60, 0x5DE00): module globals by RVA and the two
// Fog releases they call. free = Fog 10046 through thunk 0x9228 (fastcall: ECX pool 0, EDX block; stack: DataTbls.cpp, line,
// 0); unlink = Fog 10212 through thunk 0x9246 (stdcall, one word: a linker).
#pragma once
#include <cstdint>

struct D2CommonTeardownEnv {
	uint32_t (*rd)(uint32_t nRva);
	void (*wr)(uint32_t nRva, uint32_t nValue);
	void (*free)(uint32_t pBlock, uint32_t nLine);
	void (*unlink)(uint32_t pLinker);
};

// DATATBLS_UnloadBin as 1.13c inlines it: a compiled bin built in text mode ([+0x99E20] != 0) is released from its
// 4-byte header (line 0x8F6), a bin read from disk from its own address (line 0x8F8). The slot is not cleared.
inline void D2CommonTeardownBin(const D2CommonTeardownEnv* e, uint32_t nSlotRva) {
	const uint32_t p = e->rd(nSlotRva);
	if (!p) return;
	if (e->rd(0x99E20)) e->free(p - 4, 0x8F6);
	else e->free(p, 0x8F8);
}
