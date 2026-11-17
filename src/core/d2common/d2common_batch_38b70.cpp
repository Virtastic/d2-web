// D2Common+0x38B70 now runs as STATLIST_UnitGetStatValue (src/core/d2moo/src/D2StatList.cpp); the probes' entry
// point stays a forward to it.
#include <cstdint>
#include <emscripten.h>

#include "D2StatList.h"
#include "Units/Units.h"

extern "C" {

EMSCRIPTEN_KEEPALIVE int32_t d2_port_d2common_38b70(D2UnitStrc* pUnit, int32_t nStatId, uint16_t nLayer)
{
	return STATLIST_UnitGetStatValue(pUnit, nStatId, nLayer);
}
}
