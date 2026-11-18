// 1.13c BNClient+0xBFD0 (ECX = realm title, EDX = realm password): compose and send frame 0x3E (LOGONREALMEX).
// A null title returns 0 at once. Otherwise the client token is ++DAT_3F87C (stored back), the hash input is
// {token, DAT_3F874 (server token), H(password, strlen)} (0x1C bytes), and the payload is {token, H(input)[20],
// SStrCopy(title, 0x100) + NUL}, length strlen(title) + 0x19. DAT_3EE50 and DAT_3EFD0 are cleared, then 0xEB00 sends
// it with id 0x3E. A negative send returns 0; otherwise 0x30D90(0x3E) waits for the reply and DAT_3F88C is returned.
// The password is hashed as given (no lowercasing, unlike 0xC9E0). H is 0x372C0, ours.
#include <cstdint>
#include <cstring>
namespace d2::bnclient::grid { void broken_hash(uint32_t* output, const void* input, uint32_t length); }
struct BnBfd0Env {
    uint32_t (*strcopy)(char* dst, const char* src, uint32_t cap);      // Storm SStrCopy (0x804E)
    int32_t (*send)(uint32_t len, uint8_t id, const uint8_t* payload);  // 0xEB00 (ECX len, EDI payload, id on the stack)
    void (*wait)(uint32_t id);                                         // 0x30D90
};
struct BnBfd0Globals { uint32_t* g3f874; uint32_t* g3f87c; uint32_t* g3ee50; uint32_t* g3efd0; const uint32_t* g3f88c; };

// 1.13c BNClient+0xBFD0
extern "C" uint32_t bnclient_bfd0(const BnBfd0Env* e, const BnBfd0Globals* g, const char* title, const char* password) {
    if (!title) return 0;
    alignas(4) uint8_t in[0x1c], pl[0x18 + 0x100];
    const uint32_t srv = *g->g3f874;
    const uint32_t token = ++*g->g3f87c;
    memcpy(in, &token, 4); memcpy(in + 4, &srv, 4);
    d2::bnclient::grid::broken_hash((uint32_t*)(in + 8), password, (uint32_t)strlen(password));
    memcpy(pl, &token, 4);
    d2::bnclient::grid::broken_hash((uint32_t*)(pl + 4), in, 0x1c);
    const uint32_t n = e->strcopy((char*)pl + 0x18, title, 0x100);
    *g->g3ee50 = 0; *g->g3efd0 = 0;
    if (e->send(n + 1 + 0x18, 0x3E, pl) < 0) return 0;
    e->wait(0x3E);
    return *g->g3f88c;
}
