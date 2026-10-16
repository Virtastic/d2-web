// 1.13c D2Game+0x2C830 / +0x2D670 / +0x2C120 replay (s3272-s3274): the shared client-walker definition in clientsweep.h run over a vector record's input words.
#include "clientsweep.h"
#include <emscripten.h>
static SwRec gSw;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_sweep_set(int i, unsigned v) { reinterpret_cast<uint32_t*>(&gSw)[i] = v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_sweep_run(int fn)
{
    memset(&gSw.cf174o, 0, sizeof(uint32_t) * (8 + 8 * SW_LOG));
    SwCtx x{}; x.r = &gSw; x.fn = fn; SW_RUN(x);
}
extern "C" EMSCRIPTEN_KEEPALIVE unsigned d2_sweep_out(int i) { return reinterpret_cast<uint32_t*>(&gSw)[SW_OUT + i]; }
