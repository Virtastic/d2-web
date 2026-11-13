// 1.13c D2Game+0x19640: AI think entry. Fastcall ECX = game, EDX = unit, one stack word `ai` (ret 4). A null unit or a unit whose type
// word (+0x10) is not 1 ends in 0xF22C0 (EAX = game; 5). Every other unit runs the think body at 0x19662 (d2g_19662).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_19662(uint32_t unit, uint32_t game, uint32_t ai, const D2Seam* e);
extern "C" uint32_t d2g_19640(uint32_t game, uint32_t unit, uint32_t ai, const D2Seam* e)
{
    if (!unit || d2rd32(unit + 0x10) != 1) return e->call(0xF22C0, game, 0, 0, 0, 0, 5);
    return d2g_19662(unit, game, ai, e);
}
