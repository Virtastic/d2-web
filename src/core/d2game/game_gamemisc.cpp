// 1.13c D2Game+0x2CA20 / 0x29BE0 / 0x2CF30 / 0x2C0F0 replay (s3277-s3280): the models in gamemisc.h run over a vector record's input words.
#include "gamemisc.h"
#include <emscripten.h>
static GmRec gGm;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_gm_set(int i, unsigned v) { gGm.in[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_gm_run(unsigned fn) { GmCtx x{}; x.r = &gGm; x.fn = fn; GM_RUN(x); }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_gm_out(int i) { return reinterpret_cast<uint32_t*>(&gGm)[GM_IN + i]; }
