// 1.13c D2Common+0x10A00: over the level's rooms (EAX = level; plain ret), keep the non-fixed one with the minimum dword at +0x38
// that has no link in direction 1 yet and takes a new room there (the new room is linked, picked, then freed again); returns
// it or null. Raw retail 32-bit layout; the scan is d2common_maze::Probe.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
uint32_t d2common_10a00_port(uint32_t pLevel, const D2CommonMazeEnv* e)
{
	return Probe(e, pLevel, 0x38, false, 1);
}
