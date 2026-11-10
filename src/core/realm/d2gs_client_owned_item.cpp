#include "d2gs_client_owned_item.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include <D2Inventory.h>
#include <D2Items.h>
#include <D2StatList.h>
#include <D2DataTbls.h>
#include <Units/Item.h>
#include <Units/Units.h>

namespace {
constexpr uint32_t kRetailItemFormatVersion = 96;

void release_item(D2UnitStrc* item) {
    if (!item || item->dwUnitType != UNIT_ITEM) return;
    STATLIST_FreeStatListEx(item);
    if (item->pItemData) ITEMS_FreeItemData(nullptr, item);
    if (item->pStaticPath) UNITS_FreeStaticPath(item);
    UNITS_FreeUnit(item);
}

D2UnitStrc* decode_item(const D2RealmOwnedItemPacket& packet,
        const uint8_t* bytes, int length, uint8_t& bodyLoc) {
    if (!bytes || length <= 0) return nullptr;
    auto* item = UNITS_AllocUnit(nullptr, UNIT_ITEM);
    if (!item) return nullptr;
    item->dwUnitId = packet.itemGuid;

    ITEMS_AllocItemData(nullptr, item);
    if (!item->pItemData) {
        release_item(item);
        return nullptr;
    }
    ITEMS_InitItemSeed(item);
    UNITS_AllocStaticPath(item);
    if (!item->pStaticPath) {
        release_item(item);
        return nullptr;
    }
    // The server item decoder writes its compact/static coordinates while decoding. A detached
    // path with a null room is the client-side unit's initial state before inventory insertion.
    UNITS_InitializeStaticPath(item, nullptr, 0, 0);

    int socketedItems = 0;
    BOOL failed = FALSE;
    const size_t consumed = ITEMS_DecodeItemFromBitstream(item,
        const_cast<uint8_t*>(bytes), static_cast<size_t>(length), FALSE,
        &socketedItems, kRetailItemFormatVersion, &failed);
    if (failed || consumed != static_cast<size_t>(length) || !item->pItemData ||
        item->dwAnimMode != IMODE_EQUIP || item->pItemData->nBodyLoc <= BODYLOC_NONE ||
        item->pItemData->nBodyLoc >= NUM_BODYLOC) {
        release_item(item);
        return nullptr;
    }

    bodyLoc = item->pItemData->nBodyLoc;
    return item;
}

bool decode_existing_item(D2UnitStrc* item, const uint8_t* bytes, int length,
        uint8_t& bodyLoc) {
    if (!item || item->dwUnitType != UNIT_ITEM || !item->pItemData ||
        !item->pStaticPath || !bytes || length <= 0) return false;

    // Action 6 can repeat a GUID already attached to a player's inventory. Preserve that
    // UNIT_ITEM allocation, but reset decoder-owned item state as for a newly decoded unit.
    // The caller has detached it from its prior inventory, so the intrusive links are empty.
    const D2UnitGUID guid = item->dwUnitId;
    STATLIST_FreeStatListEx(item);
    D2ItemExtraDataStrc extra = item->pItemData->pExtraData;
    std::memset(item->pItemData, 0, sizeof(*item->pItemData));
    item->pItemData->pExtraData = extra;
    item->pItemData->dwOwnerGUID = D2UnitInvalidGUID;
    item->dwUnitId = guid;
    ITEMS_InitItemSeed(item);
    UNITS_InitializeStaticPath(item, nullptr, 0, 0);

    int socketedItems = 0;
    BOOL failed = FALSE;
    const size_t consumed = ITEMS_DecodeItemFromBitstream(item,
        const_cast<uint8_t*>(bytes), static_cast<size_t>(length), FALSE,
        &socketedItems, kRetailItemFormatVersion, &failed);
    if (failed || consumed != static_cast<size_t>(length) || !item->pItemData ||
        item->dwAnimMode != IMODE_EQUIP || item->pItemData->nBodyLoc <= BODYLOC_NONE ||
        item->pItemData->nBodyLoc >= NUM_BODYLOC || item->dwUnitId != guid) return false;

    bodyLoc = item->pItemData->nBodyLoc;
    return true;
}

// The client applies a worn item to its owner the way the server does: a set item updates the set
// state, then the item's stat list joins the owner's unless the item is broken. A dual-wielding
// class merges its second weapon as the off-hand list. Switch-slot items contribute nothing.
void apply_item_stats(D2UnitStrc* owner, D2UnitStrc* item) {
    if (ITEMS_GetItemQuality(item) == ITEMQUAL_SET) ITEMS_UpdateSets(owner, item, 0, 0);
    if (ITEMS_CheckItemFlag(item, IFLAG_BROKEN, __LINE__, __FILE__)) return;
    BOOL mainWeapon = TRUE;
    if (UNITS_CanDualWield(owner)) {
        D2UnitStrc* left = INVENTORY_GetLeftHandWeapon(owner->pInventory);
        if (left && left != item && ITEMS_CheckItemTypeId(item, ITEMTYPE_WEAPON)) mainWeapon = FALSE;
    }
    STATLIST_MergeStatLists(owner, item, mainWeapon);
}

void remove_item_stats(D2UnitStrc* owner, D2UnitStrc* item) {
    if (ITEMS_GetItemQuality(item) == ITEMQUAL_SET) ITEMS_UpdateSets(owner, item, 1, 1);
    STATLIST_ExpireUnitStatlist(owner, item);
}

bool item_applied(D2UnitStrc* item) {
    return STATLIST_GetOwner(item, nullptr) != nullptr;
}

void drop_from_owner(D2UnitStrc* owner, D2UnitStrc* item) {
    if (item_applied(item)) remove_item_stats(owner, item);
    INVENTORY_RemoveItem(item);
    release_item(item);
}

bool player_owner(const D2UnitStrc* owner) {
    return owner && owner->dwUnitType == UNIT_PLAYER && owner->pInventory &&
        owner->pInventory->dwSignature == D2C_InventoryHeader &&
        owner->pInventory->pOwner == owner && owner->pInventory->dwOwnerGuid == owner->dwUnitId;
}
}

extern "C" {

void d2_realm_client_owned_item_destroy(D2UnitStrc* item) {
    release_item(item);
}

// The worn-item sweep the client runs after every item or base-stat change: an item whose strength,
// dexterity or level requirement the owner no longer meets is marked unusable and stops
// contributing; one whose requirement is met again contributes again. One item's bonus can satisfy
// another's requirement, so the sweep repeats until nothing changes (bounded like the client's).
EMSCRIPTEN_KEEPALIVE int d2_realm_client_owned_item_reconcile(D2UnitStrc* owner) {
    if (!player_owner(owner)) return 0;
    int changes = 0;
    for (int sweep = 0; sweep < 101; ++sweep) {
        bool changed = false;
        for (int loc = BODYLOC_NONE + 1; loc < BODYLOC_SWRARM; ++loc) {
            D2UnitStrc* item = INVENTORY_GetItemFromBodyLoc(owner->pInventory, loc);
            if (!item || item->dwUnitType != UNIT_ITEM || !item->pItemData || !item->pStatListEx) continue;
            const bool applied = item_applied(item);
            const bool usable = ITEMS_CheckRequirements(item, owner, FALSE, nullptr, nullptr, nullptr) != 0;
            if (applied && !usable) {
                remove_item_stats(owner, item);
                ITEMS_SetItemFlag(item, IFLAG_NOEQUIP, TRUE);
                changed = true;
            } else if (!applied && usable && !ITEMS_CheckItemFlag(item, IFLAG_BROKEN, __LINE__, __FILE__)) {
                ITEMS_SetItemFlag(item, IFLAG_NOEQUIP, FALSE);
                apply_item_stats(owner, item);
                if (!item_applied(item)) continue;
                changed = true;
            }
        }
        if (!changed) break;
        ++changes;
    }
    return changes;
}

// Applies an owned-item record to the canonical player's real inventory. Equip (6) seats the item
// in its body slot; weapon switch (0x17) does the same after evicting the slot's occupant; unequip
// (8) removes the worn item with the packet's GUID. Each leaves the owner's stat list holding
// exactly the worn, usable items' lists. `existing` is the already-owned UNIT_ITEM with the packet
// GUID, if one exists in any realm player's inventory. A fresh decoded unit is staged first, so
// malformed or rejected packets leave an existing item and its inventory position intact.
EMSCRIPTEN_KEEPALIVE int d2_realm_client_owned_item_apply(D2UnitStrc* owner,
        D2UnitStrc* existing, const uint8_t* bytes, int length) {
    D2RealmOwnedItemPacket packet{};
    if (d2_realm_d2gs_parse_owned_item(bytes, length, &packet) != length) return -1;
    const bool unequip = packet.action == ITEMACTION_UNEQUIP;
    const bool weaponSwitch = packet.action == ITEMACTION_WEAPONSWITCH;
    if (packet.action != ITEMACTION_EQUIP && !unequip && !weaponSwitch) return 0;
    if (packet.ownerType != UNIT_PLAYER || !player_owner(owner) ||
        packet.ownerGuid != owner->dwUnitId || packet.itemGuid == D2UnitInvalidGUID) return 0;
    if (existing && (existing->dwUnitType != UNIT_ITEM || existing->dwUnitId != packet.itemGuid ||
        !existing->pItemData)) return 0;

    if (unequip) {
        if (!existing || existing->pItemData->pExtraData.pParentInv != owner->pInventory) return 0;
        drop_from_owner(owner, existing);
        d2_realm_client_owned_item_reconcile(owner);
        return 1;
    }

    uint8_t bodyLoc = BODYLOC_NONE;
    D2UnitStrc* staged = decode_item(packet, bytes + packet.bitstreamOffset,
        static_cast<int>(packet.bitstreamLength), bodyLoc);
    if (!staged) return -1;

    D2UnitStrc* occupant = INVENTORY_GetItemFromBodyLoc(owner->pInventory, bodyLoc);
    if (occupant && occupant != existing) {
        if (!weaponSwitch) {
            release_item(staged);
            return 0;
        }
        drop_from_owner(owner, occupant);
    }

    D2UnitStrc* placed = staged;
    if (existing) {
        D2InventoryStrc* oldInventory = existing->pItemData->pExtraData.pParentInv;
        const uint8_t oldBodyLoc = existing->pItemData->nBodyLoc;
        if (oldInventory) {
            if (item_applied(existing) && oldInventory->pOwner) remove_item_stats(oldInventory->pOwner, existing);
            INVENTORY_RemoveItem(existing);
        }
        const bool decoded = decode_existing_item(existing, bytes + packet.bitstreamOffset,
            static_cast<int>(packet.bitstreamLength), bodyLoc);
        if (!decoded || !INVENTORY_PlaceItemInBodyLoc(owner->pInventory, existing, bodyLoc)) {
            if (oldInventory && oldBodyLoc > BODYLOC_NONE && oldBodyLoc < NUM_BODYLOC) {
                INVENTORY_PlaceItemInBodyLoc(oldInventory, existing, oldBodyLoc);
                if (oldInventory->pOwner) d2_realm_client_owned_item_reconcile(oldInventory->pOwner);
            }
            release_item(staged);
            return decoded ? 0 : -1;
        }
        release_item(staged);
        placed = existing;
    } else if (!INVENTORY_PlaceItemInBodyLoc(owner->pInventory, staged, bodyLoc)) {
        release_item(staged);
        return 0;
    }

    ITEMS_SetBodyLocation(placed, bodyLoc);
    d2_realm_client_owned_item_reconcile(owner);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int d2_realm_client_owned_item_readback(const D2UnitStrc* owner,
        int bodyLoc, D2RealmOwnedItemView* out) {
    if (!out) return 0;
    *out = {};
    out->version = 1;
    out->size = sizeof(*out);
    if (!player_owner(owner) || bodyLoc <= BODYLOC_NONE || bodyLoc >= NUM_BODYLOC) return 0;
    D2UnitStrc* item = INVENTORY_GetItemFromBodyLoc(owner->pInventory, bodyLoc);
    if (!item || item->dwUnitType != UNIT_ITEM || !item->pItemData) return 0;
    out->guid = item->dwUnitId;
    out->classId = static_cast<uint32_t>(item->dwClassId);
    const D2ItemsTxt* record = DATATBLS_GetItemsTxtRecord(item->dwClassId);
    out->code = record ? record->dwCode : 0;
    out->ownerGuid = item->pItemData->dwOwnerGUID;
    out->flags = item->pItemData->dwItemFlags;
    out->mode = item->dwAnimMode;
    out->bodyLoc = item->pItemData->nBodyLoc;
    out->unitAddress = reinterpret_cast<uintptr_t>(item);
    return 1;
}

}  // extern "C"
