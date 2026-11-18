// 1.13c D2Client+0x9DB90 (150 B, no args): party_hostile_flag. g is the module window
// 0x11BBFC..0x11BC17: g[0] = the player unit (0x11BBFC, its +0xC id), g[6] = the roster list head (0x11BC14; node +0x10 id,
// +0x34 relation header, +0x38 alias list, +0x80 next). Returns 1 when some roster member's relation entry for the player has bit 3.
#include "cl_env.h"
extern "C" uint32_t d2c_9db90(const uint32_t* g, const cl::Env& e) {
    const uint32_t head = g[6], unit = g[0];
    const uint32_t me = unit ? e.R(unit + 0xc) : 0xffffffffu;
    for (uint32_t a = head; a; a = e.R(a + 0x80)) {
        const uint32_t id = e.R(a + 0x10);
        if (id == 0xffffffffu) continue;
        for (uint32_t b = head; b; b = e.R(b + 0x80)) {
            bool hit = e.R(b + 0x10) == id;
            for (uint32_t k = e.R(b + 0x38); !hit && k; k = e.R(k + 4)) hit = e.R(k) == id;
            if (!hit) continue;
            const uint32_t hdr = e.R(b + 0x34);
            if (hdr && id != me)
                for (uint32_t r = e.R(hdr); r; r = e.R(r + 8))
                    if (e.R(r) == me) { if (e.R(r + 4) & 8) return 1; break; }
            break;
        }
    }
    return 0;
}
