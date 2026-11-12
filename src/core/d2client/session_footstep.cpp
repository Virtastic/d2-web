// The session replay's local player footsteps: 1.13c D2Client+0x2B1E0 -> 0x1C150 -> 0x23C00 on the local player's client unit.
// The due rule is footstep_23c00.cpp's (d2_client_footstep_due); the row is 0x22FB0's (class group base + floor offset, + 24 in
// run); the submit goes to the sound pump's unit queue (0x88A70). Nothing is queued unless the A trace is armed.
#include <cstdint>
#include <cstring>
#include "d2client_5dfd0.h"

void d2client_sound_unit_submit(int32_t row);
extern "C" int32_t d2_client_footstep_due(uint32_t cur, uint32_t total, uint32_t step, uint32_t n, uint32_t off, uint32_t now, uint32_t last);

// The local player's client unit (D2Client+0x11BBFC): its own mode, frame (+0x44), length (+0x48), step (+0x4C) and last footfall
// (+0x84). 0x2B1E0 -> 0x1C150 runs it once a client frame: a mode whose 0xF8418 record has c0 == 1 (walk 2, run 3, town walk 6)
// reaches 0x23C00 before the frame advances; run in town then goes through 0x19D80, which re-enters the unit in town walk at
// frame 0. The mode the client unit takes is the one the server path held on the previous frame (the client's own path is not
// ported: the move starts and ends on the frame after the server's).
namespace {
struct ClientPlayer { uint32_t mode = 0, frame = 0, total = 0, step = 0, last = 0, serverMode = 0; bool seen = false; };
ClientPlayer g_cp;
// D2Client+0xF7694[class] +0x28: the class footstep group base (read from the 1.13c image).
const uint32_t kClassGait[7] = { 2768, 2720, 2720, 2768, 2816, 2720, 2720 };
bool moving(uint32_t m) { return m == 2 || m == 3 || m == 6; }
}

extern "C" void d2client_session_footstep_reset() { g_cp = ClientPlayer{}; }

// One client frame of the local player's unit. serverMode: the server unit's mode now (taken for the next frame); now: the client
// tick counter D2Client+0x1197F8 (frame - 1); rate(mode) -> the client unit's step and length in that mode (D2Common 0x33110).
extern "C" void d2client_session_player_footstep(uint32_t serverMode, uint32_t cls, int32_t inTown, int32_t floor, uint32_t now,
                                                 void (*rate)(uint32_t mode, uint32_t* step, uint32_t* total)) {
    const uint32_t s = g_cp.serverMode; const bool seen = g_cp.seen;
    g_cp.serverMode = serverMode; g_cp.seen = true;
    if (!seen || !moving(s)) { g_cp.mode = 0; return; }
    if (!moving(g_cp.mode)) { g_cp.mode = s; g_cp.frame = 0; rate(s, &g_cp.step, &g_cp.total); }
    if (g_cp.step && g_cp.total && cls < 7 && d2_client_footstep_due(g_cp.frame, g_cp.total, g_cp.step, 2, 0, now, g_cp.last)) {   // 0x23C00, player: n 2, phase 0
        g_cp.last = now;
        const uint32_t f = floor >= 2 && floor <= 6 ? 4u * uint32_t(floor - 1) : 0u;
        d2client_sound_unit_submit(int32_t(kClassGait[cls] + f + (g_cp.mode == 3 ? 24u : 0u)));   // 0x22FB0
    }
    if (g_cp.mode == 3 && inTown) { g_cp.mode = 6; g_cp.frame = 0; rate(6, &g_cp.step, &g_cp.total); return; }   // 0x19D80
    if (g_cp.total) g_cp.frame = (g_cp.frame + g_cp.step) % g_cp.total;
}

// The level's floor class fallback: SoundEnviron "Material 1" (0x58-byte records, +0x1C) of the given row; 1 without the table.
extern "C" int32_t d2client_session_level_material(uint32_t soundEnv) {
    const uint8_t* e = d2client_sounds::envTable();
    if (!e || int32_t(soundEnv) >= d2client_sounds::envCount()) return 1;
    int32_t m; std::memcpy(&m, e + soundEnv * 0x58 + 0x1C, 4); return m;
}
