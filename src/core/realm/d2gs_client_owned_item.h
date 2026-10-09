#pragma once

#include <cstdint>

struct D2UnitStrc;

struct D2RealmOwnedItemPacket {
    uint32_t itemGuid;
    uint32_t ownerGuid;
    uint32_t bitstreamOffset;
    uint32_t bitstreamLength;
    uint8_t action;
    uint8_t packetSize;
    uint8_t category;
    uint8_t ownerType;
};

static_assert(sizeof(D2RealmOwnedItemPacket) == 20);

struct D2RealmOwnedItemView {
    uint32_t version;
    uint32_t size;
    uint32_t guid;
    uint32_t classId;
    uint32_t code;
    uint32_t ownerGuid;
    uint32_t flags;
    uint32_t mode;
    uint32_t bodyLoc;
    uint64_t unitAddress;
};

static_assert(sizeof(D2RealmOwnedItemView) == 48);

extern "C" {
int d2_realm_d2gs_parse_owned_item(const uint8_t* packet, int length, D2RealmOwnedItemPacket* out);
int d2_realm_client_owned_item_apply(D2UnitStrc* owner, D2UnitStrc* existing,
    const uint8_t* packet, int length);
void d2_realm_client_owned_item_destroy(D2UnitStrc* item);
int d2_realm_client_owned_item_readback(const D2UnitStrc* owner, int bodyLoc,
    D2RealmOwnedItemView* out);
}
