// 1.13c D2Game+0x8C230: 16-byte packet to the client in the first stack word (ret 0x18). Byte 0 = AL, byte 1 = CL,
// bytes 2..5 = EDX, byte 6 and byte 7 = the next two stack bytes, dword at 8, word at 12, word at 14.
#include <cstdint>
#include <cstring>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8C230_PacketSend10(D2GSend send, void* client, uint8_t al, uint8_t cl, uint32_t edx, uint8_t b6, uint8_t b7,
                               uint32_t d8, uint16_t w12, uint16_t w14)
{
    uint8_t b[16];
    b[0] = al;
    b[1] = cl;
    std::memcpy(b + 2, &edx, 4);
    b[6] = b6;
    b[7] = b7;
    std::memcpy(b + 8, &d8, 4);
    std::memcpy(b + 12, &w12, 2);
    std::memcpy(b + 14, &w14, 2);
    send(client, b, 16);
}
