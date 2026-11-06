// 1.13c BNClient+0x0C960 -- create-account frame id 0x3D: broken_hash(lowercase(password)) [20] + name + NUL. The Storm string copy,
// CharLowerBuffA and the frame sender (0xEB00) are injected; the hash is ours (0x172C0).
#include <cstdint>
#include <cstring>
namespace d2::bnclient::grid { void broken_hash(uint32_t* output, const void* input, uint32_t length); }
struct BnC960Env {
    uint32_t (*strcopy)(char* dst, const char* src, uint32_t cap);   // SStrCopy, returns the length
    void (*lower)(char* buf, uint32_t len);                          // CharLowerBuffA
    void (*send)(uint32_t len, uint8_t id, const uint8_t* payload);
};
extern "C" uint32_t bnclient_c960(const BnC960Env* e, const char* password, const char* name) {
    uint8_t pl[0x14 + 0x101]; char a[0x100];
    const uint32_t n = e->strcopy(a, password, 0x100);
    e->lower(a, n);
    d2::bnclient::grid::broken_hash((uint32_t*)pl, a, n);
    const uint32_t m = e->strcopy((char*)pl + 0x14, name, 0x100);
    e->send(m + 1 + 0x14, 0x3D, pl);
    return 1;
}
