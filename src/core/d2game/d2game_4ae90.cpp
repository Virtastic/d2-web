// D2Game+0x4AE90 -- CLIENTS_FillCharacterPreviewInfo: fill the 0x23-byte character-preview record `out` (EAX) from a client record (ECX), no stack args.
// 0xA32E(out, 0xD) (Fog word store). Client NULL: fatal (Fog 0xA2DA/0xA2C2, then exit; not reachable). The unit is client +0x174 when client +0x3D4 bit 0 is set, else 0 without a
// cached unit (+0x174 zero) and 0xE03A0(EAX = +0x16C, EDX = +0x170, ECX = +0x1A8) with +0x174 cleared when that finds none. No unit: nothing more.
// level = 0xA42A(unit, 0xC, 0) low byte; class = unit +4 low byte. Two 16-byte blocks filled with -1 and handed to 0xA41E(unit, A, &x), 0xA41E(unit, &x, B); 0xA298(out+2, A, 0xB);
// out+0xD = class + 1 (1 from class 7 up); B bytes of 0 become 0xFF; 0xA298(out+0xE, B, 0xB); out+0x19 = level (1 outside 1..99). Flags = client word +0xA, with bit 3 set from bit 2
// when unit +0x10 is 0 or 0x11, cleared otherwise: 0xA32E(out+0x1A, flags), 0xA32E(out+0x1C, client word +0x45E). out+0x1E..0x20 = client bytes +0x480..0x482 (0 becomes 0xFF),
// out+0x21 = client dword +0x460. Then out[0] = 0 when the level is out of 1..99, the class is 7 or more, flags & 0x13, or 0xA292(out) != 0x21.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x4AE90
extern "C" void d2game_4ae90(uint32_t out, uint32_t cl, const D2Seam* e)
{
    e->call(0xA32E, 0, 0, 0, 0, 0, out, 0xD);
    const uint32_t o2 = out + 2;
    if (!cl) return;   // retail: fatal error path
    uint32_t un;
    if (d2rd8(cl + 0x3D4) & 1) un = d2rd32(cl + 0x174);
    else {
        if (!d2rd32(cl + 0x174)) return;
        un = e->call(0xE03A0, d2rd32(cl + 0x16C), d2rd32(cl + 0x1A8), d2rd32(cl + 0x170), 0, 0);
        if (!un) d2wr32(cl + 0x174, 0);
    }
    if (!un) return;
    const uint8_t lvl = (uint8_t)e->call(0xA42A, 0, 0, 0, 0, 0, un, 0xC, 0);
    const uint32_t cls = d2rd32(un + 4);
    uint8_t blk[40];   // B at 0 (16 bytes), A at 16 (16 bytes), the shared out slot at 32
    memset(blk, 0xFF, 32);
    uint32_t slot = 0;
    const uint32_t pB = (uint32_t)(uintptr_t)blk, pA = pB + 16, pS = (uint32_t)(uintptr_t)&slot;
    e->call(0xA41E, 0, 0, 0, 0, 0, un, pA, pS);
    e->call(0xA298, 0, 0, 0, 0, 0, o2, pA, 0xB);
    d2wr8(o2 + 0xB, (uint8_t)cls < 7 ? (uint32_t)(uint8_t)cls + 1 : 1);
    e->call(0xA41E, 0, 0, 0, 0, 0, un, pS, pB);
    for (int i = 0; i < 16; i++) if (!blk[i]) blk[i] = 0xFF;
    e->call(0xA298, 0, 0, 0, 0, 0, o2 + 0xC, pB, 0xB);
    d2wr8(o2 + 0x17, (lvl > 0 && lvl <= 0x63) ? lvl : 1);
    const uint32_t mode = d2rd32(un + 0x10);
    uint32_t fl = d2rd16(cl + 0xA);
    if ((mode == 0x11 || mode == 0) && (fl & 4)) fl |= 8; else fl &= 0xFFF7;
    e->call(0xA32E, 0, 0, 0, 0, 0, o2 + 0x18, fl);
    e->call(0xA32E, 0, 0, 0, 0, 0, o2 + 0x1A, d2rd16(cl + 0x45E));
    uint32_t a = d2rd8(cl + 0x480), d = d2rd8(cl + 0x481), c = d2rd8(cl + 0x482);
    if (!a) a = 0xFF;
    if (!d) d = 0xFF;
    if (!c) c = 0xFF;
    d2wr8(o2 + 0x1C, a); d2wr8(o2 + 0x1D, d); d2wr8(o2 + 0x1E, c);
    d2wr32(o2 + 0x1F, d2rd32(cl + 0x460));
    const bool bad1 = !(lvl >= 1 && lvl <= 0x63), bad2 = (uint8_t)cls >= 7, bad3 = (fl & 0x13) != 0;
    const bool bad4 = e->call(0xA292, 0, 0, 0, 0, 0, out) != 0x21;
    if (bad1 || bad2 || bad3 || bad4) d2wr8(out, 0);
}
