// 1.13c D2Common+0x5DE00 (no arguments, called from the teardown driver 0x5E870).
// One compiled bin and its linker always; then, when [+0xA145C] (text compilation) is set, fifteen bin/linker pairs in the
// order below. Each bin goes through the inlined UnloadBin; every linker is released whether or not its bin was set.
// Nothing is cleared.
#include "d2common_teardown_env.h"

extern "C" void D2Common_5DE00_UnloadTables(const D2CommonTeardownEnv* e) {   // 1.13c D2Common+0x5DE00
	D2CommonTeardownBin(e, 0x9FF54);
	e->unlink(e->rd(0x9FF58));
	if (!e->rd(0xA145C)) return;
	static const uint32_t kBins[15] = { 0x9FED8, 0x9FEE0, 0x9FEE8, 0x9FEF0, 0x9FEF8, 0xA0888, 0x9FF8C, 0x9FF00,
	                                    0x9FF08, 0x9FF60, 0xA0974, 0x9FF10, 0x9FF30, 0x9FF44, 0x9FF4C };
	for (const uint32_t s : kBins) {   // the linker sits in the dword after its bin
		D2CommonTeardownBin(e, s);
		e->unlink(e->rd(s + 4));
	}
}
