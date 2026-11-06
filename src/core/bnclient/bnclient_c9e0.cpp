// 1.13c BNClient+0x0C9E0 -- logon-response frame id 0x3A: clears DAT_3ee40/DAT_3efc0, then {tick, srv, H(tick, srv, H(lc(password)))} + name + NUL.
// Storm string copy, CharLowerBuffA, GetTickCount and the frame sender (0xEB00) are injected; the hash is ours (0x172C0).
#include <cstdint>
#include <cstring>
namespace d2::bnclient::grid { void broken_hash(uint32_t* output, const void* input, uint32_t length); }
struct BnC9e0Env {
    uint32_t (*strcopy)(char* dst, const char* src, uint32_t cap);
    void (*lower)(char* buf, uint32_t len);
    uint32_t (*tick)();
    void (*send)(uint32_t len, uint8_t id, const uint8_t* payload);
};
extern "C" uint32_t bnclient_c9e0(const BnC9e0Env* e, const char* name, const char* password, uint32_t srv, uint32_t* g3ee40, uint32_t* g3efc0) {
    *g3ee40 = 0; *g3efc0 = 0;
    uint8_t pl[0x1c + 0x101], in[0x1c]; char a[0x100];
    const uint32_t tick = e->tick();
    const uint32_t n = e->strcopy(a, password, 0x100);
    e->lower(a, n);
    memcpy(in, &tick, 4); memcpy(in + 4, &srv, 4);
    d2::bnclient::grid::broken_hash((uint32_t*)(in + 8), a, n);
    memcpy(pl, &tick, 4); memcpy(pl + 4, &srv, 4);
    d2::bnclient::grid::broken_hash((uint32_t*)(pl + 8), in, 0x1c);
    const uint32_t m = e->strcopy((char*)pl + 0x1c, name, 0x100);
    e->send(m + 1 + 0x1c, 0x3A, pl);
    return 1;
}
