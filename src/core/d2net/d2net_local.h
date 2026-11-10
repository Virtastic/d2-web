// D2Net's local connection: the one path a single-player game's server packets take to the client.
// The server hands each run to the send (D2Net+0x63E0); a local connection (mode 1 or 2) splits it into
// packets and appends each to one of two unbounded FIFO lists (D2Net+0x6BD0); the client's pump takes
// one packet per call from the system list, then from the game list, until each is empty
// (D2Net+0x71E0 / +0x7200 over +0x6AC0). Nothing but a take removes a packet.
#pragma once
#include <cstdint>

struct D2NetPktNode { uint8_t data[0x204]; uint32_t size, tick; D2NetPktNode* next; };
struct D2NetPktQueues { D2NetPktNode *normal, *second; };   // game list (opcode <= 0xae), system list (0xaf..0xb4)

struct D2NetLocalConn {
    D2NetPktQueues q;
    uint32_t mode;       // 0 no connection, 1 local, 2 local with the 500 ms simulated lag
    uint32_t queued;     // packets appended since the open
    uint32_t taken;      // packets taken since the open
    uint32_t unframed;   // runs the 1.13c size table does not split exactly (retail ends the process there)
    uint32_t compressed; // the pump's flag: set by a system 0xaf, cleared by 0xb0
};
extern D2NetLocalConn g_d2netLocal;

bool d2_realm_d2net_6bd0(const uint8_t* buf, int len, D2NetPktQueues* q, D2NetPktNode* (*alloc)(), uint32_t tick);
uint32_t d2net_local_6ac0(D2NetPktNode** head, uint8_t* dst, uint32_t len);
extern "C" {
uint32_t d2_realm_d2net_6750(const uint8_t* buf, uint32_t avail, uint32_t* out);
uint32_t d2net_local_open(uint32_t mode);
void d2net_local_close();
uint32_t d2net_local_send(const uint8_t* buf, uint32_t len);
uint32_t d2net_local_take(uint8_t* dst, uint32_t len);
uint32_t d2net_local_take_system(uint8_t* dst, uint32_t len);
const uint8_t* d2net_local_drain();
uint32_t d2net_local_drain_length();
}
