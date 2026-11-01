// 1.13c D2Game+0x8B7F0 -- the 0x95 client-update packet, written bit by bit and queued to the client in EAX.
// Prior art used: none (not in D2MOO 1.10f); the retail disassembly. The function has 7 stack arguments after the
// register client; the earlier transcription from the decompile read them as uninitialised locals.
#include <cstdint>
#include <emscripten.h>
#include <D2BitManip.h>
#include <GAME/SCmd.h>

// EAX = client; stack: a1 (15 bits), a2 (15 bits), a3 (15 bits), a4 (16 bits), a5 (16 bits), a6 (8 bits), a7 (8 bits). Fields in order: 0x95 (8), a1 (15), a2 (15), a3 (15), a4 (16), a5 (16), a6 (8), a7 (8). Packet buffer 13 bytes; the length is the writer's byte count.
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_8b7f0(D2ClientStrc* client, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7)
{ // 1.13c D2Game+0x8B7F0
    uint8_t packet[13] = {};
    D2BitBufferStrc bb = {};
    BITMANIP_Initialize(&bb, packet, sizeof(packet));
    BITMANIP_Write(&bb, 0x95, 8);
    BITMANIP_Write(&bb, a1 & 0xFFFF, 15);
    BITMANIP_Write(&bb, a2 & 0xFFFF, 15);
    BITMANIP_Write(&bb, a3 & 0xFFFF, 15);
    BITMANIP_Write(&bb, a4 & 0xFFFF, 16);
    BITMANIP_Write(&bb, a5 & 0xFFFF, 16);
    BITMANIP_Write(&bb, a6 & 0xFF, 8);
    BITMANIP_Write(&bb, a7 & 0xFF, 8);
    D2GAME_PACKETS_SendPacket_6FC3C710(client, packet, (int32_t)BITMANIP_GetSize(&bb));
}
