#pragma once
#include <cstdint>

// Retail forms the extra-data address with 32-bit +0x5c, then tests for zero.
// Reject the pre-add sentinel before doing pointer arithmetic on native/wasm64.
inline bool INVENTORY_HasItemExtraData(const void* data) {
    return data && reinterpret_cast<uintptr_t>(data) != UINT32_C(0xffffffa4);
}
