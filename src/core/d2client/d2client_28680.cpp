// 1.13c D2Client+0x28680: durability warnings. The player's inventory (player+0x60, player at global 0x11BBFC) is
// scanned over the body slots {1, 3, 8, 9, 10, 4, 5} (.data 0xF5418); nothing draws unless one durable, non-excluded
// item with a non-zero maximum is at or below 20% (stat 0x48 * 100 / max, signed). Then every slot is drawn through
// 0x28420 (EAX severity, ECX special, EDX flag, stack slot): an empty slot 4/5 is skipped, other empty slots draw
// severity 0; an item that is durable and not excluded supplies flag = #10973-class stat(0x33), special = 0xC928,
// and severity 2 at <= 0%, 1 at <= 20% (only with a non-zero maximum).
#include "d2client_callee.h"
// 1.13c D2Client+0x28680
extern "C" void d2c_28680(const D2cSvc& v, const uint32_t* player /*[0x11BBFC]*/) {
    static const uint32_t kSlot[7] = { 1, 3, 8, 9, 10, 4, 5 };
    const uint32_t inv = player[0x60 / 4];
    auto pct = [&](uint32_t it, int32_t max) {
        const int32_t s = (int32_t)d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, it, 0x48, 0);
        return (int32_t)((uint32_t)s * 100u) / max;
    };
    bool warn = false;
    for (uint32_t slot : kSlot) {
        const uint32_t it = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, inv, slot);
        if (!it || !d2c_call(v, 0xc8bc, 0, 0, 0, 0, 0, it) || d2c_call(v, 0xc73c, 0, 0, 0, 0, 0, it)) continue;
        const int32_t max = (int32_t)d2c_call(v, 0xc916, 0, 0, 0, 0, 0, it);
        if (max && pct(it, max) <= 0x14) { warn = true; break; }
    }
    if (!warn) return;
    for (uint32_t slot : kSlot) {
        const uint32_t it = d2c_call(v, 0xc262, 0, 0, 0, 0, 0, inv, slot);
        uint32_t sev = 0, flag = 0, special = 0;
        if (!it) { if (slot == 4 || slot == 5) continue; }
        else if (d2c_call(v, 0xc8bc, 0, 0, 0, 0, 0, it) && !d2c_call(v, 0xc73c, 0, 0, 0, 0, 0, it)) {
            const int32_t max = (int32_t)d2c_call(v, 0xc916, 0, 0, 0, 0, 0, it);
            flag = d2c_call(v, 0xc1cc, 0, 0, 0, 0, 0, it, 0x33);
            special = d2c_call(v, 0xc928, 0, 0, 0, 0, 0, it);
            if (max) { const int32_t p = pct(it, max); sev = p <= 0 ? 2 : p <= 0x14 ? 1 : 0; }
        }
        d2c_call(v, 0x28420, sev, special, flag, 0, 0, slot);
    }
}
