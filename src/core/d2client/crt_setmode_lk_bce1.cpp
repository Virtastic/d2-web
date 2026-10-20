// Prior art used: D2MOO has no D2Client CRT module. The shared descriptor
// owner and errno accessor are the existing D2Client CRT implementations;
// the cached 1.13c body supplies the mode transitions and error ordering.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_pioinfo[0x20]; // +0x1239A0, owned by d2client_batch_03f82.cpp
extern "C" int32_t* d2_client_579c();

struct D2ClientModeRecord {
    uint32_t handle;
    uint8_t flags;
    uint8_t remaining[31];
};
static_assert(sizeof(D2ClientModeRecord) == 0x24);
static_assert(offsetof(D2ClientModeRecord, flags) == 4);

// 1.13c D2Client+0x0BCE1
// __setmode_lk assumes the caller has validated and locked the descriptor.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_bce1(uint32_t fd, int32_t mode) {
    const uint32_t address = g_pioinfo[fd >> 5] + (fd & 31u) * 0x24u;
    auto* record = reinterpret_cast<D2ClientModeRecord*>(static_cast<uintptr_t>(address));
    const uint8_t old_flags = record->flags;
    if (mode == 0x8000) record->flags &= 0x7fu;
    else if (mode == 0x4000) record->flags |= 0x80u;
    else {
        *d2_client_579c() = 0x16;
        return -1;
    }
    return (old_flags & 0x80u) ? 0x4000 : 0x8000;
}
