// D2Common batch: A* neighbour evaluation. Callees are the vendored d2moo AStar functions
// (real definitions, same signatures); retail passes the current node in EBX.
#include <cstdint>
#include <emscripten.h>
#include "Path/AStar.h"

extern "C" {
// 1.13c D2Common+0x7AFB0: relax or insert the neighbour tNew of node cur; 0 when node storage is full.
EMSCRIPTEN_KEEPALIVE uint32_t d2_common_7afb0_evaluate_neighbor(D2PathFoWallContextStrc* ctx, D2PathFoWallNodeStrc* cur, uint32_t tNew, uint32_t tTarget) {
 D2PathPointStrc nw; nw.X = (uint16_t)tNew; nw.Y = (uint16_t)(tNew >> 16);
 D2PathPointStrc tg; tg.X = (uint16_t)tTarget; tg.Y = (uint16_t)(tTarget >> 16);
 return (uint32_t)PATH_AStar_EvaluateNeighbor(nullptr, ctx, cur, nw, tg);
}
}
