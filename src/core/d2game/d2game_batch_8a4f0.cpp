// 1.13c D2Game packet builders 0x77D90, 0x8A4F0, 0x8A570, 0x8A5A0, 0x8A5D0, 0x8A700, 0x8A7E0.
// Plain transcription of the cached decompiles. Each builds a short packet on the stack and hands it
// to the 1.13c send 0x8A3E0 (FUN_6fcaa3e0), which takes the client as the first argument here (in
// retail the client is register-passed and not visible in these decompiles). Uninitialised stack
// bytes of each packet are zeroed. The fatal length branches (Ordinal_10265 + log + _exit) are not
// transcribed: they return without sending.
#include <cstdint>
#include <cstring>

#define EXT(n) extern "C" __attribute__((weak)) intptr_t n(...)
// 1.13c 0x8A3E0: append one outgoing packet to the client queue.
EXT(FUN_6fcaa3e0);
EXT(Ordinal_10920); EXT(Ordinal_10466); EXT(Ordinal_10892); EXT(d2game_8a4f0_ord10050); EXT(d2game_8a4f0_ord10201);

// 1.13c D2Game+0x77D90 (PACKET_SendUnitRemove0x42): six-byte removal packet 0x42, unit type, unit id;
// then the room helper on unit+0x60 and the unit cleanup calls. unit is the register-passed unit (esi).
extern "C" void d2game_77d90(void* client, int32_t* unit)
{
    int32_t room = unit[0x18];                      // unit+0x60
    if (unit[0] == 0) Ordinal_10920();
    uint8_t pkt[6] = {};
    pkt[0] = 0x42;
    pkt[1] = (uint8_t)unit[0];                       // unit type +0x00
    memcpy(pkt + 2, &unit[3], 4);                    // unit id +0x0C as two 16-bit writes
    FUN_6fcaa3e0(client, pkt, 6);
    Ordinal_10466((intptr_t)room, 0);
    Ordinal_10892();
    d2game_8a4f0_ord10050((intptr_t)room);
    d2game_8a4f0_ord10201();
}

// 1.13c D2Game+0x8A4F0: variable raw packet 0xAE: [0]=0xAE, [1..2]=length, then n data bytes.
// Valid for 0 <= n < 0x1FE; the out-of-range fatal branch is not transcribed (returns 0).
extern "C" uint32_t d2game_8a4f0(void* client, uint32_t n, const void* data)
{
    if ((int32_t)n < 0 || (int32_t)n >= 0x1FE) return 0;
    uint8_t buf[0x203] = {};
    buf[0] = 0xAE;
    uint16_t len = (uint16_t)n;
    memcpy(buf + 1, &len, 2);
    memcpy(buf + 3, data, n);
    FUN_6fcaa3e0(client, buf, n + 3);
    return 1;
}

// 1.13c D2Game+0x8A570: seven-byte packet 0xA9, byte at [6].
extern "C" void d2game_8a570(void* client, uint8_t b)
{
    uint8_t pkt[7] = {};
    pkt[0] = 0xA9;
    pkt[6] = b;
    FUN_6fcaa3e0(client, pkt, 7);
}

// 1.13c D2Game+0x8A5A0: seven-byte packet 0xA7, byte at [6].
extern "C" void d2game_8a5a0(void* client, uint8_t b)
{
    uint8_t pkt[7] = {};
    pkt[0] = 0xA7;
    pkt[6] = b;
    FUN_6fcaa3e0(client, pkt, 7);
}

// 1.13c D2Game+0x8A5D0: variable packet 0xA6: [0]=0xA6, [1]=flag, [2..3]=len+6, [4..5]=tag, then len
// bytes from src at [6]. Total sent is len+6; the len+6 > 0xFFFE fatal branch is not transcribed.
// The third stack word of the decompile (param_3) is unused and not passed.
extern "C" void d2game_8a5d0(void* client, uint32_t len, uint16_t tag, uint8_t flag, const void* src)
{
    uint32_t total = len + 6;
    if ((int32_t)total > 0xFFFE) return;
    uint8_t buf[0x10006] = {};
    buf[0] = 0xA6;
    buf[1] = flag;
    uint16_t t16 = (uint16_t)total;
    memcpy(buf + 2, &t16, 2);
    memcpy(buf + 4, &tag, 2);
    memcpy(buf + 6, src, len);
    FUN_6fcaa3e0(client, buf, total);
}

// 1.13c D2Game+0x8A700: copies a 26-byte packet from src (6 dwords and a word) and sends it.
extern "C" void d2game_8a700(void* client, const void* src)
{
    uint8_t pkt[0x1A];
    memcpy(pkt, src, sizeof pkt);
    FUN_6fcaa3e0(client, pkt, 0x1A);
}

// 1.13c D2Game+0x8A7E0: six-byte packet 0x8B, rest zero.
extern "C" void d2game_8a7e0(void* client)
{
    uint8_t pkt[6] = {};
    pkt[0] = 0x8B;
    FUN_6fcaa3e0(client, pkt, 6);
}
