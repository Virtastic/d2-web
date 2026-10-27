#pragma once

namespace d2::monspawn {

// Treat any remaining player movement as an approach for the session's rc=2 recovery decision.
// Target metadata is intentionally ignored: the local 1.13c mode callback has not been shown to
// assign pTargetUnit for item pursuit. dwPathPoints is the stored total; currentPointIdx may equal
// it after arrival.
template <typename DynamicPath>
inline int pickupPathHasRemainingPoints(const DynamicPath* path) noexcept {
    return path && path->dwCurrentPointIdx < path->dwPathPoints ? 1 : 0;
}

// The pending session pickup is still this command's when the path names the pile OR the player's
// interact data (what the ITEM case writes via UNITS_SetInteractData, type 4 = item) names it.
// pTargetUnit alone is unreliable (see above), which left walked-to piles unpicked.
inline bool pickupStillOurs(bool pathTargetsPile, int interactType, unsigned interactGuid, unsigned pileId) noexcept {
    return pathTargetsPile || (interactType == 4 && interactGuid == pileId);
}

} // namespace d2::monspawn
