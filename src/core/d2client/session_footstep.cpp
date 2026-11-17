// The session replay's local player footsteps: 1.13c D2Client+0x2B1E0 -> 0x1C150 -> 0x23C00 on the local player's client unit.
// The due rule is footstep_23c00.cpp's (d2_client_footstep_due); the row is 0x22FB0's (class group base + floor offset, + 24 in
// run); the submit goes to the sound pump's unit queue (0x88A70). Nothing is queued unless the A trace is armed.
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include "d2client_5dfd0.h"

void d2client_sound_unit_submit(int32_t row);
extern "C" int32_t d2_client_footstep_due(uint32_t cur, uint32_t total, uint32_t step, uint32_t n, uint32_t off, uint32_t now, uint32_t last);

// The local player's client unit (D2Client+0x11BBFC): its own mode, frame (+0x44), length (+0x48), step (+0x4C) and last footfall
// (+0x84). 0x2B1E0 -> 0x1C150 runs it once a client frame: a mode whose 0xF8418 record has c0 == 1 (walk 2, run 3, town walk 6)
// reaches 0x23C00 before the frame advances; run in town then goes through 0x19D80, which re-enters the unit in town walk at
// frame 0. The mode the client unit takes is the one the server path held on the previous frame (the client's own path is not
// ported: the move starts and ends on the frame after the server's).
namespace {
struct ClientPlayer { uint32_t mode = 0, frame = 0, total = 0, step = 0, last = 0, serverMode = 0, entered = 0; bool seen = false; };
ClientPlayer g_cp;
// D2Client+0xF7694[class] +0x28: the class footstep group base (read from the 1.13c image).
const uint32_t kClassGait[7] = { 2768, 2720, 2720, 2768, 2816, 2720, 2720 };
bool moving(uint32_t m) { return m == 2 || m == 3 || m == 6; }
}

// A held monster's client unit (0x4F4A0): its mode, frame +0x44, length +0x48, step +0x4C, last footfall +0x84, and the server
// unit's mode on the previous frame. The client enters a mode on the frame after the server (the mode packet), and leaves for
// neutral on the server's frame (its own path and animation end there too).
namespace {
struct ClientMonster { uint32_t mode = 1, cur = 0, total = 0, step = 0, last = 0, serverMode = 1; };
std::unordered_map<uint32_t, ClientMonster> g_cm;
std::unordered_map<uint32_t, bool> g_pathDone;   // units whose server walk/run path ended since their last client frame
int32_t g_neutral[2] = { 0, 0 };   // the last monster call's neutral-mode footstep and layer rows
}

extern "C" void d2client_session_footstep_reset() { g_cp = ClientPlayer{}; g_cm.clear(); g_pathDone.clear(); }
// The server's walk or run frame callback found its path at its end (#10342 false). The client's own copy of that path
// (0x2A700, #10342 on the client unit) steps with the server's and ends on the same frame, at the last point or at the same
// wall: 0x4DA60 then 0x2A6C0 put the client unit in neutral at frame 0, after the frame advance and before 0x23C00. A server
// path stopped by another unit's footprint (collided mask 0x1000, COLLIDE_NO_PATH) does not stop the client's copy, which walks
// on until the next path packet (in the retail captures every such stop keeps the client walking, every other one ends it).
extern "C" void d2_client_monster_path_done(uint32_t id, uint16_t collided) { if (!(collided & 0x1000)) g_pathDone[id] = true; }

extern "C" uint32_t d2_client_monster_footstep_row(int32_t classId, uint32_t mode, uint32_t cur, uint32_t total, uint32_t step, uint32_t now, uint32_t last, uint32_t floor);
extern "C" const uint32_t* d2_client_monster_footstep(int32_t classId);   // [Footstep, Layer, FsCnt, FsOff, FsPrb]
extern "C" uint32_t d2client_unitseed_start_frame(const void* game, uint32_t id);
namespace d2client_playsnd { uint32_t seedRand(uint32_t n); }   // 0x89640 on the client hero seed
// One client frame of a held monster (0x4F4A0): 0x4F2C0's frame advance (0xA3000) then 0x23C00, so the first frame in a mode is
// checked at cur == step. serverMode/step/total: the server unit's mode, +0x4C and +0x48 now; now: the client tick (frame - 1).
extern "C" void d2client_session_monster_footstep(uint32_t id, int32_t cls, uint32_t serverMode, uint32_t step, uint32_t total,
                                                  int32_t floor, uint32_t now, const void* game) {
    const bool fresh = !g_cm.count(id);
    ClientMonster& c = g_cm[id];
    g_neutral[0] = g_neutral[1] = 0;
    // a unit the client creates in neutral starts on the frame its init rolled (0x4D160: +0x44 = roll % +0x48), not on 0
    if (fresh && serverMode == 1 && total) { c.cur = d2client_unitseed_start_frame(game, id) % total; c.step = step; c.total = total; }
    const bool done = g_pathDone.erase(id) && (c.mode == 2 || c.mode == 15);   // the client unit's walk or run reached its path's end
    const uint32_t mode = serverMode == 1 || done ? 1u : c.serverMode;
    c.serverMode = serverMode;
    if (mode != c.mode) { c.mode = mode; c.cur = 0; c.step = step; c.total = total; }
    if (c.total && !done) c.cur = (c.cur + c.step) % c.total;
    const uint32_t row = d2_client_monster_footstep_row(cls, c.mode, c.cur, c.total, c.step, now, c.last, uint32_t(floor));
    if (!row) return;
    c.last = now;
    // 0x23D9B submits 0x22FB0's row (none for a record without Footstep); then a record with a layer row (+0x58) draws
    // rand(100) on the hero seed (0x89640) whatever FsPrb (+0x64) is, and submits the layer when the draw is below it.
    const uint32_t* ms = d2_client_monster_footstep(cls);
    if (!ms) return;
    if (ms[0]) { d2client_sound_unit_submit(int32_t(row)); if (c.mode == 1) g_neutral[0] = int32_t(row); }
    if (ms[1] && d2client_playsnd::seedRand(100) < ms[4]) { d2client_sound_unit_submit(int32_t(ms[1])); if (c.mode == 1) g_neutral[1] = int32_t(ms[1]); }
}
// The rows the last d2client_session_monster_footstep call submitted in neutral (a class whose 0x23C00 gate takes mode 1): the
// frame its client unit started on is its own (the init roll), so the play path takes these rows from here. 0 = none.
extern "C" void d2client_session_monster_neutral_rows(int32_t out[2]) { out[0] = g_neutral[0]; out[1] = g_neutral[1]; }

// One client frame of the local player's unit. serverMode: the server unit's mode now (taken for the next frame); now: the client
// tick counter D2Client+0x1197F8 (frame - 1); rate(mode) -> the client unit's step and length in that mode (D2Common 0x33110).
extern "C" void d2client_session_player_footstep(uint32_t serverMode, int32_t targetOnClient, uint32_t cls, int32_t inTown, int32_t floor, uint32_t now, int32_t staminaZero,
                                                 void (*rate)(uint32_t mode, uint32_t* step, uint32_t* total)) {
    uint32_t s = g_cp.serverMode; const bool seen = g_cp.seen;
    g_cp.serverMode = serverMode; g_cp.seen = true;
    // A skill mode the server puts on the player for the local command (attack, cast, kick, throw, special: 7-16) reaches the
    // client unit only through the client's own action on its target; no server packet names the player. When the target is a
    // unit the client does not hold (never announced), the client's walk has nothing to reach and goes on through the server's
    // skill: the client mode setter 0x2A6C0 never runs on the player again and its walk keeps stepping. With the target held,
    // the walk ends on the frame after the server's swing starts.
    const bool skill = s >= 7 && s <= 16;
    if (!seen || (!moving(s) && !(skill && !targetOnClient && moving(g_cp.mode)))) { g_cp.mode = 0; return; }
    if (!moving(s)) s = g_cp.entered;
    // A new moving mode (a walk/run toggle mid-move too) goes through D2Common #11090, which restarts the frame and the rate
    // when the mode differs. Run in town, already turned into town walk by 0x19D80, is the same mode the server still holds.
    if (!moving(g_cp.mode) || s != g_cp.entered) { g_cp.mode = s; g_cp.entered = s; g_cp.frame = 0; rate(s, &g_cp.step, &g_cp.total); }
    if (g_cp.step && g_cp.total && cls < 7 && d2_client_footstep_due(g_cp.frame, g_cp.total, g_cp.step, 2, 0, now, g_cp.last)) {   // 0x23C00, player: n 2, phase 0
        g_cp.last = now;
        const uint32_t f = floor >= 2 && floor <= 6 ? 4u * uint32_t(floor - 1) : 0u;
        d2client_sound_unit_submit(int32_t(kClassGait[cls] + f + (g_cp.mode == 3 ? 24u : 0u)));   // 0x22FB0
    }
    if (g_cp.mode == 3 && (inTown || staminaZero)) { const uint32_t w = inTown ? 6u : 2u; g_cp.mode = w; g_cp.frame = 0; rate(w, &g_cp.step, &g_cp.total); return; }   // 0x19D80: stamina 0 turns run into walk, town or not
    if (g_cp.total) g_cp.frame = (g_cp.frame + g_cp.step) % g_cp.total;
}

// The level's floor class fallback: SoundEnviron "Material 1" (0x58-byte records, +0x1C) of the given row; 1 without the table.
extern "C" int32_t d2client_session_level_material(uint32_t soundEnv) {
    soundEnv &= 0xff;   // 0x226A0 reads the row as a byte (level +0x21C), so the signed compare below never sees a negative index
    const uint8_t* e = d2client_sounds::envTable();
    if (!e || int32_t(soundEnv) >= d2client_sounds::envCount()) return 1;
    int32_t m; std::memcpy(&m, e + soundEnv * 0x58 + 0x1C, 4); return m;
}
