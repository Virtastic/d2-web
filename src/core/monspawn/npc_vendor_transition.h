#pragma once

#include <cstdint>

// Retail's 0x38 store request is scoped to the player's active NPC interaction.
// Keep the bridge's last accepted vendor state unless this native precondition holds.
inline bool npcVendorRequestMatchesInteraction(uint32_t npcId, bool hasInteraction,
    int32_t unitType, int32_t interactionGuid, int32_t monsterUnitType) {
    return hasInteraction && unitType == monsterUnitType && interactionGuid == static_cast<int32_t>(npcId);
}

inline bool npcVendorTransitionAccepted(int32_t handlerResult, bool hasNpcRecord, bool trading) {
    return handlerResult == 0 && hasNpcRecord && trading;
}

template <typename State, typename ItemList, typename PresentationList>
class NpcVendorStateSnapshot {
public:
    NpcVendorStateSnapshot(State& state, ItemList& items, PresentationList& presentations)
        : state_(state), items_(items), presentations_(presentations),
          savedState_(state), savedItems_(items), savedPresentations_(presentations) {}

    void restore() {
        state_ = savedState_;
        items_ = savedItems_;
        presentations_ = savedPresentations_;
    }

private:
    State& state_;
    ItemList& items_;
    PresentationList& presentations_;
    State savedState_;
    ItemList savedItems_;
    PresentationList savedPresentations_;
};
