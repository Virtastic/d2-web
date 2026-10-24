// 1.13c D2Game+0x11D0 SEED_RollLimitedRandomNumber (retail: seed in ECX, limit in ESI).
// Same LCG as D2Common 0x10B0; reuses d2::rollLimited (limit <= 0 returns 0, seed untouched).
#include <cstdint>
#include "../prng.h"

extern "C" uint32_t D2Game_SEED_RollLimitedRandomNumber(d2::Seed* pSeed, int32_t nLimit) { // 1.13c D2Game+0x11D0
	return d2::rollLimited(*pSeed, nLimit);
}
