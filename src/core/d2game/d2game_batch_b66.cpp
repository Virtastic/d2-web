// src/core/d2game/d2game_batch_b66.cpp -- eight 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles. Batch lane: implementation + marker only. Records are opaque, read through retail
// byte offsets; callees outside the batch are seams. Critical sections are no-ops; the fatal path
// (Ordinal_10265 -> Ordinal_10024 -> _exit(-1)) is modelled as _exit(-1).
// Skipped: 0xE8B60/0xE8FE0 (hidden register state), 0xF5A8D (SEH/CRT), 0x2B900 (no decompile cached).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <unistd.h>

namespace {
[[noreturn]] void fatal(uint32_t) { _exit(-1); }
uint32_t r32(const void* p, ptrdiff_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
int32_t  ri32(const void* p, ptrdiff_t o) { return (int32_t)r32(p, o); }
void*    rp(const void* p, ptrdiff_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
void     w32(void* p, ptrdiff_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
void     wp(void* p, ptrdiff_t o, void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

int32_t g_gameSlots[1025];      // 0x6FD30C04 (current game) then 0x6FD30C08.. game id per slot
void*   g_curGame;              // 0x6FD30C04
void*   g_taskTab[32];          // 0x6FD30318
uint32_t g_taskA, g_taskB;      // 0x6FD303A8 / 0x6FD303AC
}

struct B66Seams {
    int32_t (*ord10050)(void);                    // Ordinal_10050: game-guid of the caller
    void*   (*getGame)(int32_t guid);             // 0x2AAE0, guid in a register
    int32_t (*fn4A680)(void* game);               // 0x4A680, game in a register
    uint64_t (*fn72020)(void);                    // 0x52020
    void    (*fn21E50)(void);                     // 0x1E50
    void    (*ord10046)(const char*, int, int);   // Ordinal_10046: free(ptr-less form, file, line, 0)
    const char* (*ord10920)(const void* in);      // Ordinal_10920 (register input = ESI record)
    int32_t (*ord10867)(void* p);
    int32_t (*ord10750)(void* p);
};
B66Seams g_b66 = {};

// 1.13c D2Game+0x2B440
int32_t b66_2B440(void) {
    g_b66.ord10050();
    void* game = g_b66.getGame(g_b66.ord10050());
    int32_t r = 0;
    if (game) { r = g_b66.fn4A680(game); if (!rp(game, 0x18)) fatal(0x100f); }
    return r;
}

// 1.13c D2Game+0x2BD70
uint32_t b66_2BD70(void) {
    g_b66.ord10050();
    g_curGame = g_b66.getGame(g_b66.ord10050());
    if (!g_curGame) return 0;
    uint32_t v = r32(g_curGame, 0x7c);
    if (!rp(g_curGame, 0x18)) fatal(0x100f);
    return v;
}

// 1.13c D2Game+0x522D0  (ECX = slot key, EDX = out ptr)
int32_t b66_522D0(uint32_t key, void** out) {
    int32_t r = 100;
    *out = nullptr;
    void* t = g_taskTab[key & 0x1f];
    if (!t) return 100;
    uint8_t* node = (uint8_t*)rp(t, 0xc);
    if (node != (uint8_t*)t + 8 && node) {
        uint64_t u = g_b66.fn72020();
        r = (int32_t)(r32(node, -12) - (int32_t)u);
        if (r < 1) { g_b66.fn21E50(); *out = node - 12; }
    }
    if (r32(t, 0) > 0x1f) fatal(0x96);
    return r;
}

// 1.13c D2Game+0x52090
void b66_52090(void) {
    for (int i = 0; i < 32; i++) {
        void* t = g_taskTab[i];
        if (t) {
            uint8_t* n = (uint8_t*)t + 0x10;
            void* next = rp(n, 0);
            void* prev = rp(t, 0x14);
            wp(next, 4, prev);
            wp(prev, 0, next);
            g_b66.ord10046("..\\Source\\D2Game\\GAME\\Task.cpp", 0x32d, 0);
            g_taskTab[i] = nullptr;
        }
    }
    g_taskA = 0; g_taskB = 0;
}

// 1.13c D2Game+0x2AF80
void b66_2AF80(int32_t* o) {
    o[0] = o[1] = o[2] = o[3] = 0;
    for (int s = 1; s <= 1024; s++) {
        if (g_gameSlots[s] == 0 || g_gameSlots[s] == -1) continue;
        void* g = g_b66.getGame(g_gameSlots[s]);
        if (!g) continue;
        for (int i = 0; i < 0x80; i++) {
            uint8_t* h = (uint8_t*)g + 0x1120 + i * 4;
            for (void* u = rp(h, 0x200); u; u = rp(u, 0xe4)) o[1]++;
            for (void* u = rp(h, 0); u; u = rp(u, 0xe4)) o[3]++;
        }
        if (!rp(g, 0x18)) fatal(0x100f);
        o[0]++;
    }
}

// 1.13c D2Game+0x2B4D0
int32_t b66_2B4D0(int16_t* out, int32_t max) {
    int32_t n = 0;
    for (int s = 1; s <= 1024 && n < max; s++) {
        if (g_gameSlots[s] == 0 || g_gameSlots[s] == -1) continue;
        void* g = g_b66.getGame(g_gameSlots[s]);
        if (!g) continue;
        out[n++] = (int16_t)r32(g, 0x28);
        if (!rp(g, 0x18)) fatal(0x100f);
    }
    return n;
}

// 1.13c D2Game+0xDFB30  (EDI = dst, ESI = src; Ordinal_10920 register input taken as src)
void b66_DFB30(uint8_t* dst, uint8_t* src) {
    if (!dst) fatal(0x9ab);
    if (!src) fatal(0x9ac);
    const char* name;
    if (r32(src, 0) == 0 && (name = g_b66.ord10920(src)) != nullptr) {
        char* d = (char*)dst + 0x24;
        do { *d = *name; } while (*name++ && d++);
    } else dst[0x24] = 0;
    w32(dst, 0, r32(src, 0));
    w32(dst, 4, r32(src, 4));
    w32(dst, 8, r32(src, 12));
    w32(dst, 12, r32(src, 16));
    int32_t t = ri32(src, 0);
    void* p = rp(src, 0x2c);
    bool direct = t == 2 || (t > 3 && t < 6);
    w32(dst, 16, direct ? r32(p, 0xc) : p ? (uint32_t)g_b66.ord10867(p) : 0);
    w32(dst, 20, direct ? r32(p, 0x10) : p ? (uint32_t)g_b66.ord10750(p) : 0);
    w32(dst, 24, r32(src, 0x94));
    w32(dst, 28, r32(src, 0x98));
}

// 1.13c D2Game+0x49FE0  (EAX = game, ECX = id)
void* b66_49FE0(void* game, int32_t id) {
    if (!game) fatal(0xe9);
    for (void* c = rp(game, 0x88); c; c = rp(c, 0x12a * 4))
        if (ri32(c, 0) == id) return c;
    return nullptr;
}
