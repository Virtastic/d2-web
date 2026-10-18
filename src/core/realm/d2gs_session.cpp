// The D2GS server session -- the game leg, and structurally unlike the BNCS/MCP ones.
//
// Those are short request/reply handshakes with self-describing frames. D2GS is a continuous game
// stream and the two directions are not symmetric:
//   * client -> server is RAW and has no length field at all. A TCP read is not a packet; the
//     server frames it by indexing a size table with the leading opcode, which is what the real
//     D2Net did in SERVER_ValidateClientPacket @0x6FC01FE0 via SERVER_GetClientPacketSize
//     @0x6FC01E60.
//   * server -> client is Huffman-compressed and length-framed (huffman.cpp).
//
// The size table is per BUILD and getting it wrong does not raise an error -- the engine dispatches
// the packet as whatever that number means to it and answers something unrelated, or nothing. Ours
// is 1.13c's, read out of that build's own D2Net (libd2 net.cs_versions.packet_size_113c), NOT
// transcribed from 1.14d: against 1.14d every gameplay opcode 0x00-0x63 matches, but the join and
// handshake range 0x64-0x6F differs, so a neighbour's table desyncs exactly where it matters.
#include <cstdint>
#include <emscripten.h>

extern "C" {
int d2_realm_huffman_compress(const uint8_t* src, int len, uint8_t* dst, int cap);
int d2_realm_huffman_frame(const uint8_t* payload, int plen, uint8_t* out, int outCap);
int d2_realm_d2gs_act_load(uint8_t act, uint32_t mapId, uint16_t areaId, uint32_t unk1, uint8_t* out);
int d2_realm_d2gs_player_assign(uint32_t unitId, uint8_t playerClass, const char* name, uint16_t x, uint16_t y, uint8_t* out);
int d2_realm_d2gs_player_in_game(uint32_t unitId, uint8_t playerClass, const char* name, uint16_t level, uint16_t partyId, uint8_t* out);
}

namespace {

// Total wire size per client->server opcode, including the opcode byte (1.13c).
// 0 = opcode unused by this build, framing cannot continue; -1 = variable-length, must be scanned.
const int16_t kCsSize113c[0x71] = {
     0,   5,   9,   5,   9,   5,   9,   9,   // 0x00-0x07
     5,   9,   9,   1,   5,   9,   9,   5,   // 0x08-0x0F
     9,   9,   1,   9,  -1,  -1,  13,   5,   // 0x10-0x17
    17,   5,   9,   9,   3,   9,   9,  17,   // 0x18-0x1F
    13,   9,   5,   9,   5,   9,  13,   9,   // 0x20-0x27
     9,   9,   9,   0,   0,   1,   3,   9,   // 0x28-0x2F
     9,   9,  17,  17,   5,  17,   9,   5,   // 0x30-0x37
    13,   5,   3,   3,   9,   5,   5,   3,   // 0x38-0x3F
     1,   1,   1,   1,  17,   9,  13,  13,   // 0x40-0x47
     1,   9,   0,   9,   5,   3,   0,   7,   // 0x48-0x4F
     9,   9,   5,   1,   1,   0,   0,   0,   // 0x50-0x57
     3,  17,   0,   0,   0,   7,   6,   5,   // 0x58-0x5F
     1,   3,   5,   5,   0,   0,  -1,  46,   // 0x60-0x67
    37,   1,   1,   1,  -1,  13,   1,   0,   // 0x68-0x6F
     1,                                      // 0x70
};

constexpr int kRxCap = 8192;
// The engine halts (ERROR_UnrecoverableInternalError) on a raw packet above this.
constexpr int kMaxPacket = 0x204;

struct D2gsSession {
    uint8_t rx[kRxCap];
    int rxLen;
    int clientActive;
    uint32_t unitId;
    uint8_t charClass;
    uint8_t act;
    uint16_t level;
    uint32_t gameHash;
    uint16_t gameToken;
    int error;
    int joined;
};
D2gsSession g_g;
#if defined(D2_REALMD_TEST_FIXTURE)
bool g_fixtureReassignOnKeepalive;
bool g_fixtureReassignSent;
bool g_fixtureReassignOnWalk;
bool g_fixtureWalkReassignSent;
bool g_fixtureMonsterOnJoin;
bool g_fixtureRemoveMonsterOnWalk;
bool g_fixtureOtherPlayerOnJoin;
bool g_fixtureSelectOtherOnKeepalive;
bool g_fixtureReturnSelfAndRemoveOtherOnWalk;
#endif

const char kCharName[] = "player";

inline uint16_t rd16(const uint8_t* p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

inline uint32_t rd32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

bool isSelectedCharacterName(const uint8_t* name) {
    for (int i = 0; i < 16; ++i) {
        const uint8_t expected = i < (int)(sizeof(kCharName) - 1)
            ? (uint8_t)kCharName[i]
            : 0;
        if (name[i] != expected) return false;
    }
    return true;
}

}  // namespace

extern "C" {

// The 1.13c client->server size for an opcode: >0 total wire bytes, 0 unused by this build,
// -1 variable-length.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_cs_size(uint8_t opcode) {
    if (opcode == 0xFF) return 16;   // fixed-size control packet the table does not cover
    return opcode < 0x71 ? (int)kCsSize113c[opcode] : 0;
}

// Frame the packet at the head of `buf`: its total wire length, 0 when the opcode cannot be framed
// (unused by this build, or a length past the engine's cap), or -1 when more bytes are needed to
// tell.  Variable-length opcodes carry a u16 after the opcode covering the bytes behind the
// three it sits in, so the total is 3 + that -- the rule libd2 frames live 1.10f and 1.14d clients
// with.  Stated rather than hidden: that is a UNIFORM rule, proven on 1.13c's neighbours rather
// than measured per opcode out of 1.13c's own size function, and -1 is not one rule for every
// opcode on every build (1.06b's 0x14/0x15 walk embedded strings instead). 1.13c's four
// variable slots are 0x14, 0x15, 0x66 and 0x6c; framing one wrong desynchronises the stream for
// good rather than dropping a single packet, so this is the first thing to re-measure against a
// live client.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_cs_packet_len(const uint8_t* buf, int len) {
    if (len < 1) return -1;
    const uint8_t op = buf[0];
    if (op == 0xFF) return len < 16 ? -1 : 16;
    if (op >= 0x71) return 0;
    const int entry = (int)kCsSize113c[op];
    if (entry == 0) return 0;
    if (entry > 0) return len < entry ? -1 : entry;
    if (len < 3) return -1;
    const int n = 3 + (int)((uint32_t)buf[1] | ((uint32_t)buf[2] << 8));
    if (n > kMaxPacket) return 0;
    return len < n ? -1 : n;
}

EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_reset(uint32_t unitId, uint8_t charClass, uint8_t act, uint16_t level,
                                                       uint16_t gameToken, uint32_t gameHash) {
    g_g.rxLen = 0;
    g_g.clientActive = 1;
    g_g.unitId = unitId;
    g_g.charClass = charClass;
    g_g.act = act;
    g_g.level = level;
    g_g.gameToken = gameToken;
    g_g.gameHash = gameHash;
    g_g.error = 0;
    g_g.joined = 0;
#if defined(D2_REALMD_TEST_FIXTURE)
    g_fixtureReassignSent = false;
    g_fixtureWalkReassignSent = false;
#endif
}

// 0x6090 (ordinal 10000) retires the indexed server client. The realmd host owns the
// socket; this core half clears the matching D2GS session before the host closes it. realmd
// currently supports one D2GS connection, assigned client slot zero.
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_disconnect_client(uint32_t clientId) {
    if (clientId != 0 || !g_g.clientActive) return;
    g_g.clientActive = 0;
    g_g.rxLen = 0;
    g_g.joined = 0;
}

#if defined(D2_REALMD_TEST_FIXTURE)
// Local integration-fixture switch. The stock server path leaves it disabled; s2901 enables it
// to drive one native-format server update through the browser's actual receive loop.
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_reassign_on_keepalive(int enabled) {
    g_fixtureReassignOnKeepalive = enabled != 0;
    g_fixtureReassignSent = false;
}
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_reassign_on_walk(int enabled) {
    g_fixtureReassignOnWalk = enabled != 0;
    g_fixtureWalkReassignSent = false;
}
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_monster_on_join(int enabled) {
    g_fixtureMonsterOnJoin = enabled != 0;
}
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_remove_monster_on_walk(int enabled) {
    g_fixtureRemoveMonsterOnWalk = enabled != 0;
}
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_other_player_on_join(int enabled) {
    g_fixtureOtherPlayerOnJoin = enabled != 0;
}
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_select_other_on_keepalive(int enabled) {
    g_fixtureSelectOtherOnKeepalive = enabled != 0;
}
EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_fixture_return_self_remove_other_on_walk(int enabled) {
    g_fixtureReturnSelfAndRemoveOtherOnWalk = enabled != 0;
}
#endif

EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_session_pending() { return g_g.rxLen; }
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_session_error() { return g_g.error; }
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_session_joined() { return g_g.joined; }

// The greeting the engine sends unprompted the moment a client connects, before any frame:
// AF 01 selects the stock Huffman table and length framing for the server->client stream.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_session_greeting(uint8_t* out) {
    out[0] = 0xAF;
    out[1] = 0x01;
    return 2;
}

// Feed raw client->server bytes.  Returns reply bytes (Huffman-compressed and framed) written to
// `out`, or -1 when the stream cannot be framed -- an opcode this build does not use, or a declared
// length past the engine's cap.  That is reported rather than skipped: silently guessing a length
// is how a stream desyncs with nothing in the log to explain it.  A packet that is merely
// incomplete is not an error; it stays buffered for the next read.
EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_session_feed(const uint8_t* in, int len, uint8_t* out, int outCap) {
    if (g_g.error) return -1;
    if (g_g.rxLen + len > kRxCap) { g_g.error = 1; return -1; }
    for (int i = 0; i < len; i++) g_g.rx[g_g.rxLen++] = in[i];

    int written = 0;
    int off = 0;
    uint8_t burst[512];
    uint8_t packed[1200];
    while (true) {
        if (g_g.rxLen - off < 1) break;
        const uint8_t op = g_g.rx[off];
        const int size = d2_realm_d2gs_cs_packet_len(g_g.rx + off, g_g.rxLen - off);
        if (size == 0) { g_g.error = 2; return -1; }    // unframeable opcode: cannot resynchronise
        if (size < 0) break;                            // need more bytes to tell the length

        if (op == 0x68) {
            // GAMELOGON is the admission handshake for this D2GS connection, not a
            // repeatable enter-game command. Once this session has emitted its join
            // burst, a second login on the same stream cannot allocate another seat.
            // Treat it as a protocol error so we never duplicate the player's roster.
            if (g_g.joined) {
                g_g.error = 4;
                return -1;
            }
            // GAMELOGON carries the exact ticket returned by MCP JOINGAME: hash at +1 and token
            // at +5, both little-endian. Bind it to the selected character too; a ticket for a
            // different name must not receive this session's player assignment.
            const uint8_t* packet = g_g.rx + off;
            if (rd32(packet + 1) != g_g.gameHash || rd16(packet + 5) != g_g.gameToken ||
                !isSelectedCharacterName(packet + 21)) {
                g_g.error = 3;
                return -1;
            }
            // GAMELOGON: local deterministic fixture. Seed 1 / Normal / Act I's first level has
            // a first client room at (1168,1176); emit MapReveal before PlayerAssign to exercise
            // the native null-current-room branch, then LoadComplete after the player exists.
            int n = 0;
            n += d2_realm_d2gs_act_load(g_g.act, 1, 1, 0, burst + n);
#if defined(D2_REALMD_TEST_FIXTURE)
            burst[n++] = 0x07;
            burst[n++] = 0x90; burst[n++] = 0x04; // x=1168, little endian
            burst[n++] = 0x98; burst[n++] = 0x04; // y=1176, little endian
            burst[n++] = 1;                       // eLevel=Act I town
            // D2GS player coordinates are subtiles; MapReveal's room anchor is in tiles.
            n += d2_realm_d2gs_player_assign(g_g.unitId, g_g.charClass, kCharName, 1168 * 5, 1176 * 5, burst + n);
#else
            n += d2_realm_d2gs_player_assign(g_g.unitId, g_g.charClass, kCharName, 0, 0, burst + n);
#endif
            // The joining client's native order is self 0x59 construction, then 0x0B selection.
            burst[n++] = 0x0b;
            burst[n++] = 0; // player unit type
            burst[n++] = (uint8_t)g_g.unitId;
            burst[n++] = (uint8_t)(g_g.unitId >> 8);
            burst[n++] = (uint8_t)(g_g.unitId >> 16);
            burst[n++] = (uint8_t)(g_g.unitId >> 24);
            n += d2_realm_d2gs_player_in_game(g_g.unitId, g_g.charClass, kCharName, g_g.level, 0xFFFF, burst + n);
#if defined(D2_REALMD_TEST_FIXTURE)
            if (g_fixtureOtherPlayerOnJoin) {
                const uint16_t otherX = 1168 * 5 + 4, otherY = 1176 * 5 + 4;
                n += d2_realm_d2gs_player_assign(g_g.unitId + 1, 1, "other", otherX, otherY, burst + n);
            }
            burst[n++] = 0x04; // LoadComplete: the client handler requires an assigned player.
            if (g_fixtureMonsterOnJoin) {
                // 16 zero component-choice fields follow the component-present bit. The body is
                // padded to cover all 16 table-derived field widths; choice zero selects the
                // retail MonStats2 variant for each available component.
                const uint8_t monster[29] = { 0xac, 0x01, 0xac, 0, 0, 0, 0,
                    0xde, 0x16, 0x02, 0x17, 128, 29, 0x11 };
                for (uint8_t byte : monster) burst[n++] = byte;
                // A real server walk update for that GUID. The client must install and tick its
                // DynamicPath before the ordinary renderer snapshot reaches these destination
                // subtiles. Production builds never synthesize this fixture packet.
                const uint8_t walk[16] = { 0x67, 0x01, 0xac, 0, 0, 1,
                    0xe6, 0x16, 0x02, 0x17, 0, 0, 0, 0, 0, 0 };
                for (uint8_t byte : walk) burst[n++] = byte;
            }
#endif
            const int pn = d2_realm_huffman_compress(burst, n, packed, (int)sizeof packed);
            if (pn < 0) { g_g.error = 1; return -1; }
            if (written + pn + 2 > outCap) { g_g.error = 1; return -1; }
            const int fn = d2_realm_huffman_frame(packed, pn, out + written, outCap - written);
            if (fn < 0) { g_g.error = 1; return -1; }
            written += fn;
            g_g.joined = 1;
#if defined(D2_REALMD_TEST_FIXTURE)
        } else if (op == 0x6d && g_g.joined && g_fixtureReassignOnKeepalive && !g_fixtureReassignSent) {
            // Deterministic local fixture: one server location reassignment after the client's
            // first keep-alive. This exercises the real WebSocket receive path without a test hook
            // in the browser or any external realm traffic.
            const uint16_t playerX = 1170 * 5, playerY = 1178 * 5;
            uint8_t response[17] = { 0x15, 0, (uint8_t)g_g.unitId, (uint8_t)(g_g.unitId >> 8),
                (uint8_t)(g_g.unitId >> 16), (uint8_t)(g_g.unitId >> 24), (uint8_t)playerX, (uint8_t)(playerX >> 8),
                (uint8_t)playerY, (uint8_t)(playerY >> 8), 1 };
            int responseLength = 11;
            if (g_fixtureSelectOtherOnKeepalive) {
                response[11] = 0x0b; response[12] = 0;
                response[13] = (uint8_t)(g_g.unitId + 1); response[14] = (uint8_t)((g_g.unitId + 1) >> 8);
                response[15] = (uint8_t)((g_g.unitId + 1) >> 16); response[16] = (uint8_t)((g_g.unitId + 1) >> 24);
                responseLength = 17;
            }
            const int pn = d2_realm_huffman_compress(response, responseLength, packed, (int)sizeof packed);
            if (pn < 0 || written + pn + 2 > outCap) { g_g.error = 1; return -1; }
            const int fn = d2_realm_huffman_frame(packed, pn, out + written, outCap - written);
            if (fn < 0) { g_g.error = 1; return -1; }
            written += fn;
            g_fixtureReassignSent = true;
        } else if (op == 0x01 && g_g.joined && g_fixtureReassignOnWalk && !g_fixtureWalkReassignSent) {
            // Test-only local realm echo: the browser's walk request reaches the real socket parser,
            // and the server answers with an authoritative location update. Production builds do
            // not synthesize this response.
            const uint16_t playerX = (uint16_t)(g_g.rx[off + 1] | ((uint16_t)g_g.rx[off + 2] << 8));
            const uint16_t playerY = (uint16_t)(g_g.rx[off + 3] | ((uint16_t)g_g.rx[off + 4] << 8));
            uint8_t response[34] = {};
            int responseLength = 0;
            if (g_fixtureReturnSelfAndRemoveOtherOnWalk) {
                response[responseLength++] = 0x0b; response[responseLength++] = 0;
                response[responseLength++] = (uint8_t)g_g.unitId;
                response[responseLength++] = (uint8_t)(g_g.unitId >> 8);
                response[responseLength++] = (uint8_t)(g_g.unitId >> 16);
                response[responseLength++] = (uint8_t)(g_g.unitId >> 24);
            }
            const uint8_t reassign[11] = { 0x15, 0, (uint8_t)g_g.unitId, (uint8_t)(g_g.unitId >> 8),
                (uint8_t)(g_g.unitId >> 16), (uint8_t)(g_g.unitId >> 24), (uint8_t)playerX, (uint8_t)(playerX >> 8),
                (uint8_t)playerY, (uint8_t)(playerY >> 8), 0x5a };
            for (uint8_t byte : reassign) response[responseLength++] = byte;
            if (g_fixtureReturnSelfAndRemoveOtherOnWalk) {
                response[responseLength++] = 0x0a; response[responseLength++] = 0;
                response[responseLength++] = (uint8_t)(g_g.unitId + 1); response[responseLength++] = (uint8_t)((g_g.unitId + 1) >> 8);
                response[responseLength++] = (uint8_t)((g_g.unitId + 1) >> 16); response[responseLength++] = (uint8_t)((g_g.unitId + 1) >> 24);
            }
            const int pn = d2_realm_huffman_compress(response, responseLength, packed, (int)sizeof packed);
            if (pn < 0 || written + pn + 2 > outCap) { g_g.error = 1; return -1; }
            const int fn = d2_realm_huffman_frame(packed, pn, out + written, outCap - written);
            if (fn < 0) { g_g.error = 1; return -1; }
            written += fn;
            g_fixtureWalkReassignSent = true;
            if (g_fixtureRemoveMonsterOnWalk) {
                const uint8_t remove[6] = { 0x0a, 1, 0x01, 0xac, 0, 0 };
                const int rn = d2_realm_huffman_compress(remove, (int)sizeof remove, packed, (int)sizeof packed);
                if (rn < 0 || written + rn + 2 > outCap) { g_g.error = 1; return -1; }
                const int rfn = d2_realm_huffman_frame(packed, rn, out + written, outCap - written);
                if (rfn < 0) { g_g.error = 1; return -1; }
                written += rfn;
            }
#endif
        }
        // In the normal server build 0x6d keep-alive and gameplay commands need no reply here.
        off += size;
    }

    const int rest = g_g.rxLen - off;
    for (int i = 0; i < rest; i++) g_g.rx[i] = g_g.rx[off + i];
    g_g.rxLen = rest;
    return written;
}

}  // extern "C"
