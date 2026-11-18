// 1.13c D2Client+0x028A0 (93 B): initialise one 3-word intrusive-list field. The embedded anchor is the
// word at +4: [0] = 0xDDDDDDDD (poison "empty"), [1] = its own address (circular next == self),
// [2] = ~address (the complement-encoded back link the 0x1030/0x10B0/0x2400 family decodes). The
// decompile zeroes all three first, then stores the final values again; only the final values are
// observable (no aliasing between the three words), so the store pairs collapse to one each.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x028A0: field points at the 3-word block (retail callers pass object+0x28 and +0x38).
// ponytail: links live in 32-bit words; the uintptr_t hop assumes the wasm64 heap stays below 4 GiB.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_28a0(int32_t* field) {
    uint32_t self = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(field + 1));
    field[0] = static_cast<int32_t>(0xddddddddu);
    field[1] = static_cast<int32_t>(self);
    field[2] = static_cast<int32_t>(~self);
}
