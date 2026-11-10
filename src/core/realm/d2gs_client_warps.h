#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// The level exits a joined game's client knows: the server names each one with packet 0x09
// ([type u8][guid u32][class u8][x u16][y u16], 11 bytes), which creates a client unit of that type.
// An exit is a tile unit (type 5) whose class is its LvlWarp id.  It leaves with the unit-remove packet,
// with its room, and with the act.
struct D2RealmClientWarp {
    uint32_t guid, warpId;
    int32_t x, y;
};
struct D2RealmClientWarps { std::vector<D2RealmClientWarp> list; };

constexpr uint32_t kRealmUnitTile = 5;

// Returns 1 when the packet named a tile unit, 0 for another unit type, -1 for a malformed packet.
inline int d2_realm_client_warps_on_assign(D2RealmClientWarps& warps, const uint8_t* packet, int size) {
    if (!packet || size != 11 || packet[0] != 0x09) return -1;
    if (packet[1] != kRealmUnitTile) return 0;
    D2RealmClientWarp warp{};
    warp.guid = static_cast<uint32_t>(packet[2]) | (static_cast<uint32_t>(packet[3]) << 8) |
        (static_cast<uint32_t>(packet[4]) << 16) | (static_cast<uint32_t>(packet[5]) << 24);
    warp.warpId = packet[6];
    warp.x = packet[7] | (packet[8] << 8);
    warp.y = packet[9] | (packet[10] << 8);
    for (auto& known : warps.list) if (known.guid == warp.guid) { known = warp; return 1; }
    warps.list.push_back(warp);
    return 1;
}

inline int d2_realm_client_warps_remove(D2RealmClientWarps& warps, uint32_t guid) {
    for (size_t i = 0; i < warps.list.size(); ++i) if (warps.list[i].guid == guid) {
        warps.list.erase(warps.list.begin() + static_cast<std::ptrdiff_t>(i));
        return 1;
    }
    return 0;
}

// Drop every exit whose room is gone: `roomAt(x, y)` says whether a room is still active there.
template <typename RoomAt>
inline int d2_realm_client_warps_keep_in_rooms(D2RealmClientWarps& warps, RoomAt&& roomAt) {
    int dropped = 0;
    for (size_t i = 0; i < warps.list.size();) {
        if (roomAt(warps.list[i].x, warps.list[i].y)) ++i;
        else { warps.list.erase(warps.list.begin() + static_cast<std::ptrdiff_t>(i)); ++dropped; }
    }
    return dropped;
}
