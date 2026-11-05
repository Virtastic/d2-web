// Probe for the host-string branch row. The host string comes in through a buffer; the seams record each call in order.
#include <emscripten.h>
#include <cstdint>
#include "d2game_hooklog.h"

// C++ linkage: the port is declared outside extern "C".
void d2launch_sub_12560(const char* (*gethost)(void), void (*a)(void), void (*b)(void),
                        void (*c)(void), void (*d)(void), void (*lbl)(void));

static D2HookLog* g_lg;
static const char* g_ip;
static const char* imp97c6(void) { g_lg->rec(0x97c6); return g_ip; }
static void imp996a(void) { g_lg->rec(0x996a); }
static void imp9b32(void) { g_lg->rec(0x9b32); }
static void imp98c8(void) { g_lg->rec(0x98c8); }
static void imp9862(void) { g_lg->rec(0x9862); }
static void imp9976(void) { g_lg->rec(0x9976); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_12560(uint32_t* r) {
    D2HookLog lg(r[14]);
    g_lg = &lg;
    g_ip = (const char*)(uintptr_t)r[16];
    d2launch_sub_12560(imp97c6, imp996a, imp9b32, imp98c8, imp9862, imp9976);
}
