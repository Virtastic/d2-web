// 1.13c D2Common+0x12250: 0 when the box of half-size margin around (px, py) sits strictly inside the room's first four box words (x, y, w, h at +0x4C), else 1.
// ECX = room, EDX = px, EAX = py, ESI = margin; the 0x3BA60 copy is inline (a null room reads as all zero).
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
uint32_t d2common_12250_port(uint32_t pRoom, int32_t px, int32_t py, int32_t nMargin) // 1.13c D2Common+0x12250
{
	int32_t x = 0, y = 0, w = 0, h = 0;
	if (pRoom)
	{
		x = static_cast<int32_t>(D(pRoom + 0x4C));
		y = static_cast<int32_t>(D(pRoom + 0x50));
		w = static_cast<int32_t>(D(pRoom + 0x54));
		h = static_cast<int32_t>(D(pRoom + 0x58));
	}
	if (px - nMargin > x && py - nMargin > y && px + nMargin < w + x && py + nMargin < h + y)
		return 0;
	return 1;
}
