// 1.13c D2Game+0x8C420: 8-byte join-game body. EAX = the client struct whose fields are read, CL = header byte,
// the stack word = the client the packet goes to (ret 4). A null pGameData at +0x1D28 is the 0x139 fatal. Byte 0 = CL,
// byte 1 = [client+0x6D], bytes 2..5 = [pGameData+8], byte 6 = (dword [client+0x70] != 0), byte 7 = (dword [client+0x74] != 0).
#include <cstdint>
#include <cstring>
typedef void (*D2GSend)(void* client, const void* buf, uint32_t len);

void D2Game_8C420_PacketSendJoinBody(D2GSend send, void (*fatal)(uint32_t), const uint8_t* st, uint8_t cl, void* sendClient)
{
    const uint8_t* gd;
    std::memcpy(&gd, st + 0x1D28, sizeof gd);   // engine pointer (retail: 4 bytes)
    if (gd == nullptr) fatal(0x139);
    uint32_t v8, d70, d74;
    std::memcpy(&v8, gd + 8, 4);
    std::memcpy(&d70, st + 0x70, 4);
    std::memcpy(&d74, st + 0x74, 4);
    uint8_t b[8];
    b[0] = cl;
    b[1] = st[0x6D];
    std::memcpy(b + 2, &v8, 4);
    b[6] = d70 != 0;
    b[7] = d74 != 0;
    send(sendClient, b, 8);
}
