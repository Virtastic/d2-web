// 1.13c D2Game+0x8BF80: 11-byte packet to the client in the first stack word (ret 0x10). Byte 0 = AL, byte 1 = CL,
// bytes 2..5 = EDX, word at 6 = second stack word, word at 8 = third, byte 10 = fourth.
#include <cstdint>
#include <cstring>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8BF80_PacketSend0B(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx, uint16_t w6, uint16_t w8, uint8_t b10)
{
    uint8_t b[11];
    b[0] = al;
    b[1] = cl;
    std::memcpy(b + 2, &edx, 4);
    std::memcpy(b + 6, &w6, 2);
    std::memcpy(b + 8, &w8, 2);
    b[10] = b10;
    send(client, b, 11);
}
