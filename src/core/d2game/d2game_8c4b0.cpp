// 1.13c D2Game+0x8C4B0: five-byte packet {0xB4, EAX dword} delivered on channel 0 to the client id in EDX through the
// Net ordinal 0xA35E (4 stack words: 0, id, buf, 5). Bytes 5..7 of the stack buffer are never written.
#include <cstdint>
#include <cstring>
typedef void (*D2GDeliver)(uint32_t zero, uint32_t id, const void* buf, uint32_t len);

void D2Game_8C4B0_PacketSendB4(D2GDeliver deliver, uint32_t eax, uint32_t edx)
{
    uint8_t b[8] = { 0xB4 };
    std::memcpy(b + 1, &eax, 4);
    deliver(0, edx, b, 5);
}
