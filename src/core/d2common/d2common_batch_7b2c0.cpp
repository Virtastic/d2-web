// D2Common 1.13c batch: delegates to the vendored d2moo function (same body, real structs).
#include <cstdint>
#include <emscripten.h>
#include "Path/AStar.h"

BOOL PATH_AStar_TargetLocationHasEnoughRoom(D2PathInfoStrc* pPathInfo);

extern "C" {
// 1.13c D2Common+0x7B2C0: true when no target unit, else true if any of the 8 +-2 probes around the target is collision-free.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7b2c0(D2PathInfoStrc* a) {
 return PATH_AStar_TargetLocationHasEnoughRoom(a) ? 1 : 0;
}
}
