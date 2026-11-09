// 1.13c D2Client+0xAF930 menuExit: a 7-byte thunk (mov eax,ecx; jmp 0x59ED0) onto the exit-menu handler at 0x59ED0. The handler
// switches on the byte at [ecx+1]; this ports case 0 (the "Save and Exit" request), the case the vector pins. Cases 1.. stay
// with the caller (sub_AF930 returns 0 for them). Globals are retail VAs read through H; register arguments are spelled out per
// callee as the disassembly passes them.
#include <cstdint>
#include <emscripten.h>

namespace d2client_af930 {
using U = uint32_t;
struct H {
    U (*rd32)(U va); void (*wr32)(U va, U v); U (*rd8)(U va);
    void (*fatal)(U line);                    // Ordinal_10265(line) + 10024(0x6FB8359F, s) + exit(-1); no return
    void (*obf8c)();                          // thunk 0xBF8C (Ordinal_10227)
    void (*send)(const uint8_t* buf, U len);  // 0x143E0
    U (*tick)();                              // GetTickCount
    void (*fc2790)(U ecx, U edx, U s0);       // 0xC2790 (ECX = entry index, EDX = 1, stack 0)
    void (*f47b20)();                         // 0x47B20
    U (*fa5440)(U eax);                       // 0xA5440 (EAX = player unit)
    U (*od0b4)();                             // thunk 0xD0B4 (Ordinal_10048)
    void (*f3cc60)(U a, U b);                 // 0x3CC60 (0x6FAECC60: EAX/ECX = sound object, then a name-table address)
    void (*f3ccc0)(U a, U b);                 // 0x3CCC0 (0x6FAECCC0, same shape)
    void (*fbfa10)();                         // 0xBFA10 (0x6FB6FA10)
    void (*f599e0)();                         // 0x599E0
    void (*f91070)(U s0);                     // 0x91070 (ret 4)
};

constexpr U kC2CC = 0x6FBCC2CC, kAADC = 0x6FBAADDC, kC3A68 = 0x6FBC3A68, kD8D0 = 0x6FB8D5D0, kEntry = 0x6FBAAD80,
            kEntryPtr = 0x6FB8D538, kBBFC = 0x6FBCBBFC, kC9721 = 0x6FBC9721, kD349C = 0x6FBD349C, kD3498 = 0x6FBD3498,
            kC434 = 0x6FBCC434, kC77FC = 0x6FBC77FC, kC77EC = 0x6FBC77EC, kC2D0 = 0x6FBCC2D0, kB348 = 0x6FBBB348,
            kBE74 = 0x6FBCBE74, kC77F4 = 0x6FBC77F4, kC77F8 = 0x6FBC77F8;

// 0x59650: the explicit leave. Sends 4F 02 00 00 00 00 00 and enters exit state 7 (stamping the tick once).
static void leave(const H& h) {
    const uint8_t pkt[7] = {0x4F, 0x02, 0, 0, 0, 0, 0};
    h.send(pkt, 7);
    if (h.rd32(kC2CC) != 7) { h.wr32(kC77F8, 1); h.wr32(kC77F4, h.tick()); h.wr32(kC2CC, 7); }
}

static void tail(const H& h) { h.wr32(kC2CC, 1); h.f599e0(); h.f91070(0); }

static void teardown(const H& h) {
    h.wr32(kC434, 0);
    if (h.rd32(kD349C) == 1) {
        h.wr32(kD349C, 0);
        h.f3cc60(h.od0b4(), 0x6FBA51BC);
        if (h.rd32(kD3498) == 0) { h.f3ccc0(h.od0b4(), 0x6FBA51EC); h.wr32(kD3498, 1); }
    }
    h.wr32(kC77FC, 0); h.wr32(kC77EC, 0); h.wr32(kC2D0, 1); h.wr32(kB348, 0);
    h.fbfa10();
    h.f3ccc0(h.od0b4(), 0x6FB8BD18);
}

// returns 1 when the case was handled here
U sub_AF930(const H& h, U ecx) {
    if (h.rd8(ecx + 1) != 0) return 0;
    if (h.rd32(kC2CC) != 0) { leave(h); return 1; }
    if (h.rd32(kAADC) == 0) {
        h.obf8c();
        if (h.rd32(kD8D0) != 0x26) h.fatal(0x139B);
        if (h.rd32(kC3A68) != 0) { leave(h); return 1; }
        for (U i = 0; (int32_t)i < (int32_t)h.rd32(kD8D0); i++) {
            if (h.rd32(kEntry + i * 4) != 1) continue;
            const U kind = h.rd32(h.rd32(kEntryPtr + i * 4) + 0x5C);
            if (kind == 0) continue;
            if (kind == 1) { h.fc2790(i, 1, 0); continue; }
            if (kind == 4) { if (h.rd8(kC9721) != 0) h.f47b20(); continue; }
            leave(h); return 1;
        }
        const U pl = h.rd32(kBBFC);
        if (!pl || (h.rd32(pl) == 0 && h.fa5440(pl) == 0)) { h.wr32(kAADC, 1); teardown(h); }
        else if (h.rd32(kAADC) != 0) teardown(h);
        h.wr32(kBE74, 0); h.wr32(kC77F4, 0xFFFFFFFFu);
    }
    tail(h);
    return 1;
}
}  // namespace d2client_af930

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_af930(const d2client_af930::H* h, uint32_t ecx) { return d2client_af930::sub_AF930(*h, ecx); }
