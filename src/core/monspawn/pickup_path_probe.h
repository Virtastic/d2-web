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

} // namespace d2::monspawn
