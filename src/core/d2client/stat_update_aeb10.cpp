// Browser entry for D2Client+0xAEB10, the hook D2Client+0xAEDF0 (server packets 0x19-0x1F, the hero's
// stat updates) runs after it writes the stat: the retail port d2c_aeb10 is called with an env that
// records which callees fire, so the browser learns what retail does on that stat change without a copy
// of the switch. A level change (stat 0xC) re-checks the unit's equipped items (0xAE3F0, then 0x82FE0) and,
// for the local hero, its inventory items (0xAC970) and its equipped items again (0x82FE0); strength and
// dexterity (0, 2) re-check the equipped items; hit points written while the unit is in mode 0x11 (dead)
// run 0x2A6C0 then 0xA6210. Nothing else does anything: 1.13c starts no overlay, animation or button
// flash on a level change (the level-up cue is the 0x2C sound event 2, D2Client+0x23200).
#include <cstdint>
#include <emscripten.h>
#include "unit_event_aeb10.h"

namespace {
uint32_t g_actions;
void equip(void*) { g_actions |= 1u; }        // D2Client+0x82FE0
void dead_a(void*) { g_actions |= 8u; }             // D2Client+0x2A6C0
void dead_b(void*) { g_actions |= 8u; }             // D2Client+0xA6210
void owner(void*) { g_actions |= 2u; }         // D2Client+0xAE3F0
void inventory(void*) { g_actions |= 4u; }          // D2Client+0xAC970
}

extern "C" {
// bit 0 0x82FE0 equipped-item re-check, bit 1 0xAE3F0, bit 2 0xAC970, bit 3 0x2A6C0+0xA6210.
// unit_mode is the unit's mode at +0x10 (0x11 = dead).
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_stat_update_actions(int32_t stat, int32_t value, int32_t is_local, int32_t unit_mode) {
    alignas(4) uint8_t unit[0x14] = {};
    *reinterpret_cast<int32_t*>(unit + 0x10) = unit_mode;
    alignas(4) uint8_t other[4] = {};
    const D2cAEB10Env env{equip, dead_a, dead_b, owner, inventory, is_local ? static_cast<void*>(unit) : static_cast<void*>(other)};
    g_actions = 0;
    d2c_aeb10(&env, unit, stat, value);
    return g_actions;
}
}
