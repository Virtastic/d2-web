// 1.13c D2Game+0xE56E0 -- EAX (low 16 bits replaced on the quiet paths), ECX = quest node (unused), EDX = event (+0 game, +0xC record, +0x14 code, 16-bit word +0x18); no stack argument. When (code, word) is one of (0xF5, 0x1CA), (0xFC, 0x1EB/0x1EA), (0xFE, 0x1F6/0x1F5), (0xFF, 0x203/0x202), (0x108, 0x1DF/0x1DE), (0x129, 0x1C5) the worker 0x695F0 (EAX = the +0xC record, ESI = the code, stack = the game) runs and its answer is returned; otherwise EAX with its low 16 bits set to the word.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe56e0(GE& e, uint32_t eax, uint32_t, uint32_t ev)
{
    const uint32_t w = e.rd16s(ev + 0x18) & 0xffff, code = e.rd32(ev + 0x14);
    bool hit = false;
    switch (code) {
    case 0xf5: hit = w == 0x1ca; break;
    case 0xfc: hit = w == 0x1eb || w == 0x1ea; break;
    case 0xfe: hit = w == 0x1f6 || w == 0x1f5; break;
    case 0xff: hit = w == 0x203 || w == 0x202; break;
    case 0x108: hit = w == 0x1df || w == 0x1de; break;
    case 0x129: hit = w == 0x1c5; break;
    }
    if (hit) return e.h(0x695F0, e.rd32(ev + 0xc), 0, 0, 0, code, e.rd32(ev));
    return (eax & ~0xffffu) | w;
}
}
