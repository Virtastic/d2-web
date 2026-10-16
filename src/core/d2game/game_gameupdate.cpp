// 1.13c D2Game+0x2E050 replay (s3276): the frame sequencer in gameupdate.h run over a vector record's input words.
#include "gameupdate.h"
#include <emscripten.h>
static UpRec gUp;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_up_set(int i, unsigned v) { reinterpret_cast<uint32_t*>(&gUp)[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_up_run() { UpCtx x{}; x.r = &gUp; UP_RUN(x); }
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_up_out(int i) { return reinterpret_cast<uint32_t*>(&gUp)[UP_IN + i]; }
