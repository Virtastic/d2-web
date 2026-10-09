#include "d2gs_client_unit_gfx.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <Fog.h>

namespace {
constexpr size_t kClientGfxDataBytes = 0x804;
constexpr size_t kInitializedPrefixBytes = 0x40;
constexpr size_t kComponentWord = 0x34 / sizeof(uint32_t);
constexpr size_t kArmorWord = 0x38 / sizeof(uint32_t);

bool supported_unit(const D2UnitStrc* unit) {
    return unit && (unit->dwUnitType == UNIT_PLAYER || unit->dwUnitType == UNIT_MONSTER);
}
}

extern "C" int d2_realm_client_unit_gfx_record_valid(const D2UnitStrc* unit) {
    if (!supported_unit(unit) || !unit->pGfxData) return 0;
    const auto* words = reinterpret_cast<const uint32_t*>(unit->pGfxData);
    return words[kComponentWord] == 2 && words[kArmorWord] == 2;
}

extern "C" int d2_realm_client_unit_gfx_empty_record(const D2UnitStrc* unit) {
    if (!d2_realm_client_unit_gfx_record_valid(unit)) return 0;
    const auto* bytes = reinterpret_cast<const uint8_t*>(unit->pGfxData);
    for (size_t i = 0; i < kClientGfxDataBytes; ++i) {
        const uint8_t expected = (i == 0x34 || i == 0x38) ? 2 : 0;
        if (bytes[i] != expected) return 0;
    }
    return 1;
}

extern "C" int d2_realm_client_unit_gfx_init(D2UnitStrc* unit) {
    if (!supported_unit(unit)) return 0;
    if (unit->pGfxData) return d2_realm_client_unit_gfx_record_valid(unit);

    // D2Client 1.13c FUN_6FB18990 allocates 0x804 bytes with Fog #10042, clears the first
    // 0x40-byte prefix, and writes 2 at +0x34 and +0x38. The rest is kept zero here so this
    // engine-owned empty record cannot expose stale child-resource links to its own teardown.
    auto* words = static_cast<uint32_t*>(FOG_Alloc(kClientGfxDataBytes, __FILE__, __LINE__, 0));
    if (!words) return 0;
    std::memset(words, 0, kClientGfxDataBytes);
    std::memset(words, 0, kInitializedPrefixBytes);
    words[kComponentWord] = 2;
    words[kArmorWord] = 2;
    unit->pGfxData = reinterpret_cast<D2GfxDataStrc*>(words);
    return 1;
}

extern "C" void d2_realm_client_unit_gfx_destroy(D2UnitStrc* unit) {
    if (!supported_unit(unit) || !unit->pGfxData) return;

    // This helper creates no D2Gfx child/resource nodes. Those are populated only by the
    // retail client loader; the browser uses its own COF/DCC resource cache. Its empty parent
    // allocation is therefore released directly through Fog #10043.
    FOG_Free(unit->pGfxData, __FILE__, __LINE__, 0);
    unit->pGfxData = nullptr;
}
