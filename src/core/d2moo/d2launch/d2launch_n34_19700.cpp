#include <cstdint>
#include <cstdlib>
#include <unistd.h>
extern "C" {
extern uint8_t* g_d2launch_state;  // DAT_6fa65abc
int32_t d2launch_ord_10265(int32_t);
void d2launch_ord_10024(const char*, int32_t);
void d2launch_ord_10149(int32_t);
int32_t d2launch_ord_10148(void);
int32_t d2launch_ord_10191(void);
int32_t d2launch_ord_10227(void);
void d2launch_ord_10027(const char*, int32_t, int32_t);
void d2launch_ord_10141(int32_t);
void d2launch_sub_15D30(void);  // 0x15D30
void d2launch_sub_11CB0(void);
int32_t d2launch_sub_12940(void);  // 0x12940 (decomp: FUN_6fa52940)
uint8_t d2launch_sub_128D0(void);
void d2launch_sub_12770(void);
void d2launch_sub_19680(void);
void d2launch_sub_18160(void);
}
// 1.13c D2Launch+0x19700 (core d2l_19700_core in d2launch_19700.cpp)
struct D2l19700Seams { void (*win10149)(uint32_t); void (*f15d30)(); void (*f11cb0)(); int (*win10148)(); int (*fog10191)(); void (*gfx10027)(uint32_t, uint32_t, uint32_t); int (*f12940)(); int (*fog10227)(); int (*f128d0)(); void (*f12770)(); void (*f19680)(); void (*win10141)(uint32_t); void (*f18160)(); void (*fatal)(uint32_t); };
extern "C" void d2l_19700_core(const uint8_t*, const uint32_t[4], const D2l19700Seams*);
static void s149(uint32_t v) { d2launch_ord_10149((int32_t)v); }
static void s141(uint32_t v) { d2launch_ord_10141((int32_t)v); }
static void s27(uint32_t n, uint32_t m, uint32_t z) { d2launch_ord_10027((const char*)(uintptr_t)n, (int32_t)m, (int32_t)z); }
static int s128d0() { return d2launch_sub_128D0(); }
static void sfatal(uint32_t id) { d2launch_ord_10024("", d2launch_ord_10265((int32_t)id)); _exit(-1); }
extern "C" void d2launch_sub_19700(void) {
    static const D2l19700Seams S = { s149, d2launch_sub_15D30, d2launch_sub_11CB0, d2launch_ord_10148, d2launch_ord_10191, s27, d2launch_sub_12940,
                                     d2launch_ord_10227, s128d0, d2launch_sub_12770, d2launch_sub_19680, s141, d2launch_sub_18160, sfatal };
    static const char* const N[4] = { "Data\\Local\\Video\\New_BLIZ640x240.bik", "Data\\Local\\Video\\BlizNorth640x240.bik",
                                      "Data\\Local\\Video\\New_BLIZ640x480.bik", "Data\\Local\\Video\\BlizNorth640x480.bik" };
    const uint32_t n[4] = { (uint32_t)(uintptr_t)N[0], (uint32_t)(uintptr_t)N[1], (uint32_t)(uintptr_t)N[2], (uint32_t)(uintptr_t)N[3] };
    d2l_19700_core(g_d2launch_state, n, &S);
}
