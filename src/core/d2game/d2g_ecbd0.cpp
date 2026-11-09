// 1.13c D2Game+0xECBD0: pet section ('fj') reader. EAX = end, ECX = self (flag), ESI = unit; stack (ret 0xC): &cursor, op, &errOut.
// 0xECB50 gets EAX = end, EDX = op, EBX = &newCursor, EDI = self and stack unit, &local (ret 8). The tail runs only with a self.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ecbd0(uint32_t end, uint32_t self, uint32_t unit, uint32_t pCur, uint32_t op, uint32_t err, const D2Seam* e)
{
    const uint32_t cur = d2rd32(pCur);
    if (d2rd32(unit + 0x70) == 0 || cur + 2 > end) return 0;
    if (d2rd16(cur) != 0x666a) {
        if (err) d2wr32(err, 0x65);
        return 0x16;
    }
    uint32_t newCur = cur + 2, local = 0;
    if (self) {
        const uint32_t r = e->call(0xECB50, end, 0, op, (uint32_t)(uintptr_t)&newCur, 0, unit, (uint32_t)(uintptr_t)&local);
        e->edi(0xECB50, self);
        if (r) {
            if (err) d2wr32(err, local);
            return 0x16;
        }
        e->call(0xD51A0, 0, 0, 0, 0, 0, unit, self, 0);
        if (e->call(0xDFA90, self) == 0) {
            const uint32_t v = e->call(0xA3A0, 0, 0, 0, 0, 0, self);
            e->call(0xA472, 0, 0, 0, 0, 0, self, 6, v, 0);
        }
    }
    d2wr32(pCur, newCur);
    return 0;
}
