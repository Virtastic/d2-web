#include <cstdint>
extern "C" {
extern int32_t* g_d2launch_bnet;  // DAT_6fa656c4
extern uint32_t g_d2launch_25500;  // DAT_6fa65500, ECX of D2Win#10027
int32_t d2launch_ord_10027(void) __asm__("d2launch_ord_10027_v");  // distinct import (other files declare 3 args)
int32_t d2launch_ord_425(const char*, const char*, int, const char*);
void d2launch_BNGateway_SetCurGateway(int32_t, int32_t);
const char* d2launch_BNGateway_Realm(int32_t, int32_t);
void d2launch_sub_15D30(void);
void d2launch_sub_17DF0(void);
}
struct D2l18810Seams { uint32_t (*gw)(); int32_t (*win10027)(uint32_t); void (*setGateway)(uint32_t, uint32_t); uint32_t (*realm)(uint32_t, uint32_t);
                       void (*store425)(uint32_t, uint32_t, uint32_t, uint32_t); void (*f15d30)(); void (*f17df0)(); };
extern "C" uint32_t d2l_18810_core(uint32_t, uint32_t, uint32_t, const D2l18810Seams*);
static uint32_t n34_gw() { return (uint32_t)(*(int32_t (**)(void))((uint8_t*)g_d2launch_bnet + 0x98))(); }
static int32_t n34_win(uint32_t) { return d2launch_ord_10027(); }
static void n34_set(uint32_t g, uint32_t i) { d2launch_BNGateway_SetCurGateway((int32_t)g, (int32_t)i); }
static uint32_t n34_realm(uint32_t g, uint32_t i) { return (uint32_t)(uintptr_t)d2launch_BNGateway_Realm((int32_t)g, (int32_t)i); }
static void n34_store(uint32_t a, uint32_t k, uint32_t d, uint32_t v) { d2launch_ord_425((const char*)(uintptr_t)a, (const char*)(uintptr_t)k, (int)d, (const char*)(uintptr_t)v); }
// 1.13c D2Launch+0x18810 -- core in d2launch_18810.cpp: gateway chosen, remember it as "Preferred Realm" (index + 1), rebuild the menu.
extern "C" int32_t d2launch_sub_18810(void) {
    static const D2l18810Seams S = { n34_gw, n34_win, n34_set, n34_realm, n34_store, d2launch_sub_15D30, d2launch_sub_17DF0 };
    return (int32_t)d2l_18810_core(g_d2launch_25500, (uint32_t)(uintptr_t)"Diablo II", (uint32_t)(uintptr_t)"Preferred Realm", &S);
}
