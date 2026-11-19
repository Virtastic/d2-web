#include <test_seam.h>
#include <D2Game/PLAYER/PlrMsgRetail113.h>
#include <unistd.h>
uint32_t D2R_CC750_A4C0(uint32_t) { __builtin_trap(); }
void     D2R_CC750_21420() { __builtin_trap(); }
void     D2R_CC750_21290(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_CC750_DFFD0(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CC750_Tables() { __builtin_trap(); }

// 1.13c D2Game+0xCC750 (ECX = game, EAX = unit id, stack word 0 = player ref). A missing game is the fatal path (_exit). An unknown unit gives 1. The
// player ref's record (Ordinal 10920) names, when the tables report more than 7 rows at +0xBF0 and the record has +0x44, a list at ((+0x44)->+0)+0x54
// of 0x14-linked entries; the first entry with bit 0 clear names a unit id: when that unit is this one the unit's ai commands are freed, its
// buffer copied and the 0xF sound step (DFFD0) run and 0 returned; any other case 1.
uint32_t D2Retail113_CC750(uint32_t game, uint32_t unitId, uint32_t param1)
{
	if (!game) _exit(-1);
	const uint32_t u = d2g_chain(game, 0x1320, unitId);
	if (!u) return 1;
	const uint32_t p = D2_CALL(D2R_CC750_A4C0, param1);
	if (p && 7 < (int32_t)rw32(D2_CALL(D2R_CC750_Tables), 0xbf0) && rw32(p, 0x44)) {
		for (uint32_t node = rw32(rw32(rw32(p, 0x44), 0), 0x54); node; node = rw32(node, 0x14)) {
			if ((*(const uint8_t*)(uintptr_t)node & 1) == 0) {
				const uint32_t other = d2g_chain(game, 0x1320, rw32(node, 4));
				if (other && rw32(other, 0xc) == rw32(node, 4) && other == u) {
					uint8_t buf[0x20];
					D2_CALL(D2R_CC750_21420);
					D2_CALL(D2R_CC750_21290, game, u, (uint32_t)(uintptr_t)buf);
					D2_CALL(D2R_CC750_DFFD0, 0xfu, param1);
					return 0;
				}
				return 1;
			}
		}
	}
	return 1;
}
