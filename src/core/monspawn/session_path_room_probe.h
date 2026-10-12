#pragma once

#include <cstddef>
#include <cstdint>

#include <Path/Path.h>
#include <Path/PathContinuation.h>

// d2_session_path_room_probe is an int32 word array. The first eight words are the
// established room/path prefix; the appended words describe the live path and the
// most recent player path-update call. Keep this POD shared with its offline ABI test.
// Word order: room ordinal, hero x/y, hero room ordinal, pRoom-covers-hero, path point
// count, flags, type, current point, final target x/y, scalar velocity, velocity vector
// x/y, ground predicate evaluated/needs-replan, #10831 called/second-argument/result,
// #10342 result.
struct D2SessionPathRoomProbe {
	int32_t roomOrdinal, heroX, heroY, heroRoomOrdinal, pRoomCoversHero;
	int32_t pathPoints, pathFlags, pathType;
	int32_t currentPoint, finalTargetX, finalTargetY;
	int32_t velocity, velocityX, velocityY;
	int32_t continuationPredicateEvaluated, continuationPredicateNeedsReplan;
	int32_t continuationReplanCalled, continuationReplanTargetUnitArgument;
	int32_t continuationReplanResult, pathUpdateResult;
};

inline void sessionPathRoomProbeCapturePath(D2SessionPathRoomProbe& probe,
	const D2DynamicPathStrc* path, const d2::path::D2PathContinuationTrace* trace)
{
	if (!path) return;
	probe.pathPoints = path->dwPathPoints;
	probe.pathFlags = static_cast<int32_t>(path->dwFlags);
	probe.pathType = static_cast<int32_t>(path->dwPathType);
	probe.currentPoint = path->dwCurrentPointIdx;
	probe.finalTargetX = path->tFinalTargetCoord.X;
	probe.finalTargetY = path->tFinalTargetCoord.Y;
	probe.velocity = path->dwVelocity;
	probe.velocityX = path->tVelocityVector.nX;
	probe.velocityY = path->tVelocityVector.nY;
	if (!trace) return;
	probe.continuationPredicateEvaluated = trace->predicateEvaluated;
	probe.continuationPredicateNeedsReplan = trace->predicateNeedsReplan;
	probe.continuationReplanCalled = trace->replanCalled;
	probe.continuationReplanTargetUnitArgument = trace->replanTargetUnitArgument;
	probe.continuationReplanResult = trace->replanResult;
	probe.pathUpdateResult = trace->pathUpdateResult;
}

static_assert(sizeof(D2SessionPathRoomProbe) == 20 * sizeof(int32_t));
static_assert(offsetof(D2SessionPathRoomProbe, currentPoint) == 8 * sizeof(int32_t));
static_assert(offsetof(D2SessionPathRoomProbe, finalTargetX) == 9 * sizeof(int32_t));
static_assert(offsetof(D2SessionPathRoomProbe, continuationPredicateEvaluated) == 14 * sizeof(int32_t));
static_assert(offsetof(D2SessionPathRoomProbe, continuationReplanTargetUnitArgument) == 17 * sizeof(int32_t));
