// D2Common 1.13c batch: delegates to the vendored d2moo function (real structs; the callees
// 4CE90 / 7AFB0 are called inside it, no fn-pointer args).
#include <cstdint>
#include <emscripten.h>
#include "Path/AStar.h"

extern "C" {
// 1.13c D2Common+0x7B3C0: evaluate each of 8 neighbours of node p that is not blocked; 0 if any evaluation fails.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7b3c0(D2PathInfoStrc* info, D2PathFoWallContextStrc* ctx, D2PathFoWallNodeStrc* p, uint32_t tgt) {
 D2PathPointStrc t; t.X = (uint16_t)tgt; t.Y = (uint16_t)(tgt >> 16);
 return (uint32_t)PATH_AStar_ExploreChildren(info, ctx, p, t);
}
}
