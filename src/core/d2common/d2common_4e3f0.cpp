// 1.13c D2Common+0x4E3F0 (#10856) skill-list head: a 0x18-byte record from the caller's pool (Skills.cpp
// line 0x380), zeroed, with the pool kept in its first dword. The allocation result is not checked.
#include <cstring>
#include "d2common_4e3f0.h"

uint32_t D2Common_4E3F0_AllocSkillList(uint32_t nPool, D2FogPoolAlloc pfnAlloc)
{
	const uint32_t nRecord = pfnAlloc(nPool, 0x18, "Skills.cpp", 0x380, 0);
	uint8_t* p = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(nRecord));
	std::memset(p, 0, 0x18);
	std::memcpy(p, &nPool, 4);
	return nRecord;
}
