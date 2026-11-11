// Host test (tools/realm-tests.sh): every unit packet the client state routes to a movement or mode handler
// is framed with the size the 1.13c server packet table gives its opcode. A route that names another size
// refuses a packet the splitter framed correctly, and a refused packet drops the whole joined-game state.
// The monster's move-to-unit packet is 21 bytes: its handler reads its last byte at index 20.
#include "d2gs_client_packets.cpp"
#include "d2gs_client_pkt_route.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>

int main() {
    int routed = 0;
    for (int op = 0; op <= 0xb4; ++op) {
        const d2::realm::PktRoute route = d2::realm::pktRoute(static_cast<uint8_t>(op));
        if (route.handler == d2::realm::PktHandler::None) continue;
        uint8_t packet[0x40] = {};
        packet[0] = static_cast<uint8_t>(op);
        uint32_t entry = 0;
        const uint32_t size = d2_realm_d2net_6750(packet, sizeof packet, &entry);
        if (size != route.length) std::printf("opcode 0x%02x: table size %u, route length %u\n", op, size, route.length);
        assert(size == route.length);
        assert(route.guidOffset + 4u <= route.length);
        ++routed;
    }
    assert(routed == 10);
    assert(d2::realm::pktRoute(0x68).length == 21);
    // The monster move codes: walk or run to a point (1, 0x17) and to a unit (0, 0x18) all move the monster.
    using d2::realm::MonsterAction;
    for (uint32_t code : {0u, 1u, 0x17u, 0x18u}) assert(d2::realm::monsterAction(code) == MonsterAction::Move);
    assert(d2::realm::monsterAction(6) == MonsterAction::GetHit);
    return 0;
}
