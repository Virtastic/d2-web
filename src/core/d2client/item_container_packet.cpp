// The session client's item hash (D2Client 0x10AE08) as the item packets 0x9C/0x9D/0x0A change it.
// 0x9C action 4, D2Client+0x84750 (into a container), and action 0xE, D2Client+0x83AD0 (into the belt): the item unit is looked
// up in the item hash and made by 0x835C0 when absent. (Their 10048 is recorded by game_net_stubs' seat count.)
// 0x9D action 5, D2Client+0x844F0 (out of a container): an item whose decoded flags carry 0x20 was merged into a stack and its
// unit is taken out of the item hash. 0x0A with unit type 4 frees the unit.
// The frame's unit pass 0xA8110 runs 0xA4DA0 over the item hash with ebx = 0x2B1E0 (D2Client+0xA8161..0xA816B), whose 0x6C130
// reads the clock once per unit, so an item the client holds that is no ground item of the room walk costs the frame clock one
// read per frame.
// Not modelled: the other actions' handlers (ground to cursor, equip, swap, shop).
#include <cstdint>
#include <set>
#include "../d2moo/include/D2Items.h"

namespace {
std::set<uint32_t> g_held;    // item ids in the client's item hash that the room walk does not count
std::set<uint32_t> g_taken;   // items the client freed (0x0A type 4): the walk still steps a taken ground item, so its re-seat is counted there
}

void d2client_item_packet_reset() { g_held.clear(); g_taken.clear(); }
extern "C" uint32_t d2client_item_packet_held() { return uint32_t(g_held.size()); }

extern "C" void d2_client_item_packet(const uint8_t* p, int32_t n) {
    if (!p || n < 6) return;
    if (p[0] == 0x0a && p[1] == 4) { uint32_t id; __builtin_memcpy(&id, p + 2, 4); g_held.erase(id); g_taken.insert(id); return; }
    if (p[0] != 0x9c && p[0] != 0x9d) return;
    const int32_t head = p[0] == 0x9c ? 8 : 13;
    if (n <= head || p[2] <= head) return;
    uint32_t id; __builtin_memcpy(&id, p + 4, 4);
    if (p[0] == 0x9d) {
        if (p[1] != 5) return;
        D2ItemSaveStrc save{};
        ITEMS_GetCompactItemDataFromBitstream(const_cast<uint8_t*>(p) + head, size_t(p[2] - head), 0, &save);
        if (save.dwFlags & 0x20) g_held.erase(id);
        return;
    }
    if ((p[1] == 4 || p[1] == 0xe) && !g_taken.count(id)) g_held.insert(id);
}
