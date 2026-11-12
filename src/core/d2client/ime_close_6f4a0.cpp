// 1.13c D2Client+0x6F4A0 (260 B): IME/input shutdown. Imports and globals are host seams.
// hwnd = 0xD0B4(); none: fatal 0x18C (no return). The four entries of the table at 0xE27D8 (12 bytes: ESI, ECX, EDX) go to
// 0x3CA00 with EAX = hwnd. imeOn (byte 0x11C038) clear: done. Else 0x3CC60(EAX 7, 0xD0B4(), string 0xE2808); imc = 0xBE2A(0xD0B4());
// imc: 0xBE4E(imc, 0) (open status off), game type (0x113F60) 7 or 0xC: 0xBE5A(imc, &conv, &sent), 0xBE48(imc, 0, sent);
// then 0xBE3C(0xD0B4(), imc). The two text buffers (0x11EC40, 0x11FE98) are cleared; flag 0x11C034 set: 0xBF86(EDX string 0xD4E5C, 0x297, 0), flag := 0.
// Returns the EAX retail leaves at each exit.
#include <cstdint>
#include <emscripten.h>

struct D2ClientImeClose6f4a0Env {
    uint32_t (*hwnd)();                                                  // 0xD0B4
    void (*fatal)(uint32_t strId);                                       // 0xBF92(0x18c) -> 0xBF80 -> exit(-1)
    uint32_t (*freeEntry)(uint32_t hwnd, uint32_t esi, uint32_t ecx, uint32_t edx);   // 0x3CA00
    uint32_t (*log)(uint32_t hwnd, uint32_t strRva);                     // 0x3CC60 (EAX = 7), string D2Client+0xE2808
    uint32_t (*getContext)(uint32_t hwnd);                               // 0xBE2A
    uint32_t (*setOpen)(uint32_t imc, uint32_t open);                    // 0xBE4E
    uint32_t (*getConv)(uint32_t imc, uint32_t* conv, uint32_t* sent);   // 0xBE5A
    uint32_t (*setConv)(uint32_t imc, uint32_t conv, uint32_t sent);     // 0xBE48
    uint32_t (*release)(uint32_t hwnd, uint32_t imc);                    // 0xBE3C
    uint32_t (*ord10043)(uint32_t a, uint32_t b);                        // 0xBF86, EDX = string D2Client+0xD4E5C
    const uint32_t* tbl;                                                 // D2Client+0xE27D8, 4 x (ESI, ECX, EDX)
    uint8_t imeOn;                                                       // byte D2Client+0x11C038
    uint32_t gameType;                                                   // D2Client+0x113F60
    uint16_t comp, read;                                                 // first words of D2Client+0x11EC40, +0x11FE98
    uint32_t flag34;                                                     // D2Client+0x11C034
};

// 1.13c D2Client+0x6F4A0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_6f4a0(D2ClientImeClose6f4a0Env* e) {
    const uint32_t hw = e->hwnd();
    if (!hw) { e->fatal(0x18c); return 0; }
    uint32_t eax = 0;
    for (uint32_t i = 0; i < 4; i++) eax = e->freeEntry(hw, e->tbl[i * 3], e->tbl[i * 3 + 1], e->tbl[i * 3 + 2]);
    if (!e->imeOn) return (eax & 0xffffff00u) | e->imeOn;
    e->log(e->hwnd(), 0xe2808);
    const uint32_t imc = e->getContext(e->hwnd());
    eax = imc;
    if (imc) {
        e->setOpen(imc, 0);
        if (e->gameType == 7 || e->gameType == 0xc) {
            uint32_t conv = 0, sent = 0;
            e->getConv(imc, &conv, &sent);
            e->setConv(imc, 0, sent);
        }
        eax = e->release(e->hwnd(), imc);
    }
    e->comp = 0; e->read = 0;
    if (e->flag34) { eax = e->ord10043(0x297, 0); e->flag34 = 0; }
    return eax;
}
