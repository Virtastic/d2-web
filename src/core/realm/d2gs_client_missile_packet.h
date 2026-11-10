#pragma once
// 0x73: a missile the server tells the client to create (one the client did not launch itself). The handler builds
// the client's missile-create parameters from the packet: the local player is the creating unit, the target point is
// used only when both of its coordinates are non-zero, and owner, pierce and current frame are set after creation.
#include <cstdint>

struct D2RealmMissilePacket {
    uint32_t flags;        // 0x40000001, or 0x40000021 with a target point
    uint32_t missileId;
    int32_t x, y, targetX, targetY;
    int32_t frame;
    uint32_t ownerType, ownerGuid;
    uint32_t level, pierce;
};

inline bool d2_realm_parse_missile_packet(const uint8_t* p, int length, D2RealmMissilePacket& out) {
    if (!p || length != 0x20 || p[0] != 0x73) return false;
    const auto u16 = [p](int at) { return static_cast<uint32_t>(p[at]) | static_cast<uint32_t>(p[at + 1]) << 8; };
    const auto i32 = [&](int at) { return static_cast<int32_t>(u16(at) | u16(at + 2) << 16); };
    out.missileId = u16(5);
    out.x = i32(7);
    out.y = i32(0x0b);
    const int32_t tx = i32(0x0f), ty = i32(0x13);
    const bool aimed = tx != 0 && ty != 0;
    out.flags = aimed ? 0x40000021u : 0x40000001u;
    out.targetX = aimed ? tx : 0;
    out.targetY = aimed ? ty : 0;
    out.frame = static_cast<int16_t>(u16(0x17));
    out.ownerType = p[0x19];
    out.ownerGuid = static_cast<uint32_t>(i32(0x1a));
    out.level = p[0x1e];
    out.pierce = p[0x1f];
    return true;
}
