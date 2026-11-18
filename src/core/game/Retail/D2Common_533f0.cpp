// Fog message 0x6AE fatal exit, moved from the stand-alone d2common tiny batch into the game tree.
#include <cstdint>
#include <cstdlib>
#include <emscripten.h>

// 1.13c D2Common+0x533f0 (#10924): required Fog seams preserve native text/context
// pointers; retail always terminates after reporting the error.
using D2CommonFogErrorTextFn = const char* (*)(uint32_t);
using D2CommonFogDisplayHaltFn = void (*)(void*, const char*);
using D2CommonExitFn = void (*)(int);
static void D2Common_ExitDefault(int code) { std::_Exit(code); }
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_533F0_FatalExit(
    void* errorContext, D2CommonFogErrorTextFn getErrorText,
    D2CommonFogDisplayHaltFn displayHalt, D2CommonExitFn fnExit = D2Common_ExitDefault)
{
    displayHalt(errorContext, getErrorText(0x6AE));
    fnExit(-1);   // retail _exit(-1) never returns; a probe's seam may
}
