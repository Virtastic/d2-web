// Fog data and clock symbols for the wasm link (single-threaded: see fog_lock_shim.cpp for the lock no-ops).
// The Fog ports (fog_batch_s7.cpp, fog_qserver_rate.cpp) call these by plain C name.
#include <cstdint>

// The critical-section no-ops themselves live in fog_lock_shim.cpp.

// Fog's log rate-limit timestamp (retail .data DAT_6ffa6ed4, read by fog_qserver_rate.cpp). Zero at load, as in retail.
uint32_t DAT_6ffa6ed4 = 0;

// The wasm clock: the compat Windows.h GetTickCount() forwards to d2web_GetTickCount (game_net_stubs.cpp). Fog's
// files declare the plain C name, so it needs a C-linkage definition here.
extern "C" uint32_t d2web_GetTickCount(void);
extern "C" uint32_t GetTickCount(void) { return d2web_GetTickCount(); }

// Retail .rdata tag string (DAT_6ff77040) passed to the 190A0 log. Empty here: the log path is gated closed by the
// rate limit (DAT_6ffa6ed4 is pinned to the current tick), so the tag is never printed in the Fog vectors.
extern "C" const char DAT_6ff77040[] = "";
