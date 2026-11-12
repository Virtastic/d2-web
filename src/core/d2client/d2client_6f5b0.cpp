// 1.13c D2Client+0x6F5B0 (649 B): IME start-up. hwnd = 0xD0B4(); none: fatal 0x179 (no return). The four entries of the table at
// 0xE27D8 (12 bytes: ESI, ECX, EDX) go to 0x3CA80 with EAX = hwnd. The game type (0xD1E6) is stored at 0x113F60; a type outside
// {6, 7, 8, 9, 0xC} returns it. imeOn (byte 0x11C038) := 0; 0x6F060 false returns its EAX; imc = 0xBE2A(0xD0B4()), none returns 0.
// The first words of the two text buffers (0x11EC40, 0x11FE98) are cleared, then by type (lang = low word of GetSystemDefaultLangID):
//  6, lang 0x411: imeOn 1, 0x3CCC0(EAX 7, 0xD0B4(), string 0xE2808), 0xBE4E(imc, 1): nonzero -> flag 0x113F64 = 1, mode 0x11C030 = 3,
//     zero -> flag 0, mode 0.
//  7 / 0xC, lang 0x412 or 0x812: 0xBE4E(imc, 1), 0xBE5A(imc, &conv, &sent), 0xBE48(imc, 1, sent), 0xBE5A(imc, &conv, &sent);
//     conv bit 0 -> mode 1, flag 1, imeOn 1; else mode 0, imeOn 1; then 0x3CCC0.
//  9 / 8: flag 0, imeOn 1, 0xBE36(GetKeyboardLayout(0)) zero -> 0xBE54(0xD0B4(), 0x70 / 0x10); 0xBE4E(imc, 1); mode 4 / 5; 0x3CCC0.
// Every path past imc ends with 0xBE3C(0xD0B4(), imc) and returns its EAX. Win32 imports go through the host as 0xF0000002
// (GetSystemDefaultLangID) and 0xF0000004 (GetKeyboardLayout, stack 0).
#include "client_host.h"
#include "d2client_hw.h"

struct D2c6f5b0State {
    const uint32_t* tbl;   // D2Client+0xE27D8, 4 x (ESI, ECX, EDX)
    uint32_t gameType;     // D2Client+0x113F60
    uint8_t flag64;        // byte D2Client+0x113F64
    uint32_t mode;         // D2Client+0x11C030
    uint8_t imeOn;         // byte D2Client+0x11C038
    uint16_t comp, read;   // first words of D2Client+0x11EC40, +0x11FE98
};

// 1.13c D2Client+0x6F5B0
extern "C" uint32_t d2c_6f5b0(D2c6f5b0State* s, D2cHost h) {
    const uint32_t hw = hc(h, 0xd0b4);
    if (!hw) { hc(h, 0xbf92, 0, 0, 0, 0, 0, 0x179); return 0; }
    for (uint32_t i = 0; i < 4; i++) hc(h, 0x3ca80, hw, s->tbl[i * 3 + 1], s->tbl[i * 3 + 2], 0, s->tbl[i * 3]);
    const uint32_t gt = hc(h, 0xd1e6);
    s->gameType = gt;
    if (gt != 6 && gt != 9 && gt != 8 && gt != 7 && gt != 0xc) return gt;
    s->imeOn = 0;
    const uint32_t ime = hc(h, 0x6f060);
    if ((ime & 0xff) == 0) return ime;
    const uint16_t lang = (uint16_t)hc(h, 0xf0000002);
    const uint32_t imc = hc(h, 0xbe2a, 0, 0, 0, 0, 0, hc(h, 0xd0b4));
    if (!imc) return 0;
    s->read = 0; s->comp = 0;
    bool log = false;
    switch (s->gameType) {
    case 6:
        if (lang != 0x411) break;
        s->imeOn = 1;
        hc(h, 0x3ccc0, 7, 0, 0, 0, 0, hc(h, 0xd0b4), 0xB2000000u | 0xe2808);
        if (hc(h, 0xbe4e, 0, 0, 0, 0, 0, imc, 1)) { s->flag64 = 1; s->mode = 3; }
        else { s->flag64 = 0; s->mode = 0; }
        return hc(h, 0xbe3c, 0, 0, 0, 0, 0, hc(h, 0xd0b4), imc);
    case 7: case 0xc: {
        if (lang != 0x412 && lang != 0x812) break;
        hc(h, 0xbe4e, 0, 0, 0, 0, 0, imc, 1);
        uint32_t conv = 0, sent = 0;
        D2cCall c = {}; c.rva = 0xbe5a; c.s[0] = imc; c.s[1] = 0xB3000000u; c.s[2] = 0xB3000000u;
        h(&c); conv = c.out[0]; sent = c.out[1];
        hc(h, 0xbe48, 0, 0, 0, 0, 0, imc, 1, sent);
        D2cCall d = {}; d.rva = 0xbe5a; d.s[0] = imc; d.s[1] = 0xB3000000u; d.s[2] = 0xB3000000u;
        h(&d); conv = d.out[0];
        if (conv & 1) { s->mode = 1; s->flag64 = 1; s->imeOn = 1; }
        else { s->mode = 0; s->imeOn = 1; }
        log = true;
        break;
    }
    case 9: case 8:
        s->flag64 = 0; s->imeOn = 1;
        if (!hc(h, 0xbe36, 0, 0, 0, 0, 0, hc(h, 0xf0000004, 0, 0, 0, 0, 0, 0)))
            hc(h, 0xbe54, 0, 0, 0, 0, 0, hc(h, 0xd0b4), s->gameType == 9 ? 0x70u : 0x10u);
        hc(h, 0xbe4e, 0, 0, 0, 0, 0, imc, 1);
        s->mode = s->gameType == 9 ? 4u : 5u;
        log = true;
        break;
    default: break;
    }
    if (log) hc(h, 0x3ccc0, 7, 0, 0, 0, 0, hc(h, 0xd0b4), 0xB2000000u | 0xe2808);
    return hc(h, 0xbe3c, 0, 0, 0, 0, 0, hc(h, 0xd0b4), imc);
}
