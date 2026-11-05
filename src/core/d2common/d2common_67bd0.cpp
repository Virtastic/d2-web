// 1.13c D2Common+0x67BD0 (#10309) waypoint record: 0x10 bytes from the caller's pool (Waypoints.cpp line
// 0xFC), zeroed, first word 0x102, then the first waypoint's bit set: the module's pair (word index, mask)
// at pWaypointBit (the first entry of the waypoint bit table) ORs mask into word index.
#include <cstring>
#include "d2common_4e3f0.h"

uint32_t D2Common_67BD0_AllocWaypointData(uint32_t nPool, const uint16_t* pWaypointBit, D2FogPoolAlloc pfnAlloc)
{
	const uint32_t nRecord = pfnAlloc(nPool, 0x10, "Waypoints.cpp", 0xFC, 0);
	uint16_t* pWords = reinterpret_cast<uint16_t*>(static_cast<uintptr_t>(nRecord));
	std::memset(pWords, 0, 0x10);
	pWords[0] = 0x102;
	pWords[pWaypointBit[0]] |= pWaypointBit[1];
	return nRecord;
}
