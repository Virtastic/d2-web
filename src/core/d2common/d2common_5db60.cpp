// 1.13c D2Common+0x5DB60 (no arguments, called from the teardown driver 0x5E870).
// Five plain allocations are released (line numbers 0x5B8, 0x5BE, 0x5C5, 0x5CC, 0x5D4) and their slot and count words
// cleared; then two compiled bins with their linkers (the bin slot is left as it is, the count after the linker is cleared).
#include "d2common_teardown_env.h"

namespace {
void FreeSlot(const D2CommonTeardownEnv* e, uint32_t nSlot, uint32_t nLine) {
	if (const uint32_t p = e->rd(nSlot)) { e->free(p, nLine); e->wr(nSlot, 0); }
}
}

extern "C" void D2Common_5DB60_UnloadTables(const D2CommonTeardownEnv* e) {   // 1.13c D2Common+0x5DB60
	FreeSlot(e, 0xA0A7C, 0x5B8);
	FreeSlot(e, 0xA0A84, 0x5BE);
	e->wr(0xA0A80, 0);
	FreeSlot(e, 0xA0A8C, 0x5C5);
	e->wr(0xA0A88, 0);
	FreeSlot(e, 0x9FF18, 0x5CC);
	e->wr(0x9FF1C, 0); e->wr(0x9FF20, 0);
	FreeSlot(e, 0x9FF24, 0x5D4);
	e->wr(0x9FF28, 0); e->wr(0x9FF2C, 0);
	D2CommonTeardownBin(e, 0xA0A70);
	e->unlink(e->rd(0xA0A74));
	e->wr(0xA0A78, 0);
	D2CommonTeardownBin(e, 0xA0A64);
	e->unlink(e->rd(0xA0A68));
	e->wr(0xA0A6C, 0);
}
