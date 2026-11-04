// D2Game AI think-event reschedule over retail-layout (32-bit) memory. Written from the 1.13c disassembly.
// Convention: EAX = game, EBX = unit, [esp+4] = frames (ret 4). Game +0xA8 frame counter; unit +0xDC event list
// (event +0 type byte, +0x24 next). The event lookup takes EAX = unit, ESI = event type 2; the inlined delete walks the list
// and hands each type-2 event to the free callee (EAX = event, EBX = unit, ESI = next); the set callee takes ECX = game, EDX = unit.
#include "d2game_seam4e.h"

// 1.13c D2Game+0xF2040
extern "C" void d2game_f2040(uint32_t game, uint32_t unit, uint32_t frames, const D2Seam* e)
{
    if (!frames) frames = 1;
    const uint32_t at = e->call(0x8E490, unit, 0, 0, 0, 2);
    if (at && at != d2rd32(game + 0xA8) && at < d2rd32(game + 0xA8) + frames) return;
    if (unit) {
        for (uint32_t ev = d2rd32(unit + 0xDC); ev;) {
            const uint32_t next = d2rd32(ev + 0x24);
            if (d2rd8(ev) == 2) e->call(0x8E6F0, ev, 0, 0, unit, next);
            ev = next;
        }
    }
    const uint32_t w[5] = { 2, d2rd32(game + 0xA8) + frames, 0, 0, 0 };
    e->call(0x8EA80, 0, game, unit, 0, 0, w[0], w[1], w[2], w[3], w[4]);
    e->blk(0x8EA80, w, 5);
}
