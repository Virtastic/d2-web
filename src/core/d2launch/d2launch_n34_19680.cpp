#include <cstdint>
extern "C" {
void d2launch_sub_11CB0(void);
int32_t d2launch_ord_10191(void);
void d2launch_sub_14920(char* buf, const char* fmt, const char* res);
void d2launch_ord_10027(char*, int32_t, int32_t);
void d2launch_sub_18160(void);
}
struct D2l19680Seams { void (*f11cb0)(); int (*fog10191)(); void (*f14920)(char*, const char*, const char*); void (*play)(char*, uint32_t, uint32_t); void (*f18160)(); };
extern "C" void d2l_19680_core(const char* fmt, const D2l19680Seams* s);
static int n34_fog() { return d2launch_ord_10191(); }
static void n34_play(char* b, uint32_t m, uint32_t z) { d2launch_ord_10027(b, (int32_t)m, (int32_t)z); }
// 1.13c D2Launch+0x19680 -- core in d2launch_19680.cpp (resolution pick, video path, play, rebuild menu)
extern "C" void d2launch_sub_19680(void) {
    static const D2l19680Seams S = { d2launch_sub_11CB0, n34_fog, d2launch_sub_14920, n34_play, d2launch_sub_18160 };
    d2l_19680_core("%s\\video\\%s\\d2intro%s.bik", &S);
}
