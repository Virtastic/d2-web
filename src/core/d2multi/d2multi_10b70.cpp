// 1.13c D2Multi+0x10B70 env form. Cdecl void with one register input (EBX = the leaving player's name). Walks the channel
// list headed by the first global (linked through +0xB0, name at +4, a second name field at +0x36, handle at +0x9C), skipping
// a leading '*' in both names. On a match it prints the leave notice when the flag is set, unlinks the node, releases the
// handle and decrements the count, then prints the channel caption and calls 0x104D0 and F750. The globals and callees arrive
// in the env so the FUNCTION-mode probe drives them; production is d2_multi_sub_10b70 in d2multi_batch_22.cpp.
#include <cstdint>
#include <cstdio>
#include <cstring>

struct D2m10b70Env {
    uint8_t** g_ef4;       // DAT_6fa09ef4 (list head)
    int32_t*  g_f00;       // DAT_6fa09f00 (count)
    uint8_t** g_f08;       // DAT_6fa09f08 (node to close)
    int32_t*  g_a090;      // DAT_6fa0a090 (leave-notice flag)
    const char* chan;      // DAT_6fa08d48 (channel caption format name)
    void (*e860)(void);
    void (*ord10054)(int32_t, int32_t);
    void (*ord10043)(int32_t, int32_t);
    void (*fb40)(const char*);
    void (*fc90)(const char*, int32_t);
    void (*sub104d0)(int32_t, int32_t);
    void (*f750)(void);
};

static const char* skip_star(const char* s) {
    const char* p = std::strchr(s, '*');
    return p ? p + 1 : s;
}

extern "C" void d2multi_10b70_env(const D2m10b70Env* e, const char* name) {
    uint8_t* prev = nullptr;
    uint8_t* n = *e->g_ef4;
    const char* want = skip_star(name);
    while (n) {
        if (std::strcmp(skip_star((const char*)(n + 4)), want) == 0) break;
        prev = n;
        n = *(uint8_t**)(n + 0xb0);
    }
    if (n) {
        if (*e->g_a090 != 0) {
            const char* shown = n[0x36] != 0 ? (const char*)(n + 0x36) : (const char*)(n + 4);
            char notice[256];
            std::snprintf(notice, sizeof notice, "%s has left the channel.", shown);
            e->fc90(notice, 5);
        }
        if (prev == nullptr)
            *e->g_ef4 = *(uint8_t**)(n + 0xb0);
        else
            *(uint8_t**)(prev + 0xb0) = *(uint8_t**)(n + 0xb0);
        if (*e->g_f08 == n)
            e->e860();
        e->ord10054(*(int32_t*)(n + 0x9c), 1);
        *(int32_t*)(n + 0x9c) = 0;
        e->ord10043(0x7f0, 0);
    }
    *e->g_f00 -= 1;
    char caption[256];
    std::snprintf(caption, sizeof caption, "%s (%d)", e->chan, (int)*e->g_f00);
    e->fb40(caption);
    e->sub104d0(0, 0);
    e->f750();
}
