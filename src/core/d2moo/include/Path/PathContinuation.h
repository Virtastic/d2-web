#pragma once

namespace d2::path {

// 1.13c D2Common 0x6fd5db70's null-target ground-path terminal predicate. Its
// caller handles target-unit and circular-monster paths separately.
constexpr bool needsGroundPathContinuationReplan(
	int currentPoint, int pointCount,
	int currentX, int currentY, int finalX, int finalY) {
	return currentPoint >= pointCount &&
		(currentX != finalX || currentY != finalY);
}

} // namespace d2::path
