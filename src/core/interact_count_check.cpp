#include <cstdint>
#include <emscripten.h>
#include <Units/Monster.h>
#include <D2Game/MONSTER/MonsterAI.h>

// Replay serialized retail fields as native-width links; unit identities are opaque and never read.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_interact_count_check(const uint32_t* fields, uint32_t count) {
    if (count > 16) return -1;
    D2InteractInfoStrc nodes[16] = {};
    D2UnitStrc identities[16] = {};
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t id = fields[i * 2];
        if (id > 16) return -1;
        nodes[i].pUnit = id ? &identities[id - 1] : nullptr;
        nodes[i].nInteract = static_cast<int32_t>(fields[i * 2 + 1]);
        nodes[i].pNext = i + 1 < count ? &nodes[i + 1] : nullptr;
    }
    D2MonsterInteractStrc container = {count ? nodes : nullptr};
    return MONSTERAI_GetInteractUnitCount(&container);
}
