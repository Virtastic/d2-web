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

// 1.13c D2Client+0x1B700 -> 0x1B1D0 -> 0x18880 (the cast-on-unit tail of 0x1B7F0, after 0x18F90 accepted the use): a target
// out of melee reach (0xC6E2 false) for a skill whose Skills.txt srvdofunc (record +0x2E) is 0x13 (Inferno, Arctic Blast) is
// range-checked by 0x18880: range = the skill's aurarangecalc (record +0x64) at its level, distance = D2Common's unit distance;
// distance <= range casts (0x1A060), else 0x1B1D0 walks the client unit toward the target (0x1A010 -> 0x2A7C0) and the client
// starts no skill, so it creates none of the skill's client missiles (a retail call trace: 0x1A060 never runs; the walk's first
// footfall plays and no Inferno blast does). Returns 1 when the client walks.
static int32_t g_castWalked = 0;
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_cast_walks(uint32_t srvDoFunc, int32_t inMelee, int32_t distance, int32_t range) {
    g_castWalked = srvDoFunc == 0x13 && !inMelee && distance > range;
    return g_castWalked;
}
// 1 while the local player's last cast on a unit walked instead of casting (no client skill, no client missile).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_cast_walked() { return g_castWalked; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_cast_walk_reset() { g_castWalked = 0; }
