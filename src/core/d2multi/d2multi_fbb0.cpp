// 1.13c D2Multi+0xFBB0: stdcall (const char* a, const char* b, int v1, int v2). Widens a and b into two local 576-character Unicode buffers and strips
// colour tokens from both (0xEB70), then 0xEC80(v1) and 0xED60(v2) with EBX = the first buffer, appends the second to the first, and when
// DAT_6fa0a0c8 is set calls Ordinal_10000(v2, 0, 1) with EDX = the first buffer.
#include "d2multi_env.h"
#include "d2multi_ucs.h"

void d2multi_fbb0(const D2mEnv& e, const uint8_t* a, const uint8_t* b, uint32_t v1, uint32_t v2, uint32_t dat0a0c8) {
    uint16_t u1[576], u2[576];
    int n = 0;
    while (a[n] && n < 575) { u1[n] = a[n]; n++; }
    u1[n] = 0;
    n = 0;
    while (b[n] && n < 575) { u2[n] = b[n]; n++; }
    u2[n] = 0;
    d2m_call(e, 0xeb70, 0, 0, 0, 0, 0, d2m_ptr(u1));
    d2m_call(e, 0xeb70, 0, 0, 0, 0, 0, d2m_ptr(u2));
    d2m_call(e, 0xec80, 0, 0, 0, d2m_ptr(u1), 0, v1);
    d2m_call(e, 0xed60, 0, 0, 0, d2m_ptr(u1), 0, v2);
    d2m_ucat(u1, u2);
    if (dat0a0c8) d2m_call(e, 0x7ce0, 0, 0, d2m_ptr(u1), 0, 0, v2, 0, 1);
}
