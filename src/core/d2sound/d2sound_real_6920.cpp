#include <cstdint>
#include <cstring>
#include <unistd.h>
extern "C" uint32_t Ordinal_10265(uint32_t);
extern "C" void Ordinal_10024(const void*, uint32_t);
static const char g_msg_0684[1] = {0};  // DAT_6f9c0684
[[noreturn]] static void d2sound_die(uint32_t id) { Ordinal_10024(g_msg_0684, Ordinal_10265(id)); _exit(-1); }

// 1.13c D2sound+0x6920 -- returns obj[0x38]; fatal 0x2e4 on null.
extern "C" uint32_t D2sound_6920(const uint8_t* obj) {
    if (!obj) d2sound_die(0x2e4);
    uint32_t v; std::memcpy(&v, obj + 0x38, 4); return v;
}

// 1.13c D2sound+0x68D0 -- if obj[0x34]&2, call vtbl slot 0x4c of the COM object at obj[0xc] with (a,b,c,0); fatal 0x302 if obj null or obj[0]==0.
// The COM pointer/vtable are retail 32-bit words, so the call goes through the caller-supplied thunk type.
typedef void (*D2soundComCall4c)(void* com, uint32_t a, uint32_t b, uint32_t c, uint32_t zero);
extern "C" void D2sound_68D0(const uint8_t* obj, uint32_t a, uint32_t b, uint32_t c, D2soundComCall4c callVt4c) {
    uint32_t w0; if (obj) std::memcpy(&w0, obj, 4);
    if (!obj || !w0) d2sound_die(0x302);
    if (obj[0x34] & 2) { void* com; std::memcpy(&com, obj + 0xc, sizeof com); callVt4c(com, a, b, c, 0); }
}
