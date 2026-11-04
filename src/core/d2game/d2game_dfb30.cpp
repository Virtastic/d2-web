// 1.13c D2Game+0xDFB30: fill a unit-info record from a unit (retail-layout seam; the engine's SUNIT_FillUnitInfo is the vendored form).
// ESI = unit, EDI = info, plain ret. Unit fields: type +0, class +4, guid +0xC, mode +0x10, path +0x2C, owner type +0x94, owner guid +0x98.
// Info record: type 0, class 4, guid 8, mode 0xC, x 0x10, y 0x14, owner type 0x18, owner guid 0x1C, name 0x24 (players only, else empty).
// Types 2, 4, 5 (object, item, tile) read x/y straight from the static path (+0xC, +0x10); the others ask D2Common for the dynamic path's
// x and y, or take 0 when the unit has no path. A player's name is copied unbounded from the player-data record D2Common returns.
#include <cstdint>
#include "../d2common/d2common_fog_env.h"

struct D2GameUnitInfoEnv {
	uint32_t (*playerData)(uint32_t pUnit);   // thunk 0xA4C0, D2Common #10920, stdcall(unit)
	uint32_t (*pathX)(uint32_t pPath);        // thunk 0xA454, D2Common #10867, stdcall(path)
	uint32_t (*pathY)(uint32_t pPath);        // thunk 0xA3B8, D2Common #10750, stdcall(path)
};

extern "C" void D2Game_DFB30_FillUnitInfo(uint32_t pUnit, uint32_t pInfo, const D2GameUnitInfoEnv* env)
{
	using d2raw::P; using d2raw::W;
	uint8_t* u = P(pUnit); uint8_t* i = P(pInfo);
	uint32_t pd = 0;
	if (W(u, 0) == 0) pd = env->playerData(pUnit);
	if (pd) { const uint8_t* s = P(pd); uint32_t k = 0; do { i[0x24 + k] = s[k]; } while (s[k++]); }
	else i[0x24] = 0;
	W(i, 0) = W(u, 0); W(i, 4) = W(u, 4); W(i, 8) = W(u, 0xC); W(i, 0xC) = W(u, 0x10);
	const int32_t t = static_cast<int32_t>(W(u, 0));
	const bool stat = t == 2 || (t > 3 && t < 6);
	const uint32_t path = W(u, 0x2C);
	W(i, 0x10) = stat ? W(P(path), 0xC) : path ? env->pathX(path) : 0;
	W(i, 0x14) = stat ? W(P(path), 0x10) : path ? env->pathY(path) : 0;
	W(i, 0x18) = W(u, 0x94); W(i, 0x1C) = W(u, 0x98);
}
