// 1.13c D2Game+0x8C010: 16-byte packet (zero-filled first) to the client in the first stack word (ret 0x20).
// Byte 0 = CL, bytes 1..4 = EDX, byte 5 = stack byte, word 6, word 8, byte 10, byte 11, word 12, word 14.
#include <cstdint>
#include <cstring>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8C010_PacketSend10(D2GSend send, void* client, uint8_t cl, uint32_t edx, uint8_t s2, uint16_t s3, uint16_t s4,
                               uint8_t s5, uint8_t s6, uint16_t s7, uint16_t s8)
{
    uint8_t b[16] = {};
    b[0] = cl;
    std::memcpy(b + 1, &edx, 4);
    b[5] = s2;
    std::memcpy(b + 6, &s3, 2);
    std::memcpy(b + 8, &s4, 2);
    b[10] = s5;
    b[11] = s6;
    std::memcpy(b + 12, &s7, 2);
    std::memcpy(b + 14, &s8, 2);
    send(client, b, 16);
}
