// Probe for the registry seed row. The first word is the result of the registry read for this case; the seams record the
// registry read, the formatting call and the registry write in order. A stack pointer is logged as a stack tag.
#include <emscripten.h>
#include <emscripten/stack.h>
#include <cstdint>
#include <cstring>
#include "d2game_hooklog.h"

extern "C" int32_t d2launch_sub_12940();

static D2HookLog* g_lg;
static int32_t g_ret422;

static uint32_t tagp(const void* p) {
    uint32_t v = (uint32_t)(uintptr_t)p;
    if (v >= (uint32_t)emscripten_stack_get_end() && v < (uint32_t)emscripten_stack_get_base()) return 0xB3000000u;
    return v;
}
static uint32_t lit(const char* s) {
    if (!strcmp(s, "Diablo II")) return 0xB2000000u | 0x1E204u;
    if (!strcmp(s, "Aux Battle.net")) return 0xB2000000u | 0x1E210u;
    return 0xFFFFFFFFu;
}

int32_t Ordinal_422(const char* a, const char* b, int32_t c, void* d, int32_t) {
    g_lg->rec(0x9754, 0, 0, 0, 0, 0, lit(a), lit(b), (uint32_t)c, tagp(d));
    return g_ret422;
}
void Ordinal_425(const char* a, const char* b, int32_t c, void* d) {
    g_lg->rec(0x101b, 0, 0, 0, 0, 0, tagp(d), 0xB2000000u | 0x1E1F8u, 0xd8u, 0x94u);
    g_lg->rec(0x9796, 0, 0, 0, 0, 0, lit(a), lit(b), (uint32_t)c, tagp(d));
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_12940(uint32_t* r) {
    D2HookLog lg(r[14]);
    g_lg = &lg;
    g_ret422 = (int32_t)r[0];
    r[4] = (uint32_t)d2launch_sub_12940();
}
