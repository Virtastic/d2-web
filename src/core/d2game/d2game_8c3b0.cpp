// 1.13c D2Game+0x8C3B0: 6-byte packet to the client in the stack word (ret 4): byte 0 = AL, byte 1 = CL, bytes 2..5 = EDX.
#include <cstdint>
#include <cstring>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8C3B0_PacketSend06(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx)
{
    uint8_t b[6];
    b[0] = al;
    b[1] = cl;
    std::memcpy(b + 2, &edx, 4);
    send(client, b, 6);
}
