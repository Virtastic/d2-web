// 1.13c D2Game+0xDF7B0 probe, FUNCTION mode, a retail recording: the row's rule replayed
// against the retail vector. Descriptor regs esi / args 4: r[0] = pUnit (ESI), r[1] = pGame (the callee's one
// stack word), r[2] = the `sc` blob (hook return values: 0xa622 -> the tuple's frame bonus), r[3] = the log
// blob. 0xA622 and 0x8EA80 are logged exactly as the descriptor's hooks record them (10 words per call).
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "../d2game/sunit_events_df7b0.h"

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_df7b0(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[3], (const uint32_t*)(uintptr_t)r[2]);
    df7b0::Schedule(e, r[1], r[0]);
    r[4] = 0;   // void: the descriptor's retmask 0x0 makes the stale eax not a record
}
