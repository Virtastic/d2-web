// 1.13c D2Common+0x10BE0: roll the level LCG for a start direction and apply three preset records (0x106B0) from the tables at
// 0x95F98 / 0x95FD8 / 0x96018, each indexed by the counter the previous call stepped; level ids 0x73 add the 0x96058 record and
// 0x71 / 0x73 / 0x76 the 0x96098 one (ESI = level; plain ret). Raw retail 32-bit layout; maze env.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

// the five 4 x {key, a, b, dir} tables, contiguous from rva 0x95F98 (exported so the probe can name a record by its retail rva)
extern "C" const int32_t d2common_10be0_records[5][4][4] = {
	{ { 0x3F2, 0x3FD, -1, 3 }, { 0x3EC, 0x3FB, -1, 0 }, { 0x3EE, 0x3FC, -1, 1 }, { 0x3EB, 0x3FA, -1, 2 } },
	{ { 0x3F2, 0x401, -1, 3 }, { 0x3EC, 0x3FF, -1, 0 }, { 0x3EE, 0x400, -1, 1 }, { 0x3EB, 0x3FE, -1, 2 } },
	{ { 0x3F2, 0x405, -1, 3 }, { 0x3EC, 0x403, -1, 0 }, { 0x3EE, 0x404, -1, 1 }, { 0x3EB, 0x402, -1, 2 } },
	{ { 0x3F2, 0x409, -1, 3 }, { 0x3EC, 0x407, -1, 0 }, { 0x3EE, 0x408, -1, 1 }, { 0x3EB, 0x406, -1, 2 } },
	{ { 0x3F2, 0x40A, -1, 3 }, { 0x3EC, 0x40C, -1, 0 }, { 0x3EE, 0x40B, -1, 1 }, { 0x3EB, 0x40D, -1, 2 } } };

extern "C"
void d2common_10be0_port(uint32_t pLevel, const D2CommonMazeEnv* e)
{
	int32_t k = static_cast<int32_t>(Lcg(pLevel + 0x1C4) & 3);
	for (int t = 0; t < 3; ++t)
		e->f106b0(d2common_10be0_records[t][k], pLevel, &k);
	const uint32_t nId = D(pLevel + 0x1D0);
	if (nId == 0x73)
		e->f106b0(d2common_10be0_records[3][k], pLevel, &k);
	if (nId == 0x71 || nId == 0x73 || nId == 0x76)
		e->f106b0(d2common_10be0_records[4][k], pLevel, &k);
}
