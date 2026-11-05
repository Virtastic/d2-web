// Name packet: bytes 1..15 of the packet, NUL-terminated, are widened into the client name text (16 characters at most);
// the dword at byte 17 becomes the name flag.
#include <cstdint>
#include <cstring>
struct D2cABFD0Env {
    void (*widen)(void* ctx, uint16_t* dst, const char* src, int32_t n);
    uint16_t* text;
    uint32_t* flag;
    void* ctx;
};
extern "C" void d2c_abfd0(const uint8_t* pkt, const D2cABFD0Env* e) {
    char name[16];
    std::memcpy(name, pkt + 1, 15);
    name[15] = 0;
    e->widen(e->ctx, e->text, name, 0x10);
    std::memcpy(e->flag, pkt + 17, 4);
}
