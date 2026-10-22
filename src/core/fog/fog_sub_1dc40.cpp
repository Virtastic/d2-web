// 1.13c Fog+0x1DC40 (ordinal 10092): async-request "is done" poll. Stateful (Win32 event wait on the
// request's handle, then the 0x1D740 completion, which frees the request's buffers): NAMED, no FN vector.
#include <cstdint>

namespace {
constexpr uint32_t kMagicMask = 0xfffffff0u, kMagic = 0x41737960u, kDone = 1u;
}

struct FogAsyncReq {
    uint32_t flags;      // +0x48: magic | done bit
    bool (*signaled)();  // WaitForSingleObject(*handle, 0) == WAIT_OBJECT_0
    void (*complete)();  // sub_1D740
};

// 1.13c Fog+0x1DC40
extern "C"
uint32_t fog_sub_1dc40(FogAsyncReq* r, void (*fatal)(uint32_t flags)) {
    if (!r) return 0;
    if ((r->flags & kMagicMask) != kMagic) {  // FogError(6, ..., 0x1c9, "Unrecoverable internal error %08x"), _exit(-1)
        fatal(r->flags);
        return 0;
    }
    if (!(r->flags & kDone) && r->signaled()) r->complete();
    return r->flags & kDone;
}
