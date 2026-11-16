// 1.13c D2Client+0x7D3C0: an NPC's greeting opens the speech window. Reached from the 0x27 handler
// (0x7DE50 builds the menu text list through D2Common 10204/10485/10151) and 0x4B8A0, which takes
// the first menu string id (10778, node 0) and passes it here. With no panel open (DAT 0x113A68 == 0)
// the window is built, the {sound, string} table at 0xDF398 (gate DAT 0xDF0D8 = 1; first match, a
// zero sound ends it) picks the spoken line, 0x88A70(player, 5, 0, 0) submits it and the handle lands
// in DAT 0x113B0B; the first window of the game (DAT 0x123448) also reads the tick and registers the
// window through D2Gfx 10048, whose edx is that handle.
// Not modelled: the per-frame update and end 0x7EA90/0x7E970/0x7BB40 and the continuation 0x4B230.
#include <cstdint>
#include <cstdio>
#include <string>
#include "../d2moo/include/D2Text.h"
#include "../d2moo/include/D2PacketDef.h"
#include "sound_unit_refs.h"

namespace {
struct Pair { int32_t sound; int16_t string; };
const Pair kSpeech[] = {
#include "npc_speech_sounds.inc"   // 0xDF398, 864 {sound, string} pairs
    , {0, 0}
};
bool g_panelUp = false;
bool g_registered = false;
std::string g_pendingG;
}

void d2client_npc_speech_reset() { g_panelUp = false; g_registered = false; g_pendingG.clear(); }

std::string d2client_npc_speech_take_g() { std::string s; s.swap(g_pendingG); return s; }

extern "C" void d2_client_npc_speech_packet(const D2GSPacketSrv27* p27, uint32_t guid) {
    if (!p27 || p27->nUnitType != 1 || p27->dwUnitGUID != guid || !d2client_sound_armed()) return;
    D2TextHeaderStrc* list = TEXT_AllocTextHeader(nullptr);
    if (!list) return;
    D2MessageListStrc msgs = p27->pMessageList;
    TEXT_CreateTextHeaderFromMessageList(list, &msgs);
    TEXT_SortTextNodeListByStringId(list);
    const int16_t w = TEXT_GetStringIdOfMenuType0Or2Node(list, 0);
    TEXT_FreeTextHeader(list);
    if (w == -1 || g_panelUp) return;
    g_panelUp = true;
    int32_t handle = 0;
    for (const Pair* p = kSpeech; p->sound != 0; ++p)
        if (p->string == w) { handle = d2client_sound_speech_submit(p->sound); break; }
    if (!g_registered) {
        g_registered = true;
        char b[32];
        std::snprintf(b, sizeof b, "10048 0,%x\n", (unsigned)handle);
        g_pendingG += b;
    }
}
