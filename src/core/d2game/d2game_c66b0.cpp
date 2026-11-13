// 1.13c D2Game+0xC66B0 -- quest callback, ECX = quest record (+4 game, +9 active, +0xC state, +0x18 data), EDX = event record (+0 game, +8 unit); plain ret, EAX leftover = the last 0xDFCC0 answer. An active record (+9 != 0) sets +0xC = 2 and, with the module debug flag (global 0x111C20) set, tells 0xCBE70 twice (EDX = a module string, then the formatted-state buffer; stack: game, 4). In the data block: byte +8 clear sets it and runs 0x33AF0 (EBX 3, stack: record, callback 0xC5BE0); then byte +9 = 0, byte +0 = 1, dword +4 = 0xA478(unit) and three 0xDFCC0 posts (EAX 0, EDI = event game, stack = event unit; callbacks 0xC2F10, 0xC5B80, 0xC4D90). Always last: the list holder (+0x10F4 of the record's game) gets byte +0x20 = 7 and 0xDFCC0 posts callback 0x359C0 (EDI = game, stack 0). `dbg` is the address of the pinned flag; the holder is assumed present.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc66b0(GE& e, uint32_t q, uint32_t ev, uint32_t dbg)
{
    auto post = [&](uint32_t fn, uint32_t edi, uint32_t s0) {
        const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | fn, 0, s0);
        e.edi(0xDFCC0, edi);
        return v;
    };
    const uint32_t game = e.rd32(q + 4);
    if (e.rd8(q + 9)) {
        if (e.rd32(dbg)) e.h(0xCBE70, 0, 0, 0xB2000000u | 0xFB27C, 0, 0, game, 4);
        e.wr8(q + 0xc, 2);
        char buf[0x80];   // wsprintfA(buf, "%d", 2): text only seen by 0xCBE70, recorded by address
        if (e.rd32(dbg)) e.h(0xCBE70, 0, 0, (uint32_t)(uintptr_t)buf, 0, 0, game, 4);
        const uint32_t d = e.rd32(q + 0x18);
        if (!e.rd8(d + 8)) { e.wr8(d + 8, 1); e.h(0x33AF0, 0, 0, 0, 3, 0, q, 0xB2000000u | 0xC5BE0); }
        e.wr8(d + 9, 0); e.wr8(d, 1);
        e.wr32(d + 4, e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(ev + 8)));
        post(0xC2F10, e.rd32(ev), e.rd32(ev + 8));
        post(0xC5B80, e.rd32(ev), e.rd32(ev + 8));
        post(0xC4D90, e.rd32(ev), e.rd32(ev + 8));
    }
    e.wr8(e.rd32(game + 0x10f4) + 0x20, 7);
    return post(0x359C0, game, 0);
}
}
