// 1.13c D2Client+0xAFA90
#include "d2client_afa90.h"

namespace d2client_afa90 {

bool panel_may_open(const Env& env) {
    return !env.playerPresent || (env.playerType == 0 && !env.playerBusy);
}

bool sub_AFA90(State& state, const uint8_t* packet, int size, const Env& env) {
    if (!packet || size != 8 || packet[0] != 0x01) return false;
    state.difficulty = packet[1];
    state.gameFlags = static_cast<uint32_t>(packet[2]) | (static_cast<uint32_t>(packet[3]) << 8) |
                      (static_cast<uint32_t>(packet[4]) << 16) | (static_cast<uint32_t>(packet[5]) << 24);
    state.expansion = packet[6];
    state.ladder = packet[7];

    // The NPC interaction block is zeroed: no menu is open, no NPC is the active one.
    state.hasActiveNpc = 0;
    state.activeNpc = 0;

    // Every panel closes, then the three that a running game always has come back.
    state.panelMode = 0;
    for (uint32_t& open : state.panel) open = 0;
    const bool may = panel_may_open(env);
    if (may) state.panel[kPanelGame] = 1;
    if (!env.miniPanelPinned && may) state.panel[kPanelMiniPanel] ^= 1u;   // a toggle, on a flag just cleared
    if (may) state.panel[kPanelHelpButton] = 1;

    state.keysLoaded = 1;
    state.listTop = 0;
    state.listSelected = 0;
    return true;
}

}  // namespace d2client_afa90
