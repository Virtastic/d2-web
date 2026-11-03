// 1.13c D2Multi+0x14850 env form. The globals and callees arrive in the env so the
// FUNCTION-mode probe can drive them; the production port is d2multi_14850 in d2multi_batch_n27.cpp.
// Retail: state=3, count=0; if session byte 0x35a != 2 tail-calls 0x13F20; else flag = 0x800 when session
// byte 0x1EB has bit 2, bump the 16-bit request counter (wrapping past 0 to 1), call D2Win 10060, pend = counter.
#include <cstdint>

struct D2m14850Env {
    int32_t*  g_state20;   // DAT_6fa09f20
    int32_t*  g_cnt_dcc;   // DAT_6fa09dcc
    uint8_t** g_sess;      // DAT_6fa09f14 (session struct pointer)
    uint16_t* g_ctr;       // DAT_6f9e9c34 (16-bit request counter)
    int32_t*  g_pend;      // DAT_6fa09f18
    void (*sub13f20)(void);
    void (*ord10060)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
};

extern "C" void d2multi_14850_env(const D2m14850Env* e) {
    *e->g_state20 = 3;
    *e->g_cnt_dcc = 0;
    uint8_t* sess = *e->g_sess;
    if ((int8_t)sess[0x35a] != 2) { e->sub13f20(); return; }
    int32_t flag = (sess[0x1eb] & 4) ? 0x800 : 0;
    uint16_t s = (uint16_t)(*e->g_ctr + 1);
    if (s == 0) s = 1;
    *e->g_ctr = s;
    e->ord10060(0, 0, 0, 0, 0, flag);
    *e->g_pend = (int32_t)(int16_t)s;
}
