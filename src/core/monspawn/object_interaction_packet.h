#pragma once

#include <array>
#include <cstdint>
#include <cstring>

namespace d2::monspawn {

// Retail D2GS opcode 0x13 is nine bytes: opcode, 32-bit unit type, 32-bit GUID.
inline std::array<uint8_t, 9> objectInteractionPacket(uint32_t objectId) {
    std::array<uint8_t, 9> packet{};
    packet[0] = 0x13;
    constexpr uint32_t unitObject = 2;
    std::memcpy(packet.data() + 1, &unitObject, sizeof unitObject);
    std::memcpy(packet.data() + 5, &objectId, sizeof objectId);
    return packet;
}

template <class Receiver>
int dispatchObjectInteraction(uint32_t objectId, Receiver&& receiver) {
    const auto packet = objectInteractionPacket(objectId);
    return receiver(packet.data(), packet.size());
}

} // namespace d2::monspawn
