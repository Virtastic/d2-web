// 1.13c D2Common+0x10820: over the level's rooms (ECX = level; plain ret), keep the non-fixed one with the maximum dword at +0x34
// that has no link in direction 2 yet and takes a new room there (the new room is linked, picked, then freed again); returns
// it or null. Raw retail 32-bit layout; the scan is d2common_maze::Probe.
#include <cstdint>
#include "d2common_maze_env.h"
using namespace d2common_maze;

extern "C"
uint32_t d2common_10820_port(uint32_t pLevel, const D2CommonMazeEnv* e)
{
	return Probe(e, pLevel, 0x34, true, 2);
}
