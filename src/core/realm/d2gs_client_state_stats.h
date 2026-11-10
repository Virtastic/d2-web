#pragma once
#include <cstddef>
#include <cstdint>

struct D2UnitStrc;

struct D2RealmClientStateStat {
    uint16_t statId;
    uint16_t layer;
    int32_t value;
};

// The stat entries of one state in a 0xA8 / 0xAA packet, applied one entry at a time. Returns false when a state list could not be made.
bool d2gs_client_apply_state_stats(D2UnitStrc* unit, uint8_t state,
    const D2RealmClientStateStat* stats, size_t count);
