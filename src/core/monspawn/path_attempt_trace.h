#pragma once

#include <cstddef>
#include <cstdint>

namespace d2::path_attempt_trace {

// Offline diagnostic ABI. All values are retail subtiles, command return values, and the
// existing MOVE_* reason enum. A context is consumed by the next native move command.
enum : int32_t { contextNone = 0, onePointPlan = 1, emptyPlanFallback = 2 };

struct Record {
    int32_t sequence;
    int32_t context;
    int32_t plannedX;
    int32_t plannedY;
    int32_t targetX;
    int32_t targetY;
    int32_t commandResult;
    int32_t acceptanceReason;
    int32_t towardPoints;
    int32_t towardEarlyReturn;
    int32_t astarGate;
    int32_t squaredDistance;
    int32_t astarNodesPopped;
    int32_t astarAdvanced;
    int32_t astarExhausted;
    int32_t astarPathPoints;
};

static_assert(sizeof(Record) == 64);
static_assert(offsetof(Record, context) == 4);
static_assert(offsetof(Record, targetX) == 16);
static_assert(offsetof(Record, acceptanceReason) == 28);
static_assert(offsetof(Record, astarPathPoints) == 60);

} // namespace d2::path_attempt_trace
