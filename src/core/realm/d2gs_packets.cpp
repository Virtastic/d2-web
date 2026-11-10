// D2GS server->client packet composition -- the schema layer above the Huffman transport
// (src/core/realm/huffman.cpp). Our realm is the server, so it composes these packets; the layouts
// are the verbatim wire schemas from the community packet defs (diablo2-protocol / blacha-diablo2,
// MIT), which OBJECTIVE names as the schema source ("nothing to RE on the wire"). This slice is the
// game-join sequence a joining client receives -- load the act, assign its player unit, announce it
// in the game. Fields are little-endian; strings are fixed-width, null-padded. Proved byte-for-byte
// against blacha's packet fixtures in s110. 
#include <cstdint>
#include <emscripten.h>

namespace {
inline void wu16(uint8_t* p, uint16_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
inline void wu32(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24); }
inline void wstr(uint8_t* p, const char* s, int n) {
    int i = 0;
    for (; s && s[i] && i < n; i++) p[i] = (uint8_t)s[i];
    for (; i < n; i++) p[i] = 0;
}
}  // namespace

extern "C" {

// Client-to-server location commands use the same wire shape for walk and run: opcode, then
// subtile x/y as little-endian u16s. The caller chooses 0x01 or 0x03 from the client run toggle.
// Prior art: libd2 `packages/net/src/cs.zig` (CoordCmd/RunToLocation) and d2-clientless
// `src/game/bot.zig` both pin the five-byte shape; the local target build validates it through
// D2GAME_PACKETCALLBACK_Rcv0x01_Walk / Rcv0x03_Run in `D2Game/PLAYER/PlrMsg.cpp`.
// `d2_session_command_move` uses this composer before dispatching the same bytes into the local
// retail D2Game handler, keeping the local command boundary byte-identical to the future socket path.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_move_to(uint8_t opcode, uint16_t x, uint16_t y, uint8_t* out) {
    if (!out || (opcode != 0x01 && opcode != 0x03)) return 0;
    out[0] = opcode;
    wu16(out + 1, x);
    wu16(out + 3, y);
    return 5;
}

// Right-hand ground skills have the same five-byte coordinate schema. The initial
// click is 0x0C and a held repeat is 0x0F; native callbacks.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_right_skill_at(uint8_t opcode, uint16_t x, uint16_t y, uint8_t* out) {
    if (!out || (opcode != 0x0C && opcode != 0x0F)) return 0;
    out[0] = opcode;
    wu16(out + 1, x);
    wu16(out + 3, y);
    return 5;
}

// Right-hand skill on a server-owned unit uses the entity command schema, not a coordinate
// packet: opcode, unit type:u32, unit GUID:u32. 0x0D is the initial press and 0x10 is its
// held-repeat callback; the D2Game 1.13c handlers are recorded
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_right_skill_on_entity(uint8_t opcode, uint32_t unitType,
        uint32_t guid, uint8_t* out) {
    if (!out || (opcode != 0x0D && opcode != 0x10) || unitType > 5) return 0;
    out[0] = opcode;
    wu32(out + 1, unitType);
    wu32(out + 5, guid);
    return 9;
}

// Native 0x3C chooses the hand before a later coordinate cast (which carries no skill ID).
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_select_skill(uint16_t skill, uint8_t left, uint32_t owner, uint8_t* out) {
    if (!out || left > 1) return 0;
    out[0] = 0x3c;
    wu32(out + 1, uint32_t(skill) | (left ? 0x80000000u : 0));
    wu32(out + 5, owner);
    return 9;
}

// 0x03 GameActLoad: act(u8), mapId(u32), areaId(u16), unk1(u32).  12 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_act_load(uint8_t act, uint32_t mapId, uint16_t areaId, uint32_t unk1, uint8_t* out) {
    out[0] = 0x03;
    out[1] = act;
    wu32(out + 2, mapId);
    wu16(out + 6, areaId);
    wu32(out + 8, unk1);
    return 12;
}

// 0x59 PlayerAssign: unitId(u32), class(u8), name(16), x(u16), y(u16).  26 bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_player_assign(uint32_t unitId, uint8_t playerClass, const char* name, uint16_t x, uint16_t y, uint8_t* out) {
    out[0] = 0x59;
    wu32(out + 1, unitId);
    out[5] = playerClass;
    wstr(out + 6, name, 16);
    wu16(out + 22, x);
    wu16(out + 24, y);
    return 26;
}

// 0x5b PlayerInGame: packetLength(u16), unitId(u32), class(u8), name(16), level(u16), partyId(u16),
// unk1(8).  36 bytes; packetLength counts the whole packet including the opcode.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_player_in_game(uint32_t unitId, uint8_t playerClass, const char* name, uint16_t level, uint16_t partyId, uint8_t* out) {
    out[0] = 0x5B;
    wu16(out + 1, 36);
    wu32(out + 3, unitId);
    out[7] = playerClass;
    wstr(out + 8, name, 16);
    wu16(out + 24, level);
    wu16(out + 26, partyId);
    for (int i = 0; i < 8; i++) out[28 + i] = 0;
    return 36;
}

// ── The session layer: entering a game and staying in it ───────────────────────────────────
// These are client->server and are NOT in the server's command dispatch table -- our realm reads
// them at the session layer. Composing them here also gives us the reference-client side, which is
// what drives our own server in testing.

// GAMELOGON (0x68), 37 bytes -- D2GSPacketClt0x68, packed and raw (d2-clientless session.zig).
// The hash and token identify the game the realm assigned (they come straight out of the MCP
// JOINGAME reply); the two constants are the expansion/version pair the engine checks before it
// will admit anyone. 1.13c D2Client+0x14A50 builds +12 as 0x2185EDD6 + (expansion ? 0xCBD7DE7A : 0),
// i.e. 0xED5DCC50 for LoD, and takes `verByte` from the BNClient interface (+0x5C returns 0x0D). The
// character name is a 16-byte null-padded field, truncated if longer.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_gamelogon(uint32_t gameHash, uint16_t gameToken, uint8_t charClass, uint32_t verByte, const char* character, uint8_t* out) {
    for (int i = 0; i < 37; i++) out[i] = 0;
    out[0] = 0x68;
    wu32(out + 1, gameHash);
    wu16(out + 5, gameToken);
    out[7] = charClass;
    wu32(out + 8, verByte);
    wu32(out + 12, 0xED5DCC50u);   // nVersionConstant (expansion)
    wu32(out + 16, 0x91A519B6u);   // nConstant
    out[20] = 0;                   // nLanguageCode
    wstr(out + 21, character, 16);
    return 37;
}

// SCMD 0x6d, the client keep-alive: exactly 13 bytes, [0x6D][tick][average RTT >> 1][check * 0xB640DE41].
// The length has to be exact.  The server frames client->server traffic by the same size table the
// client does, so a SHORT ping is not rejected: it is sized at 13 anyway and eats the eight bytes
// after it, so every later command is read at an offset and the game goes quiet rather than
// erroring.  The engine also hangs up silently on a connection that stops pinging.
//
// `st` is D2Client's keep-alive state, one dword each: [0] DAT_9800 the send tick, [1] DAT_9804 the last
// round trip, [2] DAT_982C the sample count, [3] DAT_9830 the average round trip, [4..11] DAT_9834..9850 the
// last pong's eight dwords, [12] DAT_CC4F4 the last ping tick, [13] DAT_CC4E0 bytes sent, [14] DAT_CC4E8 pings sent.
// All zero at load.
//
// 1.13c D2Client+0x14B00. The check word comes from the pong: (DAT_9848, DAT_984C) * 0x470E31C1 is an
// eight-byte module name and DAT_9850 * 0x470E31C1 an offset into it; an empty name gives 0, a module
// GetModuleHandleA does not find gives the uninitialised 0xCCCCCCCC, and a found one gives the words at
// +offset (w20) and +offset+0x20 (w24) as w24 << 16 | w20. The host answers the lookup through `found`,
// `w20` and `w24` (d2_realm_d2gs_ping_module names it). Sent only when tick - DAT_CC4F4 > 4999; the
// callers (D2Client 0x44B50, 0xC1760) then store the send tick in DAT_9800. Returns 13, or 0 when gated.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_ping(uint32_t* st, uint32_t tick, int found, uint32_t w20, uint32_t w24, uint8_t* out) {
    const uint32_t name = st[9] * 0x470E31C1u;
    uint32_t check = 0;
    if ((uint8_t)name != 0) check = found ? ((w24 & 0xFFFFu) << 16) | (w20 & 0xFFFFu) : 0xCCCCCCCCu;
    if (tick - st[12] <= 4999) return 0;
    st[12] = tick;
    out[0] = 0x6D;
    wu32(out + 1, tick);
    wu32(out + 5, st[3] >> 1);
    wu32(out + 9, check * 0xB640DE41u);
    st[13] += 13;
    st[14] += 1;
    st[0] = tick;
    return 13;
}

// The module the next ping reads: writes the NUL-terminated eight-byte name to `name` (9 bytes) and returns
// the offset, or returns 0 with an empty name when the ping's check word is 0 without a lookup.
EMSCRIPTEN_KEEPALIVE uint32_t d2_realm_d2gs_ping_module(const uint32_t* st, char* name) {
    const uint32_t a = st[9] * 0x470E31C1u, b = st[10] * 0x470E31C1u;
    for (int i = 0; i < 4; ++i) { name[i] = (char)(a >> (8 * i)); name[4 + i] = (char)(b >> (8 * i)); }
    name[8] = 0;
    return name[0] ? st[11] * 0x470E31C1u : 0;
}

// SCMD 0x8F, the pong (33 bytes). 1.13c D2Client+0x422E0 (EAX = the packet): the eight dwords at +1 go to
// DAT_9834..9850, DAT_9844 is then overwritten with GetTickCount, DAT_9804 = tick - DAT_9800, and while
// fewer than ten samples are held the average becomes (average * count + rtt) / (count + 1).
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_pong(uint32_t* st, const uint8_t* pkt, int len, uint32_t tick) {
    if (len < 33 || pkt[0] != 0x8F) return 0;
    for (int i = 0; i < 8; ++i)
        st[4 + i] = (uint32_t)pkt[1 + 4 * i] | ((uint32_t)pkt[2 + 4 * i] << 8) | ((uint32_t)pkt[3 + 4 * i] << 16) | ((uint32_t)pkt[4 + 4 * i] << 24);
    st[8] = tick;
    st[1] = tick - st[0];
    if (st[2] < 10) {
        st[3] = (st[3] * st[2] + st[1]) / (st[2] + 1);
        st[2] += 1;
    }
    return 1;
}

// 0xa8 StateSet: unitType(u8), unitId(u32), packetLength(u8), state(u8), stateEffects(bytes).  The
// packetLength counts the whole packet, so the effects fill packetLength - 8 trailing bytes.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_state_set(uint8_t unitType, uint32_t unitId, uint8_t state, const uint8_t* effects, int nEffects, uint8_t* out) {
    const int total = 8 + nEffects;
    out[0] = 0xA8;
    out[1] = unitType;
    wu32(out + 2, unitId);
    out[6] = (uint8_t)total;
    out[7] = state;
    for (int i = 0; i < nEffects; i++) out[8 + i] = effects[i];
    return total;
}

}  // extern "C"
