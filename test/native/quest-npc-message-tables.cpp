// Compile with the normal d2moo include paths, -ffunction-sections and -Wl,--gc-sections.
// The quest NPC message tables decide which NPC offers which quest line (QUESTS_InitScrollTextChain looks the
// speaking NPC up in the table for the quest's current index). These are the entries that differed from 1.13c.
#include "../../src/core/d2moo/src/D2Game/QUESTS/ACT1/A1Q3.cpp"
#include "../../src/core/d2moo/src/D2Game/QUESTS/ACT2/A2Q1.cpp"
#include "../../src/core/d2moo/src/D2Game/QUESTS/ACT3/A3Q3.cpp"
#include "../../src/core/d2moo/src/D2Game/QUESTS/ACT5/A5Q4.cpp"
#include "../../src/core/d2moo/src/D2Game/QUESTS/ACT5/A5Q5.cpp"
#include "../../src/core/d2moo/src/D2Game/QUESTS/ACT5/A5Q6.cpp"
#include <cstdio>

static int failures;

static void expect(const char* name, const D2NPCMessageTableStrc* tables, int table, int entry, int npc, int str, int menu)
{
	const auto& e = tables[table].pMessages[entry];
	if (e.nNPCNo != npc || e.nStringIndex != str || e.nMenu != menu)
	{
		std::printf("%s table %d entry %d: got {%d, %d, menu %d}, want {%d, %d, menu %d}\n", name, table, entry,
			(int)e.nNPCNo, (int)e.nStringIndex, (int)e.nMenu, npc, str, menu);
		++failures;
	}
}

int main()
{
	// Tools of the Trade: Charsi's line in the "quest started" table carries menu 2 like every other NPC there.
	expect("A1Q3", gpAct1Q3NpcMessages, 1, 3, MONSTER_CHARSI, 150, 2);
	// Radament: the first speaker in table 4 is Warriv.
	expect("A2Q1", gpAct2Q1NpcMessages, 4, 0, MONSTER_WARRIV2, 331, 2);
	// Blade of the Old Religion: the intro line is Hratli's.
	expect("A3Q3", gpAct3Q3NpcMessages, 0, 0, MONSTER_HRATLI, 571, 1);
	// Act 5: Anya (in town) speaks these lines; Nihlathak is gone from town by then.
	expect("A5Q4", gpAct5Q4NpcMessages, 2, 2, MONSTER_DREHYA, 20143, 2);
	expect("A5Q4", gpAct5Q4NpcMessages, 3, 2, MONSTER_DREHYA, 20148, 0);
	expect("A5Q4", gpAct5Q4NpcMessages, 4, 2, MONSTER_DREHYA, 20148, 2);
	expect("A5Q5", gpAct5Q5NpcMessages, 1, 4, MONSTER_DREHYA, 20156, 2);
	expect("A5Q5", gpAct5Q5NpcMessages, 2, 2, MONSTER_DREHYA, 20161, 2);
	expect("A5Q5", gpAct5Q5NpcMessages, 3, 2, MONSTER_DREHYA, 20166, 2);
	expect("A5Q6", gpAct5Q6NpcMessages, 0, 4, MONSTER_DREHYA, 20173, 2);
	expect("A5Q6", gpAct5Q6NpcMessages, 3, 4, MONSTER_DREHYA, 20176, 2);
	return failures ? 1 : 0;
}
