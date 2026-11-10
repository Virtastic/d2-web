// The game-start handler of the 1.13c client (server packet 0x01, 8 bytes): the part that sets the
// game facts and puts the in-game UI back to its start state. Pure state; the caller hands in what
// the handler reads from the player unit and the configuration.
#pragma once
#include <cstdint>

namespace d2client_afa90 {

constexpr int kPanelCount = 0x26;        // the handler refuses any other table length
constexpr int kPanelGame = 0x00;
constexpr int kPanelMiniPanel = 0x15;
constexpr int kPanelHelpButton = 0x22;

struct State {
    uint32_t difficulty = 0;             // packet[1]
    uint32_t gameFlags = 0;              // packet[2..5]
    uint32_t expansion = 0;              // packet[6]
    uint32_t ladder = 0;                 // packet[7]
    uint32_t panel[kPanelCount] = {};    // one open flag per UI panel
    uint32_t panelMode = 0;              // which side panels shift the view
    uint32_t hasActiveNpc = 0;           // the NPC whose menu is open
    uint32_t activeNpc = 0;
    uint32_t listTop = 0;                // first visible row and selected row of the scrolling list
    uint32_t listSelected = 0;
    uint32_t keysLoaded = 0;
};

struct Env {
    bool playerPresent = false;          // the client's own unit exists
    uint32_t playerType = 0;
    bool playerBusy = false;             // +0xA5440: dying or dead, or flagged 0x10000
    bool miniPanelPinned = false;        // the stored "Mini Panel" option is non-zero: the toggle is skipped
};

// True when a panel may be opened for this player: no unit yet, or a player unit that is not busy.
bool panel_may_open(const Env& env);

// Applies the handler. Returns false (state untouched) when the packet is not an 8-byte 0x01.
bool sub_AFA90(State& state, const uint8_t* packet, int size, const Env& env);

}  // namespace d2client_afa90
