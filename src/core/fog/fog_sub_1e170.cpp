// 1.13c Fog+0x1E170 (ordinal 10093): async-request blocking wait. Stateful (Win32 event waits, the
// 0x1D740 completion, process exit on corruption): NAMED, no FN vector.
#include <cstdint>

namespace {
constexpr uint32_t kMagicMask = 0xfffffff0u, kMagic = 0x41737960u;
}

struct FogAsyncWait {
    uint8_t pad48[0x48];
    uint32_t flags;                 // +0x48: magic | done bit
};
struct FogAsyncWaitSeams {
    uint32_t (*poll)(FogAsyncWait*);      // sub_1DC40 (done bit)
    void (*signal)(FogAsyncWait*);        // Ordinal_282(*(req+0x3c), 1)
    bool (*waitInfinite)(FogAsyncWait*);  // WaitForSingleObject(**(HANDLE**)(req+0x38), INFINITE) == WAIT_OBJECT_0
    void (*complete)(FogAsyncWait*);      // sub_1D740
};

// 1.13c Fog+0x1E170
extern "C"
void fog_sub_1e170(FogAsyncWait* r, const FogAsyncWaitSeams* s, void (*fatal)(int line, uint32_t flags)) {
    if (!r) { fatal(0x237, 0); return; }  // FogError(6, ..., 0x237), _exit(-1)
    if ((r->flags & kMagicMask) != kMagic) { fatal(0x238, r->flags); return; }
    if (s->poll(r)) return;
    s->signal(r);
    if (!s->waitInfinite(r)) { fatal(0x24d, r->flags); return; }  // sub_11100(..., 0x24d), _exit(-1)
    s->complete(r);
}
