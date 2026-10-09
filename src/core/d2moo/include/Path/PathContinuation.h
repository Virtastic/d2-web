#pragma once

#include <cstdint>

struct D2UnitStrc;

namespace d2::path {

// 1.13c D2Common 0x6fd5db70's null-target ground-path terminal predicate. Its
// caller handles target-unit and circular-monster paths separately.
constexpr bool needsGroundPathContinuationReplan(
	int currentPoint, int pointCount,
	int currentX, int currentY, int finalX, int finalY) {
	return currentPoint >= pointCount &&
		(currentX != finalX || currentY != finalY);
}

// Read-only evidence for the most recent D2Common_10342/#10831 continuation attempt on
// the session player. These fields describe calls that actually ran, not a predicate
// reconstructed later from the path's current coordinates.
struct D2PathContinuationTrace {
	const ::D2UnitStrc* unit = nullptr;
	int32_t predicateEvaluated = 0;
	int32_t predicateNeedsReplan = 0;
	int32_t replanCalled = 0;
	int32_t replanTargetUnitArgument = 0;
	int32_t replanResult = 0;
	int32_t pathUpdateResult = 0;
};

inline void beginGroundContinuationTrace(D2PathContinuationTrace& trace, const ::D2UnitStrc* unit)
{
	trace = {};
	trace.unit = unit;
}

inline bool recordGroundContinuationPredicate(D2PathContinuationTrace& trace,
	const ::D2UnitStrc* unit, bool needsReplan)
{
	if (trace.unit != unit) return false;
	trace.predicateEvaluated = 1;
	trace.predicateNeedsReplan = needsReplan ? 1 : 0;
	return true;
}

inline void recordContinuationReplan(D2PathContinuationTrace& trace,
	const ::D2UnitStrc* unit, int targetUnitArgument, int result)
{
	if (trace.unit != unit) return;
	trace.replanCalled = 1;
	trace.replanTargetUnitArgument = targetUnitArgument != 0 ? 1 : 0;
	trace.replanResult = result;
}

inline void recordGroundContinuationUpdate(D2PathContinuationTrace& trace,
	const ::D2UnitStrc* unit, int result)
{
	if (trace.unit != unit) return;
	trace.pathUpdateResult = result != 0 ? 1 : 0;
}

void PATH_BeginGroundContinuationTrace(const ::D2UnitStrc* unit);
void PATH_RecordGroundContinuationPredicate(const ::D2UnitStrc* unit, bool needsReplan);
void PATH_RecordContinuationReplan(const ::D2UnitStrc* unit, int targetUnitArgument, int result);
void PATH_RecordGroundContinuationUpdate(const ::D2UnitStrc* unit, int result);
const D2PathContinuationTrace* PATH_GetGroundContinuationTrace(const ::D2UnitStrc* unit);

} // namespace d2::path
