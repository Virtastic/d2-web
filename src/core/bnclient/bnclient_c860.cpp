// 1.13c BNClient+0x0C860 -- change-password frame id 0x31: {tick, srv, H(tick, srv, H(lc(old))) [20], H(lc(new)) [20]} + name + NUL.
// Storm string copy, CharLowerBuffA, GetTickCount and the frame sender (0xEB00) are injected; the hash is ours (0x172C0).
#include <cstdint>
#include <cstring>
namespace d2::bnclient::grid { void broken_hash(uint32_t* output, const void* input, uint32_t length); }
struct BnC860Env {
    uint32_t (*strcopy)(char* dst, const char* src, uint32_t cap);
    void (*lower)(char* buf, uint32_t len);
    uint32_t (*tick)();
    void (*send)(uint32_t len, uint8_t id, const uint8_t* payload);
};
extern "C" uint32_t bnclient_c860(const BnC860Env* e, const char* oldPw, const char* name, const char* newPw, uint32_t srv) {
    uint8_t pl[0x30 + 0x101], in[0x1c]; char a[0x100];
    const uint32_t tick = e->tick();
    uint32_t n = e->strcopy(a, oldPw, 0x100);
    e->lower(a, n);
    memcpy(in, &tick, 4); memcpy(in + 4, &srv, 4);
    d2::bnclient::grid::broken_hash((uint32_t*)(in + 8), a, n);
    memcpy(pl, &tick, 4); memcpy(pl + 4, &srv, 4);
    d2::bnclient::grid::broken_hash((uint32_t*)(pl + 8), in, 0x1c);
    n = e->strcopy(a, newPw, 0x100);
    e->lower(a, n);
    d2::bnclient::grid::broken_hash((uint32_t*)(pl + 0x1c), a, n);
    const uint32_t m = e->strcopy((char*)pl + 0x30, name, 0x100);
    e->send(m + 1 + 0x30, 0x31, pl);
    return 1;
}
