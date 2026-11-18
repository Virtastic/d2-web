#include "QUESTS/ACT5/A5Q5.h"
#include "GAME/Game.h"
#include "UNIT/SUnit.h"

// 1.13c D2Game+0x5D080
// Walks the player units (type 0) with the portal-closing callback (D2Game+0x5CFF0) and a null argument. The 1.10f operate functions
// spell the walk out at each call site; 1.13c has it as this one forwarder (the game rides in EDI, the iterate call ends in ret 4 for
// the pushed null). The two A5Q5 operate sites now call it.
void __fastcall ACT5Q5_CloseAllPortals(D2GameStrc* pGame)
{
	SUNIT_IterateUnitsOfType(pGame, 0, 0, ACT5Q5_UnitIterate_ClosePortals);
}
