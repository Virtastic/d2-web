// Party event packet: a zeroed 40-byte packet {0x5A, 0x07, sub, unit id (dword), kind (byte)} handed to the client
// sender with value v. sub follows the low byte of kind: 1 and 2 give 4, 3 gives 8, 4 gives 0, 5, 7 and 8 give 2,
// 6 and 9 to 11 give 9, anything else 3.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GamePartyEventEnv {
    void (*send)(const uint8_t* packet, uint32_t v);
};

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_PartyEventPacket(uint32_t id, uint32_t v, uint32_t kind, const D2GamePartyEventEnv* e)
{
    static const uint8_t kSub[11] = { 4, 4, 8, 0, 2, 9, 2, 2, 9, 9, 9 };
    const uint8_t k = (uint8_t)kind;
    uint8_t pkt[40] = {};
    pkt[0] = 0x5A;
    pkt[1] = 0x07;
    pkt[2] = (k >= 1 && k <= 11) ? kSub[k - 1] : 3;
    std::memcpy(pkt + 3, &id, 4);
    pkt[7] = k;
    e->send(pkt, v);
}
