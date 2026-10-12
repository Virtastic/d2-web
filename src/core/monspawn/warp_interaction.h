#pragma once

#include <cstdint>
#include <utility>

// Policy at the retail 1.13c PlrMsg UNIT_TILE interaction boundary. Its accepted packet
// distance is at most 50 subtiles; accepted distances above 4 approach, and 0..4 warp.
enum class D2WarpInteractionStep : uint8_t {
    Reject,
    TooFar,
    Approach,
    Warp,
};

constexpr D2WarpInteractionStep d2WarpInteractionStep(
    bool isTile,
    uint32_t requestedGuid,
    uint32_t resolvedGuid,
    int32_t distance) {
    if (!isTile || requestedGuid != resolvedGuid || distance < 0) {
        return D2WarpInteractionStep::Reject;
    }
    if (distance > 50) return D2WarpInteractionStep::TooFar;
    return distance > 4 ? D2WarpInteractionStep::Approach : D2WarpInteractionStep::Warp;
}

struct D2WarpInteractionDispatch {
    D2WarpInteractionStep step;
    int32_t handlerResult;
    bool dispatched;
};

// Construct the exact 0x13 type/GUID packet only after validating the selected UNIT_TILE.
template <typename Dispatch>
D2WarpInteractionDispatch d2DispatchWarpInteraction(
    bool isTile,
    uint32_t tileType,
    uint32_t requestedGuid,
    uint32_t resolvedGuid,
    int32_t distance,
    Dispatch&& dispatch) {
    const D2WarpInteractionStep step = d2WarpInteractionStep(isTile, requestedGuid, resolvedGuid, distance);
    if (step == D2WarpInteractionStep::Reject || step == D2WarpInteractionStep::TooFar) {
        return { step, -1, false };
    }

    uint8_t packet[9] = { 0x13 };
    for (unsigned i = 0; i < 4; ++i) {
        packet[1 + i] = static_cast<uint8_t>(tileType >> (i * 8));
        packet[5 + i] = static_cast<uint8_t>(requestedGuid >> (i * 8));
    }
    return { step, std::forward<Dispatch>(dispatch)(packet, sizeof packet), true };
}
