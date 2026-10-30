// 1.13c D2Client+0x18F40 (called from the cast gate 0x18F90 when the local player is in a non-neutral mode) -> 0x23200.
// The mode picks an event through the ushort table at D2Client+0xF8508 (9 entries, read from
// the DLL: mode 1 -> 0x15, 2/3/4 -> 0x13, 9 -> 9, others 0). Event 0x13 is the class "impossible" voice (voice table +0x08);
// 0x23200 drops it when the same row played less than 0x4B clock units ago (globals 0x11C4CC clock / 0x11C4D0 row).
// Only 0x13 is wired (the row comes from the caller); the other events' rows are not needed by a recorded vector yet.
#include <cstdint>
#include <emscripten.h>

static uint32_t g_lastClock, g_lastRow, g_refused;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_cast_gate_reset() { g_lastClock = 0; g_lastRow = 0; g_refused = 0; }
// mode = the client player's mode as 0x22080 returns it, row13 = voice[+0x08] of the player's class, clock = [0x1197F8].
// Returns the Sounds.txt row retail submits (0 = nothing).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_cast_gate_row(uint32_t mode, uint32_t row13, uint32_t clock) {
    static const uint16_t ev[9] = { 0, 0x15, 0x13, 0x13, 0x13, 0, 0, 0, 0 };
    // 1.13c D2Client+0x18F90: use-states 0 (usable) and 5 (passive) accept the local action; every other state
    // returns 0 and the client never starts the skill, so it creates none of the skill's client missiles.
    g_refused = mode != 0 && mode != 5;
    if (mode >= 9 || ev[mode] != 0x13 || row13 == 0) return 0;
    if (row13 == g_lastRow && (uint32_t)(clock - g_lastClock) < 0x4B) return 0;
    g_lastClock = clock; g_lastRow = row13;
    return row13;
}

// 1 while the local player's last client cast was refused by 0x18F90 (no client skill, no client missile).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_local_cast_refused() { return g_refused; }
