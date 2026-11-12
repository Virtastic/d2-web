// 1.13c D2Multi+0x14850 env form. The globals and callees arrive in the env so the
// FUNCTION-mode probe can drive them; the production port is d2multi_14850 in d2multi_batch_n27.cpp.
// Retail: state=3, count=0; if session byte 0x35a != 2 tail-calls 0x13F20; else flag = 0x800 when session
// byte 0x1EB has bit 2, bump the 16-bit request counter (wrapping past 0 to 1), call the realm-client (MCP client)
// module's exported request call 10060, pend = counter. That request call is register-passed: ECX = a six-dword block on this frame (five zeros, then the flag), EDX = the
// dword at the counter (its low half is the counter); it pops no stack words. The counter and pend stores are
// 16-bit (`mov [..], ax`). The function is void but leaves EAX: on the 13F20 path whatever 13F20 returns; on the
// request path 10060's EAX with its low half replaced by the counter.
#include <cstdint>

struct D2m14850Env {
    int32_t*  g_state20;   // DAT_6fa09f20
    int32_t*  g_cnt_dcc;   // DAT_6fa09dcc
    uint8_t** g_sess;      // DAT_6fa09f14 (session struct pointer)
    uint16_t* g_ctr;       // DAT_6f9e9c34 (16-bit request counter)
    int32_t*  g_pend;      // DAT_6fa09f18 (low half written)
    uint32_t (*sub13f20)(void);
    uint32_t (*ord10060)(const int32_t* blk, uint32_t edx);
};

extern "C" uint32_t d2multi_14850_env(const D2m14850Env* e) {
    *e->g_state20 = 3;
    *e->g_cnt_dcc = 0;
    uint8_t* sess = *e->g_sess;
    if ((int8_t)sess[0x35a] != 2) return e->sub13f20();
    int32_t blk[6] = { 0, 0, 0, 0, 0, (sess[0x1eb] & 4) ? 0x800 : 0 };
    uint16_t s = (uint16_t)(*e->g_ctr + 1);
    if (s == 0) s = 1;
    *e->g_ctr = s;
    uint32_t eax = e->ord10060(blk, *(const uint32_t*)e->g_ctr);
    s = *e->g_ctr;
    *(uint16_t*)e->g_pend = s;
    return (eax & 0xFFFF0000u) | s;
}
