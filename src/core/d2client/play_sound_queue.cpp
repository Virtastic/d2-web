// The play path's sound queue: 1.13c D2Client 0x88A70 (submit) and 0x88DD0 (the per-tick pump) over the typed entry
// list of play_sound_queue.h. The browser mixer submits a unit's rows here with their start offsets, runs the pump
// once per client tick and plays the entries the pump started, with the variant row 0x5D7A0 picked; it reports a
// source that ended so its entry stops. Positions, Falloff range, gain, pan and fades stay with the mixer: the
// pump treats every entry as in range with voices free (ordinal 10001 > 0, 10011 true), which is the shell's state
// while its Web Audio context runs.
#include <cstring>
#include <emscripten.h>
#include "play_sound_queue.h"
#include "d2client_5dfd0.h"

extern "C" uint32_t d2_session_client_rand(uint32_t n);

namespace d2client_playsnd {
Entry g_pool[kPool];
int32_t g_list[kPool];
int32_t g_listLen = 0;
uint32_t g_now = 0;
namespace {
int32_t g_handle = 0;          // DAT 0x10BF38
uint64_t g_hero = 0;           // DAT 0x10BBFC, the client's own unit
bool g_seedOn = false;         // a pinned seed (the deterministic tests) in place of the client hero seed
uint32_t g_seed[2] = { 0, 0 };
struct Started { int32_t handle, asked, row; uint32_t start; int32_t kind; uint64_t unit; };
Started g_started[kPool];
int32_t g_nStarted = 0;

void drop(int32_t at) {
    g_pool[g_list[at]].used = false;
    for (int32_t i = at + 1; i < g_listLen; ++i) g_list[i - 1] = g_list[i];
    --g_listLen;
}
// 0x880E0: a playing entry stops (its voice is freed, the entry is done)
void stop(Entry* e) { if (e->state == 1) { e->state = 2; e->playing = 0; } }
const uint8_t* recOf(const Entry* e) { return rec(e->id); }
bool before(const Entry* a, const Entry* b) {   // 0x88DD0's bubble order: swap when this is false for (b, a)
    if (a->prio != b->prio) return a->prio > b->prio;
    if (a->dist2 != b->dist2) return a->dist2 < b->dist2;
    if (a->start != b->start) return a->start > b->start;
    return a > b;
}
}

uint8_t* rec(int32_t id) {
    if (!d2client_sounds::records()) d2client_sounds::load();
    uint8_t* r = d2client_sounds::records();
    return (r && id >= 0 && id < d2client_sounds::recordCount()) ? r + (size_t)id * 0x8E : nullptr;
}
// 1.13c D2Client+0x89640 on the client hero seed (or the pinned one)
uint32_t seedRand(uint32_t n) {
    if (!g_seedOn) return d2_session_client_rand(n);
    if (!n) return 0;
    const uint64_t v = (uint64_t)g_seed[0] * 0x6ac690c5ull + g_seed[1];
    g_seed[0] = (uint32_t)v; g_seed[1] = (uint32_t)(v >> 32);
    return (n & (n - 1)) ? (uint32_t)v % n : ((n - 1) & (uint32_t)v);
}

// 1.13c D2Client+0x88A70 (EBX = row, unit, offset, flags): queue a row on a unit. A row with Volume 0 is refused.
// A Compound row joins a live entry of its group (0x87FE0) instead of a new one. The entry starts at now + offset
// with the row's Priority, 'P' higher on the client's own unit. Returns the entry's handle, 0 when refused.
int32_t submit88a70(int32_t id, uint64_t unit, uint32_t off, int32_t flags, float dist2, int32_t kind) {
    if (id < 1) return 0;
    const uint8_t* r = rec(id);
    if (!r || r[0x3C] == 0) return 0;
    Entry* e = nullptr;
    int16_t comp; memcpy(&comp, r + 0x45, 2);
    if (comp != 0) e = find87fe0(id, nullptr);
    if (!e) {
        int32_t slot = -1;
        for (int32_t i = 0; i < kPool; ++i) if (!g_pool[i].used) { slot = i; break; }   // 0x87D80
        if (slot < 0) return 0;
        e = &g_pool[slot];
        memset(e, 0, sizeof *e);
        e->used = true;
        e->handle = ++g_handle;
        g_list[g_listLen++] = slot;
        e->flags = flags; e->id = id; e->asked = id; e->start = g_now + off; e->volume = 0xFF;
        e->prio = r[0x4E]; e->dist2 = unit ? dist2 : 0.0f; e->kind = kind;
    }
    if (unit && unit == g_hero) e->prio = (uint8_t)(e->prio + 'P');
    if (unit) { if (!e->units) e->unit = unit; ++e->units; }
    return e->handle;
}

// 1.13c D2Client+0x88DD0 (now): sort the list, retire killed entries, then start every queued entry that is due.
void pump88dd0(uint32_t now) {
    g_now = now;
    g_nStarted = 0;
    for (bool done = false; !done;) {
        done = true;
        for (int32_t i = 0; i + 1 < g_listLen; ++i)
            if (!before(&g_pool[g_list[i]], &g_pool[g_list[i + 1]])) { const int32_t t = g_list[i]; g_list[i] = g_list[i + 1]; g_list[i + 1] = t; done = false; }
    }
    for (int32_t i = 0; i < g_listLen; ++i) {
        Entry* e = &g_pool[g_list[i]];
        if (!e->killed) continue;
        if (e->state == 0) e->state = 2;
        if (e->state == 1) stop(e);
    }
    for (int32_t i = 0; i < g_listLen;) {
        Entry* e = &g_pool[g_list[i]];
        const uint8_t* r = recOf(e);
        const uint8_t loop = r ? r[0x3E] : 0;
        uint16_t dur = 0; if (r) memcpy(&dur, r + 0x43, 2);
        if (e->state == 2) {
            if (!loop || dur != 0 || e->killed) { drop(i); continue; }
            e->state = 0;                                    // a looping row requeues
        } else {
            if (e->state == 0 && e->start <= now && e->volume != 0) {
                const bool defer = r && r[0x41], stopInst = r && r[0x42];                Entry* o = nullptr;
                if (defer || stopInst) { int32_t g = 0; if (r) memcpy(&g, r + 0x60, 4); o = inst87c30(g, e); }
                if (e->units == 1 && e->id > 0xB75 && e->id < 0x1231) o = busy87bd0(e);
                // retail: (!Defer || a unit list || no fixed position) && o; the entry is never given a fixed position
                // (+0x0C..+0x14 stay 0), so the test is o alone
                if (o) {
                    if (!stopInst) { e->killed = 1; ++i; continue; }
                    o->killed = 1;
                }
                if (start5d7a0(e)) {
                    e->state = 1;
                    g_started[g_nStarted++] = { e->handle, e->asked, e->id, e->start, e->kind, e->unit };
                }
                r = recOf(e);
                if (!e->playing && !(r && r[0x3E])) { drop(i); continue; }
            }
            if (e->state == 1) {
                r = recOf(e);
                uint16_t d = 0; if (r) memcpy(&d, r + 0x43, 2);
                if (d != 0 && (uint32_t)d < now - e->start) stop(e);
            }
        }
        ++i;
    }
}
}

using namespace d2client_playsnd;
// Queue a Sounds row on a unit (0 none) to start `off` ticks after the current sound clock; flags = 0x88A70's p3
// (bit 0: play the row itself, no variant pick); dist2 = squared distance to the listener (the pump's sort key);
// kind = the caller's tag, returned with the start. Returns the entry handle, 0 when refused.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_play_sound_submit(int32_t row, uint32_t unit, uint32_t off, int32_t flags, int32_t kind, float dist2) {
    return submit88a70(row, unit, off, flags, dist2, kind);
}
// One pump at sound clock `now`; returns how many entries started. Read each with d2_client_play_sound_started(i, f):
// f 0 handle, 1 the row asked, 2 the row started (the group variant), 3 its start tick, 4 kind, 5 unit.
// 0x5F020 runs 0x88DD0 on its clock (0x11C268), then steps the clock: a submit after this pump starts `off` ticks
// after now + 1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_play_sound_pump(uint32_t now) { pump88dd0(now); g_now = now + 1; return g_nStarted; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_play_sound_started(int32_t i, int32_t f) {
    if (i < 0 || i >= g_nStarted) return 0;
    const auto& s = g_started[i];
    switch (f) { case 0: return s.handle; case 1: return s.asked; case 2: return s.row; case 3: return (int32_t)s.start; case 4: return s.kind; case 5: return (int32_t)s.unit; }
    return 0;
}
// The browser source of an entry finished (or never sounded): its voice is free and the entry is done.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_play_sound_ended(int32_t handle) {
    for (int32_t i = 0; i < g_listLen; ++i) if (g_pool[g_list[i]].handle == handle) stop(&g_pool[g_list[i]]);
}
// 0x87BD0 with no entry to skip: a row of the local voice range 0xB76..0x1230 still plays.
// An entry's state: 0 queued, 1 playing, 2 done, -1 no longer in the list (dropped by the pump).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_play_sound_state(int32_t handle) {
    for (int32_t i = 0; i < g_listLen; ++i) if (g_pool[g_list[i]].handle == handle) return g_pool[g_list[i]].state;
    return -1;
}
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_play_sound_busy() { return busy87bd0(nullptr) ? 1 : 0; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_play_sound_now() { return g_now; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_play_sound_hero(uint32_t unit) { g_hero = unit; }
// 0x242B0's draw (lo32 % 100) for a monster attack entry whose gate opened on the play path: the value the session drew
// at the order's arrival (monster_face.cpp), or the pinned seed's draw.
extern "C" int d2_client_attack_roll_take(uint32_t id);
extern "C" int32_t d2_client_play_sound_attack_roll(uint32_t id) {
    return g_seedOn ? (int32_t)seedRand(100) : d2_client_attack_roll_take(id);
}
// 0x5EE70 for a row played outside the queue (the shell's immediate sources): the variant, with the row's pick
// history shifted as 0x5D7A0 does on a start.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_play_sound_variant(int32_t row) {
    uint8_t* r = rec(row);
    const int32_t v = d2_client_5ee70(d2client_sounds::records(), d2client_sounds::recordCount(), row, seedRand);
    if (r) { memcpy(r + 0x68, r + 0x64, 4); memcpy(r + 0x64, &v, 4); }
    return v;
}
// Clear the queue, the handle counter, every row's pick history and the clock. on != 0 pins the variant draws to the
// seed {lo, hi} (tests); on == 0 draws on the client hero seed.
extern "C" void d2_client_unit_mode_sound_reset();
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_play_sound_reset(uint32_t lo, uint32_t hi, int32_t on) {
    d2_client_unit_mode_sound_reset();
    memset(g_pool, 0, sizeof g_pool); g_listLen = 0; g_nStarted = 0; g_handle = 0; g_now = 0; g_hero = 0;
    g_seedOn = on != 0; g_seed[0] = lo; g_seed[1] = hi;
    rec(0);
    if (uint8_t* r = d2client_sounds::records())
        for (int32_t i = 0; i < d2client_sounds::recordCount(); ++i) memset(r + (size_t)i * 0x8E + 0x64, 0, 8);
}
