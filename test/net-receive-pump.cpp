// Fixture for the client's receive pump (D2Client+0x43920) against the stream splitter our joined-game client uses.
// The pump's callees are modelled by a host that serves the two D2Net queues the splitter filled.
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "core/realm/d2gs_client_packets.cpp"
#include "core/d2client/d2client_43920.cpp"
#include "core/d2client/server_packet_table.h"

namespace {
D2NetPktNode g_pool[512];
int g_used;
D2NetPktNode* alloc() { return &g_pool[g_used++]; }

D2NetPktQueues g_q;
struct Event { uint32_t rva, first, arg, last; };
std::vector<Event> g_log;

uint32_t pop(D2NetPktNode*& head, uint8_t* buf, uint32_t len) {
    if (!head) return 0xFFFFFFFFu;
    D2NetPktNode* n = head;
    head = n->next;
    memcpy(buf, n->data, n->size < len ? n->size : len);   // D2Net+0x7200 copies min(len, node size)
    return n->size;
}
uint32_t takeSystem(uint8_t* b, uint32_t len) { return pop(g_q.second, b, len); }
uint32_t takeGame(uint8_t* b, uint32_t len) { return pop(g_q.normal, b, len); }
void acd70(const uint8_t* p) { g_log.push_back({ 0xACD70, p[0], 0, 0 }); }
void acbf0(const uint8_t* p) { g_log.push_back({ 0xACBF0, p[0], 0, 0 }); }
void ace20(const uint8_t* p, int32_t n) { g_log.push_back({ 0xACE20, p[0], static_cast<uint32_t>(n), p[0x1ff] }); }
void run(int32_t* flag) { const D2c43920Env e = { takeSystem, takeGame, acd70, acbf0, ace20, flag }; d2c_43920(&e); }
void reset() { g_used = 0; g_q = {}; g_log.clear(); }
void feed(std::initializer_list<uint8_t> bytes) {
    std::vector<uint8_t> v(bytes);
    assert(d2_realm_d2net_6bd0(v.data(), static_cast<int>(v.size()), &g_q, alloc, 1234));
}
}  // namespace

static void sizes() {
    // retail's dispatch loop exits (line 0x144f) when a fixed-size row of its own table disagrees with the D2Net size
    for (uint32_t op = 0; op < d2::client::kServerPacketCount; ++op) {
        const int32_t row = d2::client::kServerPacketTable[op].size;
        if (row == -1) continue;
        uint8_t pkt[0x300] = { static_cast<uint8_t>(op) };
        uint32_t entry = 0;
        const uint32_t n = d2_realm_d2net_6750(pkt, sizeof pkt, &entry);
        if (row == 0) { assert(n == 0); continue; }   // an opcode the server never sends
        // 0x80 is the one row the two tables disagree on: the client's has a null handler and size 4, the D2Net splitter's
        // size is 0, so the packet never leaves the splitter and the dispatcher's size check is never reached for it
        if (op == 0x80) { assert(n == 0 && d2::client::kServerPacketTable[op].handler == 0); continue; }
        if (n != static_cast<uint32_t>(row)) { std::fprintf(stderr, "size mismatch op %02x: client row %d, D2Net %u\n", op, row, n); assert(false); }
    }
}

static void order() {
    reset();
    // arrival order: game, refusal, game, close flag, game, connection info
    feed({ 0x00, 0xB4, 1, 0, 0, 0, 0x02, 0xB0, 0x04, 0xAF, 0x01 });
    int32_t flag = 7;
    run(&flag);
    // the system queue drains first, in arrival order; then the game queue in arrival order; each ends on -1
    assert(g_log.size() == 5);
    assert(g_log[0].rva == 0xACBF0 && g_log[0].first == 0xB4);
    assert(g_log[1].rva == 0xACE20 && g_log[1].first == 0x00 && g_log[1].arg == 1);
    assert(g_log[2].rva == 0xACE20 && g_log[2].first == 0x02 && g_log[2].arg == 1);
    assert(g_log[3].rva == 0xACE20 && g_log[3].first == 0x04 && g_log[3].arg == 1);
    assert(g_log[4].rva == 0xACE20 && g_log[4].arg == 0xFFFFFFFFu);
    assert(flag == 1);   // B0 cleared it, then AF set it: queue order, not arrival order of the stream's other packets
}

static void flags_and_chunks() {
    reset();
    feed({ 0xAF, 0x01, 0xB0, 0xB3, 2, 0, 0, 0, 0, 0, 0 });
    int32_t flag = 0;
    run(&flag);
    assert(flag == 0);   // 0xAF set, 0xB0 cleared
    assert(g_log.size() == 2 && g_log[0].rva == 0xACD70 && g_log[0].first == 0xB3);
}

static void unbounded() {
    reset();
    for (int i = 0; i < 300; ++i) feed({ 0x00 });
    int32_t flag = 0;
    run(&flag);
    // no cap per pass: every queued packet is dispatched, then the terminating -1
    assert(g_log.size() == 301 && g_log.back().arg == 0xFFFFFFFFu);
    for (int i = 0; i < 300; ++i) assert(g_log[i].rva == 0xACE20 && g_log[i].arg == 1);
}

static void oversize() {
    reset();
    // an 0x16 packet declares its own size: 0x203 fits a node (0x204) but not the pump's 0x200-byte buffer
    std::vector<uint8_t> big(0x203, 0x11);
    big[0] = 0x16; big[1] = 0x03; big[2] = 0x02;
    assert(d2_realm_d2net_6bd0(big.data(), static_cast<int>(big.size()), &g_q, alloc, 0));
    int32_t flag = 0;
    run(&flag);
    // retail hands the dispatcher the node's size (0x203) but copied only 0x200 bytes: the tail is the stack's
    assert(g_log.size() == 2 && g_log[0].arg == 0x203);
    assert(g_log[0].last == 0x11);
}

int main() { sizes(); order(); flags_and_chunks(); unbounded(); oversize(); std::puts("ok"); return 0; }
