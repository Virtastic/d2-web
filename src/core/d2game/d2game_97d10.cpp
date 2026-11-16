// 1.13c D2Game+0x97D10: sweep the owner's 12-byte list table and hand every node whose unit is gone, dead or dying to
// 0x97B50 (caller 0x98250). ESI = pGame, stack = (pOwner, ppLists), ret 8.
#include <cstdint>
struct G97D10Env {
    void (*drop)(uint32_t game, uint32_t owner, uint32_t id);   // 0x97B50 (ECX game, EDX owner, stack id, 0)
};
static uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Game+0x97D10
void g_97d10(uint32_t game, uint32_t owner, uint32_t ppLists, uint32_t dt, const G97D10Env& env)
{
    for (int32_t i = 1; i < (int32_t)rd32(dt + 0xBF0); ++i) {   // the count is read again every round
        if (i == 7) continue;                                    // entry offset 0x54 is never swept
        uint32_t node = rd32(rd32(ppLists) + (uint32_t)i * 12);
        while (node != 0) {
            const uint32_t next = rd32(node + 0x14), id = rd32(node + 4);
            if (game == 0) return;                               // retail: fatal assert (line 0x2D3), the process exits
            bool keep = false;
            for (uint32_t u = rd32(game + 0x1320 + (id & 0x7F) * 4); u != 0; u = rd32(u + 0xE4)) {
                if (rd32(u + 0xC) != id) continue;
                if ((*(const uint8_t*)(uintptr_t)(u + 0xC6) & 1) != 0) break;
                const uint32_t type = rd32(u), mode = rd32(u + 0x10);
                if (type == 0) keep = mode != 0 && mode != 0x11;
                else keep = type == 1 && mode != 0 && mode != 0xC;
                break;
            }
            if (!keep) env.drop(game, owner, id);
            node = next;
        }
    }
}
