// 1.13c D2Common implementation batch: +0x22590, +0x2FED0, +0x39050, +0x38E80, +0x38C20, +0x2D070.
// Bodies translated from the pinned 1.13c decompiles. Retail pointers are 32-bit image addresses in
// the offsets below; unit fields with a vendored twin are read by name. Callees that are still
// UNMAPPED rows are env-seam function pointers (null until the environment installs them). Fatal
// paths (Fog message + _exit(-1)) are modelled as returns, as in the other d2common batch files.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Item.h"
#include "Units/Units.h"
#include "Path/Path.h"

static_assert(offsetof(D2ItemsTxt, dwBitField1) == 0xDC, "items.txt bitfield 1");
static_assert(offsetof(D2ItemsTxt, nInvWidth) == 0x10F, "items.txt inventory width");
static_assert(offsetof(D2ItemsTxt, nInvHeight) == 0x110, "items.txt inventory height");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(void* b, int o) { return *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b) + o); }
}

// D2Common+0x22590 now runs as ITEMS_GetRealmData (src/core/d2moo/src/Items/Items.cpp).

// 1.13c D2Common+0x2FED0 (#10574, D2SetEnabledStat): sets the unit's static path (unit+0x2C) to a
// room and a game position: +0x00 room, +0x0C/+0x10 game X/Y, +0x04 client X = (X - Y) * 0x10,
// +0x08 client Y = (X + Y) * 8, +0x1C direction byte = 0. Written by field: the wasm64 path holds an
// 8-byte room pointer, so the retail dword offsets do not address it. A null unit is fatal 0x866 in
// retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2SetEnabledStat(D2UnitStrc* unit, D2ActiveRoomStrc* pRoom, int32_t nX,
                                                int32_t nY)
{
	if (!unit)
		return;
	D2StaticPathStrc* path = unit->pStaticPath;
	path->pRoom = pRoom;
	path->tGameCoords.nX = nX;
	path->dwClientCoordX = (nX - nY) * 0x10;
	path->nDirection = 0;
	path->tGameCoords.nY = nY;
	path->dwClientCoordY = (nX + nY) * 8;
}

// D2Common+0x39050 now runs as STATLIST_GetMaxStaminaFromUnit (src/core/d2moo/src/D2StatList.cpp).

// D2Common+0x38C20 now runs as STATLIST_GetUnitBaseStat (src/core/d2moo/src/D2StatList.cpp); the probes' entry
// point stays a forward to it.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_38c20(D2UnitStrc* pUnit, int32_t nStatId, int32_t nLayer)
{
	return STATLIST_GetUnitBaseStat(pUnit, nStatId, static_cast<uint16_t>(nLayer));
}

