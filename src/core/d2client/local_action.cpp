// 1.13c D2Client+0x2A750: the client starts its own local action when a cast is
// issued, before the server's reply, and that action's mode plays a sound of its own. A cast on the ground starts mode 2
// (WL: 24490, then 1C150 -> 23C00 plays the first footfall). The session command notes the mode here; the platform layer
// takes it once. (A cast on a unit starts mode 7, 24490 -> 242B0: not wired, see the spec.)
#include <cstdint>
#include <emscripten.h>

static int32_t g_localAction = 0;

extern "C" void d2_client_note_local_action(int32_t mode) { g_localAction = mode; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_take_local_action() { int32_t m = g_localAction; g_localAction = 0; return m; }
