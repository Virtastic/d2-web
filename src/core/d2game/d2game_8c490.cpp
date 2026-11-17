// Stand-alone body of D2Game 0x8C490 (the marker is on the play-path D2GAME_PACKETS_SendHeaderOnlyPacket): one-byte packet {AL} to the client in the stack word (ret 4).
#include <cstdint>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8C490_PacketSend01(D2GSend send, void* client, uint8_t al)
{
    send(client, &al, 1);
}
