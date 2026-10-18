// 1.13c D2Game 0x2D9B0 replay (s3275): the per-frame client state machine in gameframe.h run over a vector record's input words.
#include "gameframe.h"
#include <emscripten.h>
static GfRec gGf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_gf_set(int i, unsigned v) { reinterpret_cast<uint32_t*>(&gGf)[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_gf_run() { GfCtx x{}; x.r = &gGf; GF_RUN(x); }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_gf_out(int i) { return reinterpret_cast<uint32_t*>(&gGf)[GF_IN + i]; }
