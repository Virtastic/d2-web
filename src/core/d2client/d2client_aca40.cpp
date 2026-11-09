// 1.13c D2Client+0xACA40: remove unit (ECX = packet); a player packet naming the local player's own roster id is ignored.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_unit_ports.h"
extern "C" {
// 1.13c D2Client+0xACA40
// Returns retail's EAX: the roster id on the ignored path, else the callee's (void here, so 0).
EMSCRIPTEN_KEEPALIVE uint32_t d2c_aca40(const D2cRemoveEnv* e, const uint8_t* p) {
    uint32_t guid; std::memcpy(&guid, p + 2, 4);
    const uint32_t type = p[1];
    if (type == 1) {
        uint32_t self = 0xFFFFFFFFu;
        if (e->localPlayer) {
            const uint32_t key = e->rd32(e->localPlayer + 0xC);
            for (uintptr_t n = e->rosterHead; n; n = e->rd32(n + 0x30)) {
                if (e->rd32(n + 4) == 7 && e->rd32(n + 0xC) == key) { self = e->rd32(n + 8); break; }
            }
        }
        if (guid == self) return self;
    }
    e->fA6EF0(e->pkt->unitTable + type * 0x200, guid & 0x7F, guid, type);
    return 0;
}
}
