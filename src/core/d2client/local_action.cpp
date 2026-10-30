// 1.13c D2Client+0x2A750: the client starts its own local action when a cast is
// issued, before the server's reply, and that action's mode plays a sound of its own. A cast on the ground starts mode 2
// (WL: 24490, then 1C150 -> 23C00 plays the first footfall). The session command notes the mode here; the platform layer
// takes it once. (A cast on a unit starts mode 7, 24490 -> 242B0: not wired, see the spec.)
#include <cstdint>
#include <emscripten.h>

// The mode is taken one tick after the cast, as the walk footfalls are (the client submits the sound one tick after
// the unit enters the mode).
static int32_t g_localAction = 0, g_localActionNext = 0;

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_note_local_action(int32_t mode) { g_localActionNext = mode; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_take_local_action() {
    int32_t m = g_localAction;
    g_localAction = g_localActionNext;
    g_localActionNext = 0;
    return m;
}
// Who started the player's current run: an explicit move command (the client predicts the move and its unit drops to the walk/town-walk
// mode after the first tick, a retail recording/a retail recording F rows) or a cast that walks the player (the client unit follows the server and stays in run mode 3,
// a retail recording ama-10 ticks 12-44: every footfall the run group). 23C00 reads that mode at 0x23C18.
static int32_t g_explicitMove = 0;
extern "C" void d2_client_note_move_cmd(int32_t explicitMove) { g_explicitMove = explicitMove; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_explicit_move() { return g_explicitMove; }
