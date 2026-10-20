// Builds and sends the 0x25-byte client packet 0x68 and bumps the two send counters.
#include <cstddef>
#include <cstdint>
#include <cstring>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

struct D2ClientPacket68Services {
    uint32_t (*vtbl5c)();                                      // [DAT_6fbcc378+0x5c]()
    uint32_t (*is_expansion)();                                // thunk 0xBF8C, Fog #10227
    uint8_t (*o10009)();                                       // thunk 0xD1E6
    void (*str_copy)(char* dst, const char* src, uint32_t n);  // thunk 0xBE96, Storm #501
    void (*send)(uint32_t len, uint32_t flags, const void* p); // thunk 0xD252
};
#pragma pack(push, 1)
struct D2ClientPacket68 {
    uint8_t id;
    uint32_t eax;
    uint16_t cx;
    uint8_t flagbyte;
    uint32_t v5c, key0, key1;
    uint8_t b14;
    char name[16];
};
#pragma pack(pop)
static_assert(sizeof(D2ClientPacket68) == 0x25);
static_assert(offsetof(D2ClientPacket68, name) == 0x15);

// 1.13c D2Client+0x14A50
// eax/cx are register args. flags = DAT 6fb9264c, byte_a = 6fb92650, byte_b = 6fbc991c,
// src = 6fbc9928, bytes_sent = 6fbcc4e0, packets_sent = 6fbcc4e8.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_14a50(uint32_t eax, uint16_t cx, uint32_t flags, uint8_t byte_a,
    uint8_t byte_b, const char* src, uint32_t* bytes_sent, uint32_t* packets_sent, const D2ClientPacket68Services* api) {
    D2ClientPacket68 p;
    memset(&p, 0, sizeof p); // retail leaves the unwritten stack bytes undefined
    p.id = 0x68; p.eax = eax; p.cx = cx;
    p.v5c = api->vtbl5c();
    p.key0 = (api->is_expansion() ? 0xcbd7de7au : 0u) + 0x2185edd6u;
    p.key1 = 0x91a519b6u;
    p.b14 = api->o10009();
    p.flagbyte = (flags & 0x100) ? byte_a : byte_b;
    api->str_copy(p.name, src, 0x7fffffff);
    api->send(0x25, 0, &p);
    *bytes_sent += 0x25;
    *packets_sent += 1;
}
