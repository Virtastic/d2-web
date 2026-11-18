#include "../compat/test_seam.h"
#include "d2game_d2f00.h"
#include "d2game_retail_rw.h"
uint32_t D2R_D2F00_FirstItem(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_TxtRow(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_Quality(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_FileIndex(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_UniqueRec(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_Mask8() { __builtin_trap(); }
uint32_t D2R_D2F00_PlayerData(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_QuestRec(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_QuestState(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_FirstCorpse(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_CorpseId(uint32_t) { __builtin_trap(); }
uint32_t D2R_D2F00_NextCorpse(uint32_t) { __builtin_trap(); }
void     D2R_D2F00_Fatal(uint32_t) { __builtin_trap(); }

// Retail's class-lock table at D2Game+0x102CC0: four (quest, nQuest byte) entries, 8 bytes apart.
static const uint32_t kTbl[8] = { 3, 4, 0x12, 0x11, 0x13, 0x12, 0x1b, 0x19 };

// 1.13c D2Game+0xD2F00 -- EDI = player (+0x60 inventory), stack = game, item (ret 8). 1.13c puts the carry-1 unique test (0xD2070) in front
// of the quest gates and runs it again over the player's own list and over every corpse owner's inventory.
uint32_t D2Retail113_D2F00(uint32_t player, uint32_t game, uint32_t item)
{
	const uint32_t inv = rw32(player, 0x60);
	const uint32_t head = D2_CALL(D2R_D2F00_FirstItem, inv);
	if (inv == 0) return 0;
	const uint32_t row = D2_CALL(D2R_D2F00_TxtRow, item == 0 ? 0xffffffffu : rw32(item, 4));
	if (row == 0) return 0;
	if (item != 0 && rw32(item, 0) == 4 && D2_CALL(D2R_D2F00_Quality, item) == 7) {
		const int32_t idx = (int32_t)D2_CALL(D2R_D2F00_FileIndex, item);
		if (idx >= 0) {
			D2_CALL(D2R_D2F00_FileIndex, item);
			const uint32_t rec = D2_CALL(D2R_D2F00_UniqueRec, (uint32_t)idx);
			if (rec != 0 && (D2_CALL(D2R_D2F00_Mask8) & (rw32(rec, 0x2c) & 0xff)) != 0 && D2_CALL(D2Retail113_D2070, head, row, item) != 0) return 0;
		}
	}
	const uint8_t grp = (uint8_t)rw32(row, 0x12a);
	if (grp == 0) return 1;
	const uint32_t code = rw32(row, 0x80);
	auto q = [&](uint32_t quest, uint32_t flag) { return D2_CALL(D2R_D2F00_QuestState, D2_CALL(D2R_D2F00_QuestRec, player, game), quest, flag); };
	if (code == 0x20737361) {                         // "ass "
		const uint32_t pd = D2_CALL(D2R_D2F00_PlayerData, player);
		if (D2_CALL(D2R_D2F00_QuestState, rw32(pd, 0x10 + (rw32(game, 0x6c) >> 8 & 0xff) * 4), 9, 5) == 0) return 0;
	} else if (code == 0x2034336a) {                  // "j43 "
		if (q(0x14, 0) != 0) return 0;
	} else if (code == 0x207a7978) {                  // "xyz "
		if (q(0x14, 5) == 0) return 0;
	} else if (code == 0x20333367) {                  // "g33 "
		if (q(0x13, 7) != 0) return 0;
		if (q(0x13, 8) != 0) return 0;
	} else if (code == 0x20327274) {                  // "tr2 "
		if (q(0x25, 8) == 0) return 0;
		if (q(0x25, 7) != 0) return 0;
	}
	const int32_t k = (int32_t)grp - 1;
	if (k == 9) {
		if (q(10, 0) != 0 && code != 0x20786f62) return 0;
	} else if (k == 4) {
		if (q(4, 0) != 0 && code != 0x2067656c) return 0;
	} else {
		for (uint32_t u = 0; u < 4; ++u)
			if (grp == (uint8_t)kTbl[2 * u + 1] && q(kTbl[2 * u], 0) != 0) return 0;
	}
	if (D2_CALL(D2Retail113_D2070, head, row, item) != 0) return 0;
	for (uint32_t c = D2_CALL(D2R_D2F00_FirstCorpse, rw32(player, 0x60)); c != 0; c = D2_CALL(D2R_D2F00_NextCorpse, c)) {
		const uint32_t id = D2_CALL(D2R_D2F00_CorpseId, c);
		if (game == 0) D2_CALL(D2R_D2F00_Fatal, 0x2d3);
		for (uint32_t n = rw32(game, 0x1120 + (id & 0x7f) * 4); n != 0; n = rw32(n, 0xe4)) {
			if (rw32(n, 0xc) == id) {
				if (rw32(n, 0x60) != 0) {
					const uint32_t h2 = D2_CALL(D2R_D2F00_FirstItem, rw32(n, 0x60));
					if (h2 != 0 && D2_CALL(D2Retail113_D2070, h2, row, item) != 0) return 0;
				}
				break;
			}
		}
	}
	return 1;
}
