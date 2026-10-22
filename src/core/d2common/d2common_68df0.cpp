// D2Common 1.13c implementation: tile-slot initialiser. Register inputs (eax, esi) are explicit; the
// D2CMP #10051 import (via thunk 0x92FA) is a seam.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x68DF0: fill the 0x30-byte slot `s` (esi) for tile coords (p1=y-ish, p2=x-ish), flag word
// p3 and tile handle p4; `room` (eax) may be 0, else supplies origin offsets at +0x34/+0x38.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_68df0(int32_t p1, int32_t p2, uint32_t p3, int32_t p4, uintptr_t room,
	uintptr_t s, uint32_t (*w92fa)(int32_t)) {
	int32_t* o = reinterpret_cast<int32_t*>(s);
	uint8_t* b = reinterpret_cast<uint8_t*>(s);
	o[6] = p4; o[7] = 0; o[5] = 0; o[9] = 0;
	b[0x29] = 0xff; b[0x2a] = 0xff; b[0x28] = 0xff;
	if (room) {
		const int32_t* r = reinterpret_cast<const int32_t*>(room);
		o[2] = p2 - r[0x34 / 4];
		o[0] = (p2 - (p1 + 1)) * 0x50;
		o[3] = p1 - r[0x38 / 4];
		o[1] = (((p1 + 1 + p2) * 0x50) >> 1) + 0x28;
	}
	uint32_t f = (((p3 >> 0x12) & 3) + 1) * 0x4000;
	if ((int8_t)p3 < 0) f |= 1;
	if (p3 & 0x10000000) f |= 0x102;
	if (p3 & 0x20000) f |= 0x40;
	if (p3 & 0x10000) f |= 0x80;
	if (p3 & 8) f |= 4;
	if ((int32_t)p3 < 0) f |= 8; else f &= 0xfffffff7u;
	if (p3 & 0x4000000) f |= 0x20c;
	if (p3 & 0x20000000) f |= 0x800;
	if (p3 & 4) f |= 0x2000;
	uint32_t u = w92fa(p4);
	if (u & 1) f |= 4;
	if (u & 4) f |= 0x800;
	o[5] = (int32_t)f;
}
}
