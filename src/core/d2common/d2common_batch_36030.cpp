// D2Common 1.13c batch: dynamic path allocation/initialisation. Retail 32-bit pointers are read as uint32_t words.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"

extern "C" {
__attribute__((weak)) void* Fog_Alloc10045(const char* file, int32_t line, int32_t zero);
__attribute__((weak)) void Fog_Fatal10265_10024(int32_t msg);   // 10265 + 10024 then _exit(-1)
__attribute__((weak)) uint32_t d2_fn_d2common_35100(void* unit);
__attribute__((weak)) void D2Common_359D0(void* unit);
__attribute__((weak)) int32_t D2Common_35360(void* unit, int32_t size);
__attribute__((weak)) void D2Common_34F80(void* path, int32_t type);
__attribute__((weak)) int32_t d2_sub_30F80(const int32_t* unit);
__attribute__((weak)) uint32_t D2Common_66800(void* unit, uint32_t room);

// 1.13c D2Common+0x36030: allocate the unit's dynamic path (unit+0x2C), seed position/room/size, pick the path type
// defaults by unit type (0 player, 1 monster, 3 object-like), register in the room and derive the half extents.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_36030(int32_t y, uint32_t* unit, int32_t flag, int32_t x, uint32_t room)
{
	uint32_t* p = static_cast<uint32_t*>(Fog_Alloc10045("..\\Source\\D2Common\\PATH\\Path.cpp", 0x453, 0));
	if (!p) { Fog_Fatal10265_10024(0x870); return; }
	std::memset(p, 0, 0x200);
	unit[0xb] = static_cast<uint32_t>(reinterpret_cast<intptr_t>(p));
	p[0xc] = static_cast<uint32_t>(reinterpret_cast<intptr_t>(unit));
	uint32_t sx = static_cast<uint32_t>(d2_sub_30F80(reinterpret_cast<int32_t*>(unit)));
	p[0x11] = sx;
	p[0x12] = static_cast<uint32_t>(D2Common_35360(unit, sx));
	p[0] = static_cast<uint32_t>(x) * 0x10000 + 0x8000;
	*reinterpret_cast<int16_t*>(p + 0x76) = static_cast<int16_t>(x);
	p[1] = static_cast<uint32_t>(y) * 0x10000 + 0x8000;
	p[0x1f] = 0x800;
	p[7] = room;
	*(reinterpret_cast<uint8_t*>(p) + 0x93) = 0;
	p[0x75] = 1;
	*reinterpret_cast<int16_t*>(reinterpret_cast<uint8_t*>(p) + 0x1da) = static_cast<int16_t>(y);
	const int32_t type = static_cast<int32_t>(unit[0]);
	if (type == 0) {
		p[0x13] = 0x80;
		p[0x14] = 0x1c09;
		D2Common_34F80(p, 7);
		*(reinterpret_cast<uint8_t*>(p) + 0x91) = 0x49;
		*(reinterpret_cast<uint8_t*>(p) + 0x92) = 0x46;
	} else if (type == 1) {
		p[0x13] = 0x100;
		p[0xd] &= 0xfff800ff;
		p[0xf] = 2;
		p[0x26] = 0;
		if (p[0x10] == 0xb) { Fog_Fatal10265_10024(0x871); return; }
		if (p[0x10] == 8) { Fog_Fatal10265_10024(0x872); return; }
		int32_t cls = static_cast<int32_t>(unit[1]);
		bool special = false;
		if (cls >= 0) {
			if (cls < sgptDataTables->nMonStatsTxtRecordCount && sgptDataTables->pMonStatsTxt)
				cls = *reinterpret_cast<const int16_t*>(reinterpret_cast<const uint8_t*>(&sgptDataTables->pMonStatsTxt[cls]) + 2);
			if (cls >= 0 && cls < sgptDataTables->nMonStatsTxtRecordCount && cls == 0x26) special = true;
		}
		if (special) {
			*reinterpret_cast<uint32_t*>(reinterpret_cast<intptr_t>(p) + 0x48) = 5;
			p[0x14] = 0x804;
		} else {
			p[0x14] = d2_fn_d2common_35100(unit);
		}
		*(reinterpret_cast<uint8_t*>(p) + 0x91) = 0xe;
	} else if (type == 3) {
		p[0x13] = 0;
		p[0x14] = 0;
		D2Common_34F80(p, 4);
	}
	if (room != 0) {
		D2Common_359D0(unit);
		D2Common_66800(unit, p[7]);
	}
	p[2] = static_cast<uint32_t>((static_cast<int32_t>((p[0] >> 0xb) - (p[1] >> 0xb))) >> 1);
	p[3] = static_cast<uint32_t>(static_cast<int32_t>((p[1] >> 0xb) + (p[0] >> 0xb)) >> 2);
	if (flag != 0) p[0xd] |= 0x10;
}
}
