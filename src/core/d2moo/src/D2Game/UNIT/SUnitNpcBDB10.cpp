#include <test_seam.h>
#include "UNIT/NpcRetail113.h"
#include "SUnitNpcCommon.h"

using namespace npc113;

// Retail-layout (1.13c) Cain identify-all handler: ECX = player, EAX = npc guid, stack = game (ret 4). Classes 0xF4-0xF6, 0x109 and
// 0x208 only; no unidentified items: reply code 9; the price (count * 100) is taken from gold or stash unless the quest check
// passes, short: reply code 0xC; then every item of the inventory list that the class test marks is identified through D6BC0 and one
// code-3 reply follows. Every callee goes through D2_CALL on the NpcCall stub.
// 1.13c D2Game+0xBDB10
uint32_t D2Retail113_BDB10(uint32_t player, uint32_t guid, uint32_t game)
{
	uint32_t unit = 0;
	if (rd8(player + 0x6c)) unit = Call(0xE03A0, rd32(player + 0x68), game, rd32(player + 0x64));
	uint32_t node = rd32(game + 0x1320 + (guid & 0x7f) * 4);
	while (node && rd32(node + 0xC) != guid) node = rd32(node + 0xE4);
	if (!node || node != unit) { Packet15(player); return 0; }
	const uint32_t cls = rd32(node + 4);
	if (!(cls == 0xF4 || cls == 0xF5 || cls == 0xF6 || cls == 0x109 || cls == 0x208)) return 0;
	const uint32_t unid = Call(0xABC2, 0, 0, 0, 0, 0, player);
	if (unid == 0) { Reply2A(player, 9, 0xFFFFFFFFu); return 0; }
	const uint32_t pg = Call(0xA4C0, 0, 0, 0, 0, 0, player);
	if (Call(0xA568, 0, 0, 0, 0, 0, rd32(pg + 0x10 + rd8(game + 0x6d) * 4), 4, 0) != 1) {
		const uint32_t x = Call(0x1020, player, 0, 0, 0, 0, game);
		if (Call(0xA568, 0, 0, 0, 0, 0, x, 4, 1) != 1 && Call(0xBCC60, 0, 0, 0, 0, player, unid * 100) == 0) { Reply2A(player, 0xC, 0xFFFFFFFFu); return 0; }
	}
	uint32_t cur = Call(0xA3DC, 0, 0, 0, 0, 0, rd32(player + 0x60));
	while (cur) {
		const uint32_t next = Call(0xA4CC, 0, 0, 0, 0, 0, cur);
		const uint32_t ty = Call(0xA736, 0, 0, 0, 0, 0, cur);
		const uint32_t it = Call(0xA46C, 0, 0, 0, 0, 0, cur);
		bool ident = false;
		if (ty == 1) {
			if ((Call(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF) == 0) ident = true;
			else if ((Call(0xA6C4, 0, 0, 0, 0, 0, it) & 0xFF) == 3) ident = true;
		} else if (ty == 3) ident = true;
		if (ident && Call(0xA8A4, 0, 0, 0, 0, 0, it, 0x10, 0xA9C, 0xB20FB3A4u) == 0) {
			Call(0xD6BC0, player, 0, 0, 0, 0, game);
			Edi(0xD6BC0, it);
		}
		cur = next;
	}
	Reply2A(player, 3, 0xFFFFFFFFu);
	return 0;
}
