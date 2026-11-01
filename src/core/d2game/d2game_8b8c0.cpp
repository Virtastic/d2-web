// 1.13c D2Game+0x8B8C0 -- the 0x18 client-update packet, written bit by bit and queued to the client in EAX.
// Prior art used: none (not in D2MOO 1.10f); the retail disassembly. The function has 9 stack arguments after the
// register client; the earlier transcription from the decompile read them as uninitialised locals.
#include <cstdint>
#include <emscripten.h>
#include <D2BitManip.h>
#include <GAME/SCmd.h>

// EAX = client; stack: a1 (15 bits), a2 (15 bits), a3 (15 bits), a4 (7 bits), a5 (7 bits), a6 (16 bits), a7 (16 bits), a8 (8 bits), a9 (8 bits). Fields in order: 0x18 (8), a1 (15), a2 (15), a3 (15), a4 (7), a5 (7), a6 (16), a7 (16), a8 (8), a9 (8). Packet buffer 15 bytes; the length is the writer's byte count.
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_8b8c0(D2ClientStrc* client, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9)
{ // 1.13c D2Game+0x8B8C0
    uint8_t packet[15] = {};
    D2BitBufferStrc bb = {};
    BITMANIP_Initialize(&bb, packet, sizeof(packet));
    BITMANIP_Write(&bb, 0x18, 8);
    BITMANIP_Write(&bb, a1 & 0xFFFF, 15);
    BITMANIP_Write(&bb, a2 & 0xFFFF, 15);
    BITMANIP_Write(&bb, a3 & 0xFFFF, 15);
    BITMANIP_Write(&bb, a4 & 0xFF, 7);
    BITMANIP_Write(&bb, a5 & 0xFF, 7);
    BITMANIP_Write(&bb, a6 & 0xFFFF, 16);
    BITMANIP_Write(&bb, a7 & 0xFFFF, 16);
    BITMANIP_Write(&bb, a8 & 0xFF, 8);
    BITMANIP_Write(&bb, a9 & 0xFF, 8);
    D2GAME_PACKETS_SendPacket_6FC3C710(client, packet, (int32_t)BITMANIP_GetSize(&bb));
}
