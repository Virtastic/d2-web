// Decompile-derived, 1.13c-only. Seams: Fog pool free (#10046) and D2Common #10850 injected.
#include <cstdint>
#include <cstring>

struct D2Game1ebd0Env {
    void (*freePool)(void* pPool, void* p, const char* file, int line);  // Fog #10046
    void (*freeAux)(void* p);                                            // D2Common #10850
};

static void* rp(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

// 1.13c D2Game+0x1EBD0
// EAX = inactive-unit list (pool at +0x08, node chain head at +0x0C, node next at +0x58).
extern "C" void d2game_free_inactive_chain_1ebd0(void* list, const D2Game1ebd0Env* env)
{
    void* pool = rp(list, 0x08);
    for (void* n = rp(list, 0x0C); n;) {
        void* next = rp(n, 0x58);
        for (void* i = rp(n, 0x20); i;) {
            void* in = rp(i, 0x04);
            env->freePool(pool, i, "..\\Source\\D2Game\\Ai\\AiGeneral.cpp", 0x246);
            i = in;
        }
        if (rp(n, 0x24)) env->freeAux(rp(n, 0x24));
        env->freePool(pool, n, "..\\Source\\D2Game\\UNIT\\SUnitInactive.cpp", 0x8D);
        n = next;
    }
}
