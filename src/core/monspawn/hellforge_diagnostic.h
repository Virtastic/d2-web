#pragma once

#include <cstddef>
#include <cstdint>

// Pointer-free snapshot of the local 1.13c Hellforge quest/event join. This is
// diagnostic state only; callers cannot use it to mutate the simulation.
struct D2HellforgeDiagnostic {
    uint32_t version;
    uint32_t size;
    int32_t found;
    int32_t forgeGuid;
    int32_t forgeAnimMode;
    int32_t questForgeMode;
    int32_t gameFrame;
    int32_t hitCount;
    int32_t gemDropTier;
    int32_t rewardDropsPending;
    int32_t soulstoneSmashed;
    int32_t matchingQuestEventCount;
    int32_t scheduledEventFound;
    int32_t scheduledEventType;
    int32_t scheduledEventUnitType;
    int32_t scheduledEventUnitGuid;
    int32_t scheduledEventDueFrame;
};

static_assert(sizeof(D2HellforgeDiagnostic) == 68);
static_assert(offsetof(D2HellforgeDiagnostic, found) == 8);
static_assert(offsetof(D2HellforgeDiagnostic, forgeGuid) == 12);
static_assert(offsetof(D2HellforgeDiagnostic, hitCount) == 28);
static_assert(offsetof(D2HellforgeDiagnostic, matchingQuestEventCount) == 44);
static_assert(offsetof(D2HellforgeDiagnostic, scheduledEventType) == 52);
static_assert(offsetof(D2HellforgeDiagnostic, scheduledEventDueFrame) == 64);

inline void d2HellforgeResetDiagnostic(D2HellforgeDiagnostic& out,
                                        int32_t gameFrame = -1) {
    out = {};
    out.version = 1;
    out.size = sizeof(out);
    out.forgeGuid = -1;
    out.forgeAnimMode = -1;
    out.questForgeMode = -1;
    out.gameFrame = gameFrame;
    out.hitCount = -1;
    out.gemDropTier = -1;
    out.rewardDropsPending = -1;
    out.soulstoneSmashed = -1;
    out.scheduledEventType = -1;
    out.scheduledEventUnitType = -1;
    out.scheduledEventUnitGuid = -1;
    out.scheduledEventDueFrame = -1;
}

template <typename Event>
inline void d2HellforgeReadQuestEvents(D2HellforgeDiagnostic& out,
                                  const Event* event,
                                  int32_t objectType,
                                  const void* forge) {
    out.matchingQuestEventCount = 0;
    out.scheduledEventFound = 0;
    out.scheduledEventType = -1;
    out.scheduledEventUnitType = -1;
    out.scheduledEventUnitGuid = -1;
    out.scheduledEventDueFrame = -1;
    for (; event; event = event->pNext) {
        if (event->nEventType != 7 || event->nUnitType != objectType ||
            event->pUnit != forge || event->nUnitGUID != out.forgeGuid) {
            continue;
        }
        ++out.matchingQuestEventCount;
        if (!out.scheduledEventFound || event->nExpireFrame < out.scheduledEventDueFrame) {
            out.scheduledEventFound = 1;
            out.scheduledEventType = event->nEventType;
            out.scheduledEventUnitType = event->nUnitType;
            out.scheduledEventUnitGuid = event->nUnitGUID;
            out.scheduledEventDueFrame = event->nExpireFrame;
        }
    }
}
