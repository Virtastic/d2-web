// 1.13c D2Multi+0x10AA0 env form. No arguments. Walks the list headed by the first global (linked through +0xB0,
// handle at +0x9C) and the list headed by the second global (linked through +0x44), clears the counters, rebuilds the
// caption from DAT_6fa08d48 and calls 0x104D0 with zeros. The callees and the globals arrive in the env so the FUNCTION-mode
// probe drives them; production is d2_multi_sub_10aa0 in d2multi_batch_22.cpp.
// The list heads and the links are retail 4-byte guest words: they are read and written as uint32.
#include <cstdint>
#include <cstdio>

struct D2m10aa0Env {
    uint32_t* g_ef4;       // DAT_6fa09ef4 (list one head, guest word)
    uint32_t* g_ef8;       // DAT_6fa09ef8 (list two head, guest word)
    int32_t*  g_efc;       // DAT_6fa09efc
    int32_t*  g_f00;       // DAT_6fa09f00
    int32_t*  g_f04;       // DAT_6fa09f04
    const char* chan;      // DAT_6fa08d48 (caption format name)
    void (*e860)(void);
    void (*ord10054)(int32_t, int32_t);
    void (*ord10043)(int32_t, int32_t);
    void (*fb40)(const char*);
    void (*sub104d0)(int32_t, int32_t);
};

static uint8_t* ptr(uint32_t w) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(w)); }

extern "C" void d2multi_10aa0_env(const D2m10aa0Env* e) {
    e->e860();
    uint32_t n = *e->g_ef4;
    while (n) {
        uint8_t* node = ptr(n);
        uint32_t next = *reinterpret_cast<uint32_t*>(node + 0xb0);
        e->ord10054(*(int32_t*)(node + 0x9c), 1);
        *(int32_t*)(node + 0x9c) = 0;
        e->ord10043(0x814, 0);
        n = next;
    }
    *e->g_ef4 = 0;
    uint32_t m = *e->g_ef8;
    while (m) {
        m = *reinterpret_cast<uint32_t*>(ptr(m) + 0x44);
        e->ord10043(0x81c, 0);
    }
    *e->g_ef8 = 0;
    *e->g_efc = 0;
    *e->g_f00 = 0;
    char caption[256];
    std::snprintf(caption, sizeof caption, "%s (%d)", e->chan, 0);
    e->fb40(caption);
    *e->g_f04 = 0;
    e->sub104d0(0, 0);
}
