// src/core/realm/bnclient_batch_a180.cpp -- BNClient.dll sub_A180, transcribed from the cached decompile.
#include <cstdint>
#include <emscripten.h>

typedef void (*BnImpFn)(void* a, void* b, void* c);

// 1.13c BNClient+0x0A180 (56 B, caller 0xB0C0): four imported calls on one stack slot, in order:
// Ordinal_624(&slot); Ordinal_612(&slot, param_2); Ordinal_622(&slot, slot, &slot); Ordinal_606(&slot).
// The slot is the imports' own object; the imports (ordinals 624/612/622/606, the 0x872E/0x870A/0x86F8/
// 0x8740 thunks) are seams here. param_1 is unused. Stateful through the imports, so no vector.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_bnclient_a180(uint32_t param_1, uint32_t param_2, uint32_t* slot,
                                              BnImpFn o624, BnImpFn o612, BnImpFn o622, BnImpFn o606) {
    (void)param_1;
    o624(slot, nullptr, nullptr);
    o612(slot, (void*)(uintptr_t)param_2, nullptr);
    o622(slot, (void*)(uintptr_t)*slot, slot);
    o606(slot, nullptr, nullptr);
}
