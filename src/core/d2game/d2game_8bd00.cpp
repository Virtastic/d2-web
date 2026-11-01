// 1.13c D2Game+0x8BD00: attribute-by-value packet to one client. EAX = client, ECX = value, DX = attribute id.
// The frame is [opcode][attribute byte][value]: 0x1D byte value (len 3), 0x1E word (len 4), 0x1F dword (len 6).
// An attribute id of 0xFF or more is the 0x3CB fatal (Ordinal 10265/10024 then _exit(-1)); `fatal` models it.
#include <cstdint>
#include <cstring>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8BD00_SendAttrByValue(D2GSend send, void (*fatal)(uint32_t), void* client, uint32_t value, uint16_t attr)
{
    if (attr >= 0xFF) fatal(0x3CB);
    uint8_t b[6];
    b[1] = (uint8_t)attr;
    if (value < 0xFF) { b[0] = 0x1D; b[2] = (uint8_t)value; send(client, b, 3); }
    else if (value < 0xFFFF) { b[0] = 0x1E; std::memcpy(b + 2, &value, 2); send(client, b, 4); }
    else { b[0] = 0x1F; std::memcpy(b + 2, &value, 4); send(client, b, 6); }
}
