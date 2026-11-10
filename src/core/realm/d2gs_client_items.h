#pragma once
// Items the server describes with 0x9C (the local player's and the ground's) and 0x9D (another unit's). The item
// bitstream carries the item's mode and place, so one record per item GUID follows every action: a later packet for
// the same GUID replaces the record, and a unit removal (0x0A, type 4) drops it.
#include <algorithm>
#include <cstdint>
#include <vector>

struct D2RealmClientItemView {
    uint32_t guid, classId, mode, x, y, ownerType, ownerGuid, bodyLoc, page;
};
static_assert(sizeof(D2RealmClientItemView) == 36);

constexpr uint32_t kRealmItemNoOwner = 0xffffffffu;
constexpr uint32_t kRealmItemModeOnGround = 3, kRealmItemModeDropping = 5;
constexpr uint8_t kRealmItemActionAddToShop = 0x0b, kRealmItemActionRemoveFromShop = 0x0c;

// `decoded` holds guid, classId, mode, x, y, bodyLoc and page as the bitstream gave them; the owner fields are filled
// here. opcode 0x9D names the owner in the packet; 0x9C is the receiving player's own item unless it lies on the
// ground or stands in a shop.
inline void d2_realm_client_items_apply(std::vector<D2RealmClientItemView>& items, uint8_t opcode, uint8_t action,
        D2RealmClientItemView decoded, uint32_t packetOwnerType, uint32_t packetOwnerGuid, uint32_t localPlayerGuid) {
    const auto same = [&](const D2RealmClientItemView& item) { return item.guid == decoded.guid; };
    if (action == kRealmItemActionRemoveFromShop) {
        items.erase(std::remove_if(items.begin(), items.end(), same), items.end());
        return;
    }
    if (decoded.mode == kRealmItemModeOnGround || decoded.mode == kRealmItemModeDropping ||
            action == kRealmItemActionAddToShop) {
        decoded.ownerType = kRealmItemNoOwner;
        decoded.ownerGuid = kRealmItemNoOwner;
    } else if (opcode == 0x9d) {
        decoded.ownerType = packetOwnerType;
        decoded.ownerGuid = packetOwnerGuid;
    } else {
        decoded.ownerType = 0;  // UNIT_PLAYER
        decoded.ownerGuid = localPlayerGuid;
    }
    const auto found = std::find_if(items.begin(), items.end(), same);
    if (found != items.end()) *found = decoded;
    else items.push_back(decoded);
}

inline bool d2_realm_client_items_remove(std::vector<D2RealmClientItemView>& items, uint32_t guid) {
    const auto first = std::remove_if(items.begin(), items.end(),
        [guid](const D2RealmClientItemView& item) { return item.guid == guid; });
    const bool removed = first != items.end();
    items.erase(first, items.end());
    return removed;
}

// 0x42: the server destroyed the unit's cursor item (used up, merged into a stack); the record goes with it.
inline bool d2_realm_client_items_clear_cursor(std::vector<D2RealmClientItemView>& items, uint32_t ownerType, uint32_t ownerGuid) {
    const auto first = std::remove_if(items.begin(), items.end(), [=](const D2RealmClientItemView& item) {
        return item.mode == 4 && item.ownerType == ownerType && item.ownerGuid == ownerGuid; });
    const bool removed = first != items.end();
    items.erase(first, items.end());
    return removed;
}

// The packet's fixed part. 0x9C: {op, action, size, category, item guid, bitstream}; 0x9D adds {owner type, owner guid}
// before the bitstream. Returns the bitstream offset, or 0 when the frame is not an item packet of `length` bytes.
inline int d2_realm_client_item_frame(const uint8_t* packet, int length, uint32_t& itemGuid, uint32_t& ownerType,
        uint32_t& ownerGuid) {
    if (!packet || (packet[0] != 0x9c && packet[0] != 0x9d)) return 0;
    const int head = packet[0] == 0x9c ? 8 : 13;
    if (length <= head || packet[2] != length) return 0;
    const auto u32 = [packet](int at) { return static_cast<uint32_t>(packet[at]) | static_cast<uint32_t>(packet[at + 1]) << 8 |
        static_cast<uint32_t>(packet[at + 2]) << 16 | static_cast<uint32_t>(packet[at + 3]) << 24; };
    itemGuid = u32(4);
    ownerType = packet[0] == 0x9d ? packet[8] : kRealmItemNoOwner;
    ownerGuid = packet[0] == 0x9d ? u32(9) : kRealmItemNoOwner;
    return head;
}
