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
    uint32_t unitId;
    uint8_t charClass;
    uint8_t act;
    uint16_t level;
    int error;
    int joined;
};
D2gsSession g_g;

const char kCharName[] = "player";

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

EMSCRIPTEN_KEEPALIVE void d2_realm_d2gs_session_reset(uint32_t unitId, uint8_t charClass, uint8_t act, uint16_t level) {
    g_g.rxLen = 0;
    g_g.unitId = unitId;
    g_g.charClass = charClass;
    g_g.act = act;
    g_g.level = level;
    g_g.error = 0;
    g_g.joined = 0;
}

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
            // GAMELOGON: answer with the game-entry burst -- load the act, assign the player unit,
            // announce it -- composed by d2gs_packets.cpp, then compressed and framed as one frame.
            int n = 0;
            n += d2_realm_d2gs_act_load(g_g.act, 1, 1, 0, burst + n);
            n += d2_realm_d2gs_player_assign(g_g.unitId, g_g.charClass, kCharName, 0, 0, burst + n);
            n += d2_realm_d2gs_player_in_game(g_g.unitId, g_g.charClass, kCharName, g_g.level, 0xFFFF, burst + n);
            const int pn = d2_realm_huffman_compress(burst, n, packed, (int)sizeof packed);
            if (pn < 0) { g_g.error = 1; return -1; }
            if (written + pn + 2 > outCap) { g_g.error = 1; return -1; }
            const int fn = d2_realm_huffman_frame(packed, pn, out + written, outCap - written);
            if (fn < 0) { g_g.error = 1; return -1; }
            written += fn;
            g_g.joined = 1;
        }
        // 0x6d keep-alive and gameplay commands need no reply here; the engine only notes liveness.
        off += size;
    }

    const int rest = g_g.rxLen - off;
    for (int i = 0; i < rest; i++) g_g.rx[i] = g_g.rx[off + i];
    g_g.rxLen = rest;
    return written;
}

}  // extern "C"
