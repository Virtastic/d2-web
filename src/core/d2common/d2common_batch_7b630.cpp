// D2Common 1.13c batch: delegates to the vendored d2moo A* driver, which calls 7B2C0, 7AE20, 7B220,
// 7B3C0 and 7AB50 directly (hidden register args are explicit parameters there).
#include <cstdint>
#include <emscripten.h>
#include "Path/AStar.h"

extern "C" {
// Vendored A* driver wrapper (the 1.13c D2Common+0x7B630 seam port is d2common_7b630.cpp); returns the number of path points written.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_astar_compute_path_vendored(D2PathInfoStrc* info) {
 return (uint32_t)PATH_AStar_ComputePath(info);
}
}
