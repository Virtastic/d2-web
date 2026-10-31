// 1.13c D2Client+0x9d280 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2_client_9d280(uint32_t, void (*)(uint32_t));
static void fatal(uint32_t) { __builtin_trap(); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_9d280(uint32_t* r) { r[4] = d2_client_9d280(r[0], fatal); }
