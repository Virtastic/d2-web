// Fog: destroy an async request. Retail (disassembly, not the 1.10 wing in
// fog_asyncdata_m01.cpp) is fastcall(ECX = req), void. Magic 0x41737960 (flags & 0xFFFFFFF0) is checked first; a deferred
// request (flags & 8) is unlinked from the list head; poll (1DC40) decides whether the request is still running; if not it is
// cancelled (Storm 283), its event waited and completed (1D740); an owned buffer (flags & 2) is freed; the request is freed.
// Fatal lines: 0x295 null, 0x296 bad magic, 0x2C8 owned buffer is null.
#include <cstring>

#include "fog_1e010.h"

namespace {
constexpr uint32_t kFile = 0x6FF76924u;   // the FogError source-file string (module data)

uint32_t ld32(const uint8_t* base, uint32_t off) {
    uint32_t v;
    memcpy(&v, base + off, 4);
    return v;
}
void st32(uint8_t* base, uint32_t off, uint32_t v) { memcpy(base + off, &v, 4); }
}  // namespace

// Destroy the request (retail flow).
void fog_req_destroy_1e010(uint8_t* req, uint32_t* listHead, uint32_t poolEnabled, const FogReqSeams* s) {
    if (!req) s->fatal(0x295);
    if ((ld32(req, 0x48) & 0xFFFFFFF0u) != 0x41737960u) s->fatal(0x296);
    const uint32_t me = (uint32_t)(uintptr_t)req;
    if (ld32(req, 0x48) & 8) {
        // retail: walk the link cells from DAT 9af64 (cell = &head, then node + 0x50) and unlink req
        uint32_t* cell = listHead;
        while (cell) {
            if (*cell == me) {
                *cell = ld32(req, 0x50);
                break;
            }
            if (!*cell) break;   // retail would read address 0x50 here; a listed request never reaches it
            cell = (uint32_t*)(uintptr_t)(*cell + 0x50);
        }
    }
    if (!s->poll(me)) {
        s->cancel(ld32(req, 0x3C));
        s->waitEvent(*(const uint32_t*)(uintptr_t)ld32(req, 0x38));   // **(req+0x38): the slot's event handle
        s->complete(me);
    }
    const uint32_t ctx = poolEnabled ? ld32(req, 0x04) : 0;
    if (ld32(req, 0x48) & 2) {
        const uint32_t buf = ld32(req, 0x3C);
        if (!buf) s->fatal(0x2C8);
        s->poolFree(ctx, buf, kFile, 0x2CB);
        st32(req, 0x3C, 0);
        st32(req, 0x48, ld32(req, 0x48) & 0xF);
    }
    s->poolFree(ctx, me, kFile, 0x2D2);
}
