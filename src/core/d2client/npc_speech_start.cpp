// 1.13c D2Client+0x7D3C0: an NPC's greeting opens the speech window. Reached from the 0x27 handler
// (0x7DE50 builds the menu text list through D2Common 10204/10485/10151) and 0x4B8A0, which takes
// the first menu string id (10778, node 0) and passes it here. With no panel open (DAT 0x113A68 == 0)
// the window is built, the {sound, string} table at 0xDF398 (gate DAT 0xDF0D8 = 1; first match, a
// zero sound ends it) picks the spoken line, 0x88A70(player, 5, 0, 0) submits it and the handle lands
// in DAT 0x113B0B; the first window of the game (DAT 0x123448) also reads the tick and registers the
// window through D2Gfx 10048, whose edx is that handle.
// The per-frame update 0x7EA90 -> 0x7E970 -> 0x7BB40 and the continuation 0x4B230 (next message 0x45FF0) follow below.
// 0x7CEF0 ends the window: 0x7C9F0 loads edx from DAT 0x11BF9C (the line's sound id) and clears it, then the registered
// window's own 10048 runs with that edx. Not modelled: the scroll text's draw calls.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include "../include/D2Text.h"
#include "../include/D2PacketDef.h"
#include "sound_unit_refs.h"

extern "C" const char* d2_lang_string_by_index(int index);
extern "C" int32_t d2_session_view_y();
uint32_t& d2client_frame_clock();   // frame_tail_g.cpp: the rig clock the frame's reads advance

namespace {
struct Pair { int32_t sound; int16_t string; };
const Pair kSpeech[] = {
#include "npc_speech_sounds.inc"   // 0xDF398, 864 {sound, string} pairs
    , {0, 0}
};
// the scroll struct *0x113A68 (0x7C690): +04 line count, +08 scroll position, +0C rate, +1A last clock, +1E first clock,
// +22 per-repeat step, +26 repeat sum, +2A repeat count
struct Scroll { int32_t count, pos, rate; uint32_t last, first, step, sum; uint16_t reps; };
bool g_panelUp = false;     // DAT 0x113A68 != 0
bool g_registered = false;  // DAT 0x123448
int32_t g_handle = 0;       // DAT 0x113B0B
Scroll g_s{};
D2TextHeaderStrc* g_menu = nullptr;   // DAT 0x113B10, the NPC's menu text list
bool g_talking = false;     // DAT 0x119661
int16_t g_cur = -1;         // DAT 0x11966F
int16_t g_played[9]; uint16_t g_nPlayed = 0;   // 0x119792, count 0x1197A6
std::string g_pendingG;
std::string g_frameG;       // the frame step's 10048s, recorded after 0x739E0's tail
int32_t g_sound = 0;        // DAT 0x11BF9C
std::vector<int32_t> g_playRows;   // the 0x7D3C0 submits for the play path (d2client_npc_speech_take_row); bit 31: opened under the packet dispatch
bool g_tailDriven = false;  // the G frame tail runs the frame step (the draw-list rig); otherwise the session frame does

// 0x7BFB0: line 0 of the string is the scroll rate (atol); every further '\n'-ended run of up to 100 units is a line
void parse(int16_t id, int32_t& count, int32_t& rate) {
    const std::string s = d2_lang_string_by_index((uint16_t)id);
    count = 0; rate = 8;
    size_t i = 0, nl = s.find('\n');
    rate = (int32_t)(int16_t)atol(s.substr(0, nl).c_str());
    if (nl == std::string::npos) return;
    i = nl + 1;
    while (i < s.size()) {
        size_t e = s.find('\n', i), end = e == std::string::npos ? s.size() : e;
        uint32_t units = 0;
        for (size_t k = i; k < end; ++k) units += ((uint8_t)s[k] & 0xC0) != 0x80;
        count += (int32_t)(units / 100) + 1;
        if (e == std::string::npos) break;
        i = e + 1;
    }
}

// 0x7D3C0(npc, id). `tick` 0: under the packet dispatch; else the frame's tick.
void open(int16_t w, uint32_t tick) {
    uint32_t& clk = d2client_frame_clock();
    if (g_panelUp) return;
    g_panelUp = true;
    clk += 3;                                    // 10184, then 0x7C690's two 10184
    parse(w, g_s.count, g_s.rate);
    g_s.pos = 0; g_s.last = g_s.first = g_s.step = g_s.sum = 0; g_s.reps = 0;
    clk += 1;                                    // 10184
    g_handle = 0;
    for (const Pair* p = kSpeech; p->sound != 0; ++p)
        if (p->string == w) {
            g_handle = tick ? d2client_sound_speech_submit_at(tick, p->sound) : d2client_sound_speech_submit(p->sound); g_sound = p->sound;
            if (g_playRows.size() < 16) g_playRows.push_back(p->sound | (tick ? 0 : INT32_MIN));   // the same submit for the play path's mixer
            break;
        }
    if (!g_registered) {
        g_registered = true;
        clk += 1;                                // 7D5E3 GetTickCount
        char b[32];
        std::snprintf(b, sizeof b, "10048 0,%x\n", (unsigned)g_handle);
        (tick ? g_frameG : g_pendingG) += b;
    }
}

// 0x45FF0: the first menu string (10778) that is not `bx` and not yet played
int16_t nextMessage(int16_t bx) {
    if (!g_menu) return -1;
    for (int i = 0;; ++i) {
        const int16_t s = TEXT_GetStringIdOfMenuType0Or2Node(g_menu, i);
        if (s == -1) return -1;
        if (s == bx) continue;
        bool played = false;
        for (uint16_t k = 0; k < g_nPlayed; ++k) if (g_played[k] == s) { played = true; break; }
        if (!played) return s;
    }
}
void push(int16_t s) { if (g_nPlayed < 9) g_played[g_nPlayed++] = s; }
void freeMenu() { if (g_menu) TEXT_FreeTextHeader(g_menu); g_menu = nullptr; }
}

void d2client_npc_speech_reset() {
    g_panelUp = false; g_registered = false; g_pendingG.clear(); g_frameG.clear(); g_sound = 0; g_handle = 0; g_s = {};
    freeMenu(); g_talking = false; g_cur = -1; g_nPlayed = 0; g_playRows.clear(); g_tailDriven = false;
}
bool d2client_npc_speech_take_row(int32_t* row, bool* reply) {
    if (g_playRows.empty()) return false;
    *row = g_playRows.front() & INT32_MAX; *reply = g_playRows.front() < 0; g_playRows.erase(g_playRows.begin());
    return true;
}

std::string d2client_npc_speech_take_g() { std::string s; s.swap(g_pendingG); return s; }
std::string d2client_npc_speech_take_frame_g() { std::string s; s.swap(g_frameG); return s; }

extern "C" void d2_client_npc_speech_packet(const D2GSPacketSrv27* p27, uint32_t guid) {
    if (!p27 || p27->nUnitType != 1 || p27->dwUnitGUID != guid) return;
    D2TextHeaderStrc* list = TEXT_AllocTextHeader(nullptr);
    if (!list) return;
    D2MessageListStrc msgs = p27->pMessageList;
    TEXT_CreateTextHeaderFromMessageList(list, &msgs);
    TEXT_SortTextNodeListByStringId(list);
    const int16_t w = TEXT_GetStringIdOfMenuType0Or2Node(list, 0);
    if (w == -1 || g_panelUp) { TEXT_FreeTextHeader(list); return; }
    // 0x4B8A0: the '/' client packet (0x143E0's clock read, 0x1441B), 966F = w, 0x7D3C0, 0x45D30 (played list), 0x45FF0,
    // 0x14910 (a client packet: 0x143E0's clock read), 9661 = 1
    freeMenu(); g_menu = list; g_nPlayed = 0;
    d2client_frame_clock() += 1;
    g_cur = w;
    open(w, 0);
    push(w);
    d2client_frame_clock() += 1;
    g_talking = true;
}

// 1.13c D2Client+0x7EA90's speech step (0x7E970 on *0x113A68), from the frame after the room walk. `tick` is the frame's tick.
static void speechFrame(uint32_t tick);
void d2client_npc_speech_frame(uint32_t tick) { g_tailDriven = true; speechFrame(tick); }
// The play path's frame (no G frame tail): the same step, timeGetTime read as the frame clock at 40 ms a frame (25 frames a second).
void d2client_npc_speech_play_frame(uint32_t tick) {
    if (g_tailDriven || !g_panelUp) return;
    d2client_frame_clock() += 40;
    speechFrame(tick);
}
static void speechFrame(uint32_t tick) {
    if (!g_panelUp) return;
    uint32_t& clk = d2client_frame_clock();
    // 0x7EA90's SetRect: the view bottom (client Y + H/2 for the 640x480 view) is the edx 10001 is called with
    d2client_sound_log_at(tick, 10001, 0, (uint32_t)(d2_session_view_y() - 16 + 480));
    g_handle = 0;                                // 0x884D0(h) (D2Sound 10054 returns 0), then h = 0
    // 0x7BB40: 10184, timeGetTime, ... 10184
    clk += 1;
    const uint32_t now = ++clk;
    if (g_s.first == 0) { g_s.last = g_s.first = now; }
    else {
        uint32_t extra = 0;
        if (g_s.last == now) {
            if (g_s.step == 0) ++g_s.reps; else extra = g_s.sum += g_s.step;
        } else {
            g_s.sum = 0;
            if (g_s.step == 0 && g_s.reps != 0) g_s.step = (now - g_s.last) / g_s.reps;
            g_s.last = now;
        }
        g_s.pos = (int32_t)((extra + ((now - g_s.first) >> 2)) * (uint32_t)g_s.rate);
    }
    clk += 1;
    const int32_t lines = g_s.count - 1 < 1 ? 1 : g_s.count - 1;
    if (!((uint32_t)(lines * 0x12 + 0x70) < ((uint32_t)g_s.pos >> 10))) return;
    // 0x7E970 on a 1: the struct is freed (0x7BF40), 0x7CEF0 clears 0x123448, then the callback 0x4B230(0)
    g_panelUp = false;
    if (g_registered) {                          // 0x7CEF0: 0x7C9F0 leaves DAT 0x11BF9C in edx for the window's 10048
        g_registered = false;
        char b[32];
        std::snprintf(b, sizeof b, "10048 0,%x\n", (unsigned)g_sound);
        g_frameG += b;
    }
    g_sound = 0;
    const int16_t next = nextMessage(g_cur);
    if (g_talking && next != -1) {
        open(next, tick);
        push(next);
        g_cur = next;
        clk += 1;                                // the '1' client packet (0x143E0)
        return;
    }
    g_talking = false;                           // 0x48150 -> 0x47B20: the close packet (0x143E0) and the menu list freed
    clk += 1;
    freeMenu();
}
