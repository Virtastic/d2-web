#include <test_seam.h>
#include "UNIT/NpcRetail113.h"
#include "SUnitNpcCommon.h"

using namespace npc113;

// Retail-layout (1.13c) NPC "resurrect mercenary" handler: EAX = npc guid, ECX = game, stack = player (ret 4). Only NPC classes 0xFC,
// 0x96, 0xC6, 0x16F and 0x203 hire/revive; any other class gets reply code 9. 96A40 (EAX = player, EBX = 1, ESI = 7, EDI = game)
// finds the player's dead merc; none: the 15-byte packet. Gold is taken through BCC60 (ESI = player); short: reply code 0xC.
// Otherwise the merc is revived (flag 0x10000 cleared, A4A2/A3A0/A472), C0B90 re-links it, 0x9B goes out (8A690) and reply code 5
// carries the merc id. Every callee goes through D2_CALL on the NpcCall stub (it traps in the game; the FN probe installs stand-ins).
// 1.13c D2Game+0xC0DB0
void D2Retail113_C0DB0(uint32_t guid, uint32_t game, uint32_t player)
{
	uint32_t unit = 0;
	if (rd8(player + 0x6c)) unit = Call(0xE03A0, rd32(player + 0x68), game, rd32(player + 0x64));
	uint32_t node = rd32(game + 0x1320 + (guid & 0x7f) * 4);
	while (node && rd32(node + 0xC) != guid) node = rd32(node + 0xE4);
	if (!node || node != unit) { Packet15(player); return; }
	const uint32_t cls = rd32(node + 4);
	if (!(cls == 0xFC || cls == 0x96 || cls == 0xC6 || cls == 0x16F || cls == 0x203)) { Reply2A(player, 9, 0xFFFFFFFFu); return; }
	const uint32_t m = Call(0x96A40, player, 0, 0, 1, 7);
	Edi(0x96A40, game);
	if (!m) { Packet15(player); return; }
	const uint32_t cost = Call(0xAC22, 0, 0, 0, 0, 0, m);
	if (!Call(0xBCC60, 0, 0, 0, 0, player, cost)) { Reply2A(player, 0xC, 0xFFFFFFFFu); return; }
	wr32(m + 0xC4, rd32(m + 0xC4) & 0xFFFEFFFFu);
	Call(0xA4A2, 0, 0, 0, 0, 0, m, 1);
	const uint32_t v = Call(0xA3A0, 0, 0, 0, 0, 0, m, 0);
	Call(0xA472, 0, 0, 0, 0, 0, m, 6, v);
	Call(0xC0B90, m, 0, 0, 0, 0, game, player);
	Call(0x8A690, 0xFFFF, 0, 0, 0, 0, ClientOf(player));
	Reply2A(player, 5, rd32(m + 0xC));
}
