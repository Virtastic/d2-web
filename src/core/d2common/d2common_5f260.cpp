// 1.13c D2Common+0x5F260 (ordinal 10274): unit to display string. One function per file (pb8 batch).
// Moved out of d2common_batch_31d20.cpp, body unchanged in intent; callees 0x5E4D0 and 0x5D800 are wired to
// their ported C++ bodies (both __fastcall in retail: class/object id in ECX, buffer in EDX). 0x5E480 and
// 0x5E410 are not wired: retail passes them the unit type/id with no buffer and they write module state,
// which the ported bodies do not model, so they stay env seams (null contributes nothing).
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <emscripten.h>

void D2Common_5E4D0_GetPlayerName(int32_t nClassId, char* szOut);
void D2Common_5D800_GetObjectName(int32_t nObjectId, char* szOut);

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// Retail stdcall, two arguments, returns the buffer. The buffer's first byte is cleared. A null unit becomes
// "NULL Unit"; unit types 0..4 dispatch as above, type 3 formats "missile %d", any other type writes
// "Invalid Unit". The unit is {type, id} (+0, +4).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_5F260_UnitName(uint32_t pUnit, uint32_t pBuffer,
	void (*pfn5E480)(uint32_t, uint32_t),
	void (*pfn5E410)(uint32_t))
{
	uint8_t* u = P(pUnit);
	uint8_t* buf = P(pBuffer);
	if (buf)
		buf[0] = 0;

	if (!u)
	{
		// Retail calls printf("NULL Unit") (stdout), not a buffer write; the buffer stays "".
		return pBuffer;
	}

	switch (W(u, 0))
	{
	case 0:
		if (buf)
			D2Common_5E4D0_GetPlayerName(static_cast<int32_t>(W(u, 4)), reinterpret_cast<char*>(buf));
		break;
	case 1:
		if (pfn5E480)
			pfn5E480(W(u, 0), W(u, 4));
		break;
	case 2:
		if (buf)
			D2Common_5D800_GetObjectName(static_cast<int32_t>(W(u, 4)), reinterpret_cast<char*>(buf));
		break;
	case 3:
		if (buf)
			std::sprintf(reinterpret_cast<char*>(buf), "missile %d", static_cast<int>(W(u, 4)));
		break;
	case 4:
		if (pfn5E410)
			pfn5E410(W(u, 4));
		break;
	default:
		// printf("Invalid Unit") to stdout in retail; the buffer stays "".
		break;
	}
	return pBuffer;
}
