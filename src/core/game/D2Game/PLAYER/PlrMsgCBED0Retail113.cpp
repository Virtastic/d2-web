#include <test_seam.h>
#include <D2Game/PLAYER/PlrMsgCBED0Retail113.h>
#include "../../../d2game/d2game_retail_rw.h"
uint32_t D2R_CBED0_A4C0(uint32_t) { __builtin_trap(); }
void     D2R_CBED0_8B400(uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xCBED0 (EAX = unit): builds a 0x28-byte packet {0x5a, 0x0e, 1}; a player unit (type 0) passes the client record's +0x9C
// (client = 0xA4C0(unit)) as the one stack word, anything else 0. Sent through 0x8B400 (EAX = packet).
void D2Retail113_CBED0(uint32_t unit)
{
	uint8_t pkt[0x28] = {};
	pkt[0] = 0x5a; pkt[1] = 0x0e; pkt[2] = 1;
	uint32_t arg = 0;
	if (unit && rw32(unit, 0) == 0) arg = rw32(D2_CALL(D2R_CBED0_A4C0, unit), 0x9c);
	D2_CALL(D2R_CBED0_8B400, (uint32_t)(uintptr_t)pkt, arg);
}
