// D2Client npcTradeOpen_4AEE0: the NPC menu "Trade" / "Trade-Repair"
// callback. Opens the merchant panel (UI index 12) locally and sends C->S 0x38 {1, npc guid, 0}. Ghidra made no
// function here; this follows the disassembly 0x6FAFAEE0-0x6FAFB218 (switch table 0x6FAFB21C). Globals are retail
// VAs read through H; register arguments are spelled out per callee.
#include <cstdint>
#include <emscripten.h>

namespace d2client_4aee0 {
using U = uint32_t;
struct H {
    U (*rd32)(U va); void (*wr32)(U va, U v); void (*wr16)(U va, uint16_t v);
    void (*f88550)(U node, U rec, U id);      // 0x88550 (EAX = list node, ECX = node[0], EDX = [0x6FBC97B0])
    void (*fa3d20)();                         // 0xA3D20
    void (*f48150)(U edi);                    // 0x48150 (EDI = 1)
    void (*fatal)(U line);                    // Ordinal_10265(line) + 10024(0x6FB8359F, s) + exit(-1); no return
    void (*obf8c)();                          // thunk 0xBF8C
    void (*fc2790)(U ecx, U edx, U s0);       // 0xC2790 (ECX = window, EDX = 1, stack 0)
    void (*f47b20)(U edi);                    // 0x47B20 (EDI = npc guid)
    U (*fa5440)(U eax);                       // 0xA5440 (EAX = player unit)
    U (*od0b4)();                             // thunk 0xD0B4 (no arguments; returns the sound object)
    void (*f3cc60)(U eax, U s0, U s1);        // 0x3CC60 (EAX = count, stack = 0xD0B4 result, then the name-table address; ret 8)
    void (*f3ccc0)(U eax, U s0, U s1);        // 0x3CCC0 (same shape as 0x3CC60)
    void (*f911a0)();                         // 0x911A0 shop initialiser
    void (*fbfa10)(U edi);                    // 0xBFA10 (EDI = 0x6FB805E0)
    void (*f492f0)();                         // 0x492F0
    void (*od1b0)(U ecx);                     // thunk 0xD1B0 (ECX = 0)
    void (*f8a4d0)(U eax, U ecx, U edx, U esi, U s0);  // 0x8A4D0 (EAX = [0x6FBA9E18], ECX = [0x6FBA9E14], EDX = 0, ESI = [0x6FBCC3C4], stack 0)
    void (*send)(const uint8_t* buf, U len);  // 0x143E0 (stack buf, EBX = len)
};

static void sounds(const H& h) {  // shared by both open paths (0x6FAFB077 / 0x6FAFB110)
    if (h.rd32(0x6FBD349C) != 0) return;
    h.wr32(0x6FBD349C, 1);
    if (h.rd32(0x6FBD3498) != 0) { h.f3cc60(2, h.od0b4(), 0x6FBA51EC); h.wr32(0x6FBD3498, 0); }
    h.f3ccc0(4, h.od0b4(), 0x6FBA51BC);
}
static void layout(const H& h, U mode) {
    h.wr32(0x6FBCC418, 0); h.wr32(0x6FBCC414, mode);
    h.od1b0(0);
    h.f8a4d0(h.rd32(0x6FBA9E18), h.rd32(0x6FBA9E14), 0, h.rd32(0x6FBCC3C4), 0);
}

// 1.13c D2Client+0x4AEE0
U sub_4AEE0(const H& h) {
    if (h.rd32(0x6FBC97AC) != 0) {
        const U id = h.rd32(0x6FBC97B0);
        if (id != 0) {
            for (U n = h.rd32(0x6FBCBF30); n; n = h.rd32(n + 4)) {
                const U rec = h.rd32(n);
                if (h.rd32(rec + 8) == id) { h.f88550(n, rec, id); break; }
            }
            h.wr32(0x6FBC97B0, 0);
        }
        h.wr32(0x6FBC97AC, 0);
    }
    if (h.rd32(0x6FBC9763) != 0) { h.fa3d20(); h.wr32(0x6FBC9763, 0); }
    const U guid = h.rd32(0x6FBC971D);
    U u = h.rd32(0x6FBBA808 + (guid & 0x7F) * 4);
    while (u && h.rd32(u + 0xC) != guid) u = h.rd32(u + 0xE4);
    if (!u) { h.f48150(1); return 1; }
    if (h.rd32(u) != 1) h.fatal(0x5A);
    if (h.rd32(u + 0x60) == 0) { h.f48150(1); return 1; }

    const U old = h.rd32(0x6FBAADB0);
    h.wr32(0x6FBC9663, 3); h.wr32(0x6FBC97A8, 0);
    h.obf8c();
    if (h.rd32(0x6FB8D5D0) != 0x26) h.fatal(0x139B);
    if (h.rd32(0x6FBC3A68) == 0) {
        for (U i = 0; (int32_t)i < (int32_t)h.rd32(0x6FB8D5D0); i++) {
            if (h.rd32(0x6FBAAD80 + i * 4) != 1) continue;
            const U kind = h.rd32(h.rd32(0x6FB8D538 + i * 4) + 0x30);
            if (kind == 0) continue;
            if (kind == 1) { h.fc2790(i, 1, 0); continue; }
            if (kind == 4) { if (h.rd32(0x6FBC9721)) h.f47b20(h.rd32(0x6FBC971D)); continue; }
            goto send;  // kinds 2, 3 and > 4 leave the loop straight to the packet
        }
        const U pl = h.rd32(0x6FBCBBFC);
        U now;
        if (!pl || (h.rd32(pl) == 0 && h.fa5440(pl) == 0)) { h.wr32(0x6FBAADB0, 1); now = 1; }
        else now = h.rd32(0x6FBAADB0);
        if (old == 0) {
            if (now == 0) { layout(h, 0); goto send; }
            h.wr32(0x6FBCC434, 2);
            sounds(h);
            h.f911a0();
            h.fbfa10(0x6FB805E0);
            h.f3ccc0(8, h.od0b4(), 0x6FB8BC60);
        } else if (now == 0) {
            h.f3cc60(8, h.od0b4(), 0x6FB8BC60);
            h.f492f0();
            h.wr32(0x6FBCC434, 1);
            sounds(h);
        }
        if (now == 0) { layout(h, 0); goto send; }
        layout(h, 3);
        h.wr32(0x6FBCC3DC, 0); h.wr32(0x6FBCC3E0, 0); h.wr32(0x6FBCC3F8, 0); h.wr32(0x6FBCC3FC, 0);
    }
send:
    {
        uint8_t pkt[13] = {0x38, 1, 0, 0, 0};
        const U g = h.rd32(0x6FBC971D);
        for (int k = 0; k < 4; k++) { pkt[5 + k] = (uint8_t)(g >> (8 * k)); pkt[9 + k] = 0; }
        h.send(pkt, 13);
    }
    h.wr16(0x6FBCC33C, 0);
    h.wr32(0x6FBCC338, h.rd32(0x6FBCBBFC));
    return 1;
}
}  // namespace d2client_4aee0

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_4aee0(const d2client_4aee0::H* h) { return d2client_4aee0::sub_4AEE0(*h); }
